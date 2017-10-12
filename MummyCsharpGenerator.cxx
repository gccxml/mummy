//----------------------------------------------------------------------------
//
//  $Id: MummyCsharpGenerator.cxx 517 2010-11-11 15:00:41Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2010-11-11 10:00:41 -0500 (Thu, 11 Nov 2010) $
//  $Revision: 517 $
//
//  Copyright (C) 2006-2009 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MummyCsharpGenerator.h"
#include "MummyLineOrientedTextFileReader.h"
#include "MummyLog.h"
#include "MummySettings.h"

#include "cableArrayType.h"
#include "cableClass.h"
#include "cableClassType.h"
#include "cableConstructor.h"
#include "cableEnumeration.h"
#include "cableEnumerationType.h"
#include "cableField.h"
#include "cableFunctionType.h"
#include "cableFundamentalType.h"
#include "cableMethod.h"
#include "cablePointerType.h"
#include "cableReferenceType.h"
#include "cableType.h"
#include "cableTypedef.h"

#include "cxxFundamentalType.h"

#include "gxsys/RegularExpression.hxx"
#include "gxsys/SystemTools.hxx"
#include "gxsys/ios/sstream"
#include "gxsys/stl/algorithm"
#include "gxsys/stl/map"
#include "gxsys/stl/set"
#include "gxsys/stl/string"
#include "gxsys/stl/vector"

#include "string.h" // strstr


//----------------------------------------------------------------------------
MummyCsharpGenerator::MummyCsharpGenerator()
{
  this->CurrentMethodId = 0;
  this->ClassLineNumber = 0;
  //this->MethodIdMap; // empty, as constructed
  //this->TargetInterface; // empty, as constructed
  //this->HintsMap; // empty, as constructed
}


//----------------------------------------------------------------------------
MummyCsharpGenerator::~MummyCsharpGenerator()
{
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::GenerateWrappers()
{
  gxsys_stl::string elDllVariableName(this->GetSettings()->GetGroup());
  elDllVariableName += "EL_dll";

  this->EmitCSharpWrapperClass(*GetStream(), elDllVariableName.c_str(),
    GetTargetClass());

  if (this->GetSettings()->GetVerbose())
    {
    gxsys_stl::map<const gxsys_stl::string, gxsys_stl::string>::iterator it;
    for (it = this->HintsMap.begin(); it != this->HintsMap.end(); ++it)
      {
      LogInfo(mi_VerboseInfo, << it->first << ": '" << it->second << "'");
      }
    }

  return true;
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::SetTargetClass(const cable::Class *c)
{
  this->MummyGenerator::SetTargetClass(c);

  // Ensure HeaderFileReader is always called first with the target class.
  // (Presently, there is only one cached HeaderFileReader and it only reads
  // the file when it is first called... Subsequent calls ignore the input
  // data and simply return the cached HeaderFileReader. Multiple readers
  // may eventually be necessary to support BTX/ETX exclusion fully...)
  //
  this->GetHeaderFileReader(c);

  // Only populate the lookup entries with stuff from the *parent* class
  // (and its parents...) That way, we can use the existence of a signature
  // in the table as evidence that a virtual is being overridden (for C#
  // keyword override purposes) or that a static is being redefined at a
  // more derived level (hence hiding the parent's definition and requiring
  // use of the C# keyword "new" to avoid the warning about hiding...)
  //
  ClearLookupEntries();
  AddLookupEntries(GetWrappableParentClass(c));

  // Cache a map of the externalHints file:
  //
  this->CacheExternalHints(this->GetSettings()->GetExternalHints(c));
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::CacheExternalHints(const gxsys_stl::string& hintsfile)
{
  gxsys_stl::vector<gxsys_stl::string> hints;
  gxsys_stl::string line;

  gxsys::RegularExpression re;
  re.compile("([^\t ]+)[\t ]+([^\t ]+)[\t ]+([^\t ]+)[\t ]+([^\t ]+)");

  MummyLineOrientedTextFileReader reader;
  reader.SetFileName(hintsfile.c_str());

  this->HintsMap.clear();

  unsigned int n = reader.GetNumberOfLines();
  unsigned int i = 0;
  for (i= 1; i<=n; ++i)
  {
    line = reader.GetLine(i);

    if (!line.empty())
      {
      if (re.find(line))
        {
        gxsys_stl::string className = re.match(1);
        gxsys_stl::string methodName = re.match(2);
        gxsys_stl::string type = re.match(3);
        gxsys_stl::string count = re.match(4);
        gxsys_stl::string key(className + "::" + methodName);

        if (this->HintsMap.find(key) == this->HintsMap.end())
          {
          this->HintsMap[key] = line;
          }
        else
          {
          LogFileLineWarningMsg(hintsfile, i, mw_MultipleHints,
            "More than one line in the hints file for " << className
            << "::" << methodName);
          }
        }
      }
  }
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::AddTargetInterface(const gxsys_stl::string& iface)
{
  if (this->TargetInterface != "")
    {
    LogWarning(mw_MultipleTargetInterfaces,
      "AddTargetInterface being called more than once. " <<
      "Implementation currently only supports 1 interface. " <<
      "Clobbering existing target interface: " <<
      this->TargetInterface);
    }

  this->TargetInterface = iface;
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::HasTargetInterface(const char *iface) const
{
  if (iface)
    {
    return this->TargetInterface == iface;
    }

  return false;
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::IsKeyword(const char *p)
{
  static gxsys_stl::set<gxsys_stl::string> keywords;

  if (0 == keywords.size())
    {
    keywords.insert("abstract");
    keywords.insert("as");
    keywords.insert("base");
    keywords.insert("bool");
    keywords.insert("break");
    keywords.insert("byte");
    keywords.insert("case");
    keywords.insert("catch");
    keywords.insert("char");
    keywords.insert("checked");
    keywords.insert("class");
    keywords.insert("const");
    keywords.insert("continue");
    keywords.insert("decimal");
    keywords.insert("default");
    keywords.insert("delegate");
    keywords.insert("do");
    keywords.insert("double");
    keywords.insert("else");
    keywords.insert("enum");
    keywords.insert("event");
    keywords.insert("explicit");
    keywords.insert("extern");
    keywords.insert("false");
    keywords.insert("finally");
    keywords.insert("fixed");
    keywords.insert("float");
    keywords.insert("for");
    keywords.insert("foreach");
    keywords.insert("goto");
    keywords.insert("if");
    keywords.insert("implicit");
    keywords.insert("in");
    keywords.insert("int");
    keywords.insert("interface");
    keywords.insert("internal");
    keywords.insert("is");
    keywords.insert("lock");
    keywords.insert("long");
    keywords.insert("namespace");
    keywords.insert("new");
    keywords.insert("null");
    keywords.insert("object");
    keywords.insert("operator");
    keywords.insert("out");
    keywords.insert("override");
    keywords.insert("params");
    keywords.insert("private");
    keywords.insert("protected");
    keywords.insert("public");
    keywords.insert("readonly");
    keywords.insert("ref");
    keywords.insert("return");
    keywords.insert("sbyte");
    keywords.insert("sealed");
    keywords.insert("short");
    keywords.insert("sizeof");
    keywords.insert("stackalloc");
    keywords.insert("static");
    keywords.insert("string");
    keywords.insert("struct");
    keywords.insert("switch");
    keywords.insert("this");
    keywords.insert("throw");
    keywords.insert("true");
    keywords.insert("try");
    keywords.insert("typeof");
    keywords.insert("uint");
    keywords.insert("ulong");
    keywords.insert("unchecked");
    keywords.insert("unsafe");
    keywords.insert("ushort");
    keywords.insert("using");
    keywords.insert("virtual");
    keywords.insert("void");
    keywords.insert("volatile");
    keywords.insert("while");
    }

  gxsys_stl::set<gxsys_stl::string>::iterator it = keywords.find(gxsys_stl::string(p));
  if (it != keywords.end())
    {
    return true;
    }

  return false;
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::IsReservedMethodName(const gxsys_stl::string &name)
{
  return (
    name == "Equals" ||
    name == "Finalize" ||
    name == "GetHashCode" ||
    name == "GetType" ||
    name == "MemberwiseClone" ||
    name == "Object" ||
    name == "ToString");
}


//----------------------------------------------------------------------------
gxsys_stl::string MummyCsharpGenerator::GetFundamentalTypeString(const cable::Type *t)
{
  gxsys_stl::string s;

  if (cable::Type::FundamentalTypeId == t->GetTypeId())
    {
    switch (cxx::FundamentalType::SafeDownCast(t->GetCxxType().GetType())->GetId())
      {
      case cxx::FundamentalType::UnsignedChar:
        s = "byte";
      break;

      case cxx::FundamentalType::UnsignedShortInt:
        s = "ushort";
      break;

      case cxx::FundamentalType::UnsignedInt:
      case cxx::FundamentalType::UnsignedLongInt:
        s = "uint";
      break;

      case cxx::FundamentalType::SignedChar:
      case cxx::FundamentalType::Char:
        s = "sbyte";
      break;

      case cxx::FundamentalType::ShortInt:
        s = "short";
      break;

      case cxx::FundamentalType::Int:
      case cxx::FundamentalType::LongInt:
        s = "int";
      break;

      case cxx::FundamentalType::Bool:
        s = "bool";
      break;

      case cxx::FundamentalType::Float:
        s = "float";
      break;

      case cxx::FundamentalType::Double:
        s = "double";
      break;

      case cxx::FundamentalType::Void:
        s = "void";
      break;

      case cxx::FundamentalType::UnsignedLongLongInt:
        s = "ulong";
      break;

      case cxx::FundamentalType::LongLongInt:
        s = "long";
      break;

      //case cxx::FundamentalType::WChar_t:
      //case cxx::FundamentalType::LongDouble:
      //case cxx::FundamentalType::ComplexFloat:
      //case cxx::FundamentalType::ComplexDouble:
      //case cxx::FundamentalType::ComplexLongDouble:
      //case cxx::FundamentalType::NumberOfTypes:

      default:
      break;
      }
    }

  if (s == "")
    {
    LogError(me_UnknownFundamentalType,
      << "Unhandled variable type. GetFundamentalTypeString returning the empty string...");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummyCsharpGenerator::GetWrappedMethodName(const cable::Method *m)
{
  gxsys_stl::string name(m->GetName());

  if (IsReservedMethodName(name))
    {
    name = name + "Wrapper";

    LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_ReservedMethodName,
      << "Reserved method name '" << m->GetName() << "' used. Rename it to eliminate this warning...");
    }

  return name;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetWrappedEnumName(const cable::Enumeration *e)
{
  gxsys_stl::string ename(e->GetName());

  // Don't name unnamed enums with invalid C#/C++ identifiers like "$"...
  //
  if (ename == "" ||
      strstr(ename.c_str(), "$"))
    {
    if (e->Begin() != e->End())
      {
      ename = *(e->Begin());
      ename += "_WrapperEnum";
      }
    else
      {
      ename = "WARNING_unnamed_enum";
      }

    LogFileLineWarningMsg(e->GetFile(), e->GetLine(), mw_UnnamedEnum,
      "Unnamed enum found. Name it to eliminate this warning...");
    }

  return ename;
}


//----------------------------------------------------------------------------
bool ExtractTypeAndCountFromHintLine(
  const gxsys_stl::string& hint,
  gxsys_stl::string& type,
  gxsys_stl::string& count
)
{
  gxsys::RegularExpression re;
  re.compile("([^\t ]+)[\t ]+([^\t ]+)[\t ]+([^\t ]+)[\t ]+([^\t ]+)");

  if (re.find(hint))
    {
    //className = re.match(1);
    //methodName = re.match(2);
    type = re.match(3);
    count = re.match(4);
    return true;
    }

  return false;
}


//----------------------------------------------------------------------------
bool ReturnTypeMatchesHintType(
  cable::Type *t,
  const gxsys_stl::string& type
  )
{
  gxsys_stl::string utype = gxsys::SystemTools::UpperCase(type);

  //  Init with the "not a real enum value" value:
  //
  cxx::FundamentalType::Id ftid = cxx::FundamentalType::NumberOfTypes;

  // Values of 'type' currently present in the VTK hints file are:
  //
  if      (utype ==  "301") { ftid = cxx::FundamentalType::Float; }
  else if (utype == "303") { ftid = cxx::FundamentalType::Char; }
  else if (utype ==  "304") { ftid = cxx::FundamentalType::Int; }
  else if (utype == "305") { ftid = cxx::FundamentalType::ShortInt; }
  else if (utype == "306") { ftid = cxx::FundamentalType::LongInt; }
  else if (utype ==  "307") { ftid = cxx::FundamentalType::Double; }
  else if (utype == "313") { ftid = cxx::FundamentalType::UnsignedChar; }
  else if (utype == "314") { ftid = cxx::FundamentalType::UnsignedInt; }
  else if (utype == "315") { ftid = cxx::FundamentalType::UnsignedShortInt; }
  else if (utype == "316") { ftid = cxx::FundamentalType::UnsignedLongInt; }
  else if (utype ==  "30A") { ftid = cxx::FundamentalType::Int; } // vtkIdType (could be 32 or 64 bit...)
  else if (utype == "30B") { ftid = cxx::FundamentalType::LongLongInt; } // vtkIdType (could be 32 or 64 bit...)
  else if (utype == "30D") { ftid = cxx::FundamentalType::SignedChar; }
  else if (utype == "31B") { ftid = cxx::FundamentalType::UnsignedLongLongInt; }
  else if (utype == "2307") { ftid = cxx::FundamentalType::Double; } // 0x2307 == double* + something?

  if (cxx::FundamentalType::NumberOfTypes == ftid)
    {
    LogWarning(mw_UnknownHintDataType, "Unknown externalHints data type string '" << type << "'");
    }
  else if (IsFundamentalPointer(t, ftid))
    {
    return true;
    }

  return false;
}


//----------------------------------------------------------------------------
bool ExtractCountFromMethodDeclarationLine(
  const gxsys_stl::string& line,
  gxsys_stl::string& count
)
{
  // Grudgingly allow this VTK-ism to creep into the mummy codebase to avoid
  // introducing a list of regular expressions in the MummySettings.xml file
  // just for VTK-wrapping-hint-based array size specification...
  //
  // In VTK, in addition to the explicit external hints file VTK/Wrapping/hints,
  // the following macros from VTK/Common/vtkSetGet.h are also recognized as
  // implicitly having hint sizes associated with them:
  //
  gxsys::RegularExpression re;

  re.compile("^[\t ]*vtkGetVector([0-9]+)Macro\\(.*,.*\\)");
  if (re.find(line))
    {
    count = re.match(1);
    return true;
    }

  re.compile("^[\t ]*vtkGetVectorMacro\\(.*,.*,.*([0-9]+).*\\)");
  if (re.find(line))
    {
    count = re.match(1);
    return true;
    }

  re.compile("^[\t ]*vtkViewportCoordinateMacro\\(.*\\)");
  if (re.find(line))
    {
    count = "2";
    return true;
    }

  re.compile("^[\t ]*vtkWorldCoordinateMacro\\(.*\\)");
  if (re.find(line))
    {
    count = "3";
    return true;
    }

  return false;
}


//----------------------------------------------------------------------------
// Callers of GetMethodArgumentArraySize pass this value as "i" to query
// about the return value of the method. Otherwise, "i" is presumed to be an
// index (0 to cArgs-1) of one of the method's arguments.
#define RETURN_VALUE (0x84848484)


//----------------------------------------------------------------------------
gxsys_stl::string MummyCsharpGenerator::GetMethodArgumentArraySize(const cable::Class *c,
  const cable::Method *m, const cable::FunctionType *ft, unsigned int i)
{
  gxsys_stl::string argArraySize;
  cable::Type *retType = 0;

  // First check for an attribute directly in the source code:
  //
  gxsys_stl::string atts;

  if (RETURN_VALUE == i)
    {
    atts = m->GetAttributes();
    }
  else
    {
    atts = ft->GetArgumentAttributes(i);
    }

  if (atts != "")
    {
    argArraySize = ExtractArraySize(atts);
    }

  // If no direct hint, check the externalHints file, if any:
  //
  if ((argArraySize == "") && (this->HintsMap.size() > 0))
    {
    // The VTK/Wrapping/hints file only tells us about the size of the pointer
    // returned by the method. Only return a non-empty array size from a
    // VTK/Wrapping/hints-style file if this is for a return value of a method:
    //
    if (RETURN_VALUE == i)
      {
      retType = ft->GetReturns();
      if (!IsVoid(retType))
        {
        gxsys_stl::string methodName(m->GetName());
        gxsys_stl::map<const gxsys_stl::string, gxsys_stl::string>::iterator it;

        const cable::Class *cIt = c;
        while (cIt != NULL && (argArraySize == ""))
          {
          gxsys_stl::string fullClassName(GetFullyQualifiedNameForCPlusPlus(cIt));
          gxsys_stl::string key(fullClassName + "::" + methodName);
          gxsys_stl::string hintline;

          it = this->HintsMap.find(key);
          if (it != this->HintsMap.end())
            {
            hintline = it->second;

            gxsys_stl::string type;
            gxsys_stl::string count;
            if (ExtractTypeAndCountFromHintLine(hintline, type, count) &&
              ReturnTypeMatchesHintType(retType, type))
              {
              argArraySize = count;
              LogVerboseInfo("using external array size hint: " << argArraySize << " " << key);
              }
            }

          // If not found yet, keep looking up at parent class hints to see
          // if it is inherited. Method name and return type should still match
          // even if the hint is at the parent class level.
          //
          if (argArraySize == "")
            {
            cIt = GetParentClass(cIt);
            }
          }
        }
      }
    }

  // If still no hint, see if the line declaring the method in the header
  // file uses a macro that gives us a hint about the size of the array...
  //
  if (argArraySize == "")
    {
    if (RETURN_VALUE == i)
      {
      retType = ft->GetReturns();
      if (!IsVoid(retType) && !IsObjectPointer(retType))
        {
        gxsys_stl::string line(this->GetHeaderFileReader(c)->GetLine(m->GetLine()));
        gxsys_stl::string count;

        if (ExtractCountFromMethodDeclarationLine(line, count))
          {
          argArraySize = count;

          if (this->GetSettings()->GetVerbose())
            {
            LogFileLineInfoMsg(c->GetFile(), m->GetLine(), mi_VerboseInfo,
              "inferred array size hint '" << count <<
              "' from method declaration '" << line << "'");
            }
          }
        }
      }
    }

  return argArraySize;
}


//----------------------------------------------------------------------------
//
// GetMethodSignature is very similar to EmitCSharpMethodDeclaration.
// Changes in either may need to be made in both places...
//
gxsys_stl::string MummyCsharpGenerator::GetMethodSignature(const cable::Class *c, const cable::Method *m)
{
  gxsys_ios::ostringstream os;

  cable::FunctionType *ft = m->GetFunctionType();
  unsigned int cArgs = ft->GetNumberOfArguments();
  unsigned int i = 0;
  cable::Type *argType = 0;

  // Method name:
  Emit(os, GetWrappedMethodName(m).c_str());

  // Open args:
  Emit(os, "(");

  // The C# arg types:
  for (i= 0; i<cArgs; ++i)
    {
    argType = ft->GetArgument(i);

    // Is arg an array?
    //
    gxsys_stl::string argArraySize(GetMethodArgumentArraySize(c, m, ft, i));

    // arg type:
    Emit(os, GetCSharpTypeString(argType, false, argArraySize != "").c_str());

    // array notation:
    if (argArraySize != "")
      {
      Emit(os, "[]");
      }

    if (i<cArgs-1)
      {
      Emit(os, ", ");
      }
    }

  // Close args:
  Emit(os, ")");

  return os.str();
}


//----------------------------------------------------------------------------
const char *MummyCsharpGenerator::GetArgName(cable::FunctionType *ftype, unsigned int i)
{
  const char *p = ftype->GetArgumentName(i);

  if (p && *p && !IsKeyword(p))
    {
    return p;
    }

  // Hacky, but for now:
  static char buf[32];
  sprintf(buf, "arg%u", i);
  return buf;
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::FundamentalTypeIsWrappable(const cable::Type* t)
{
  if (cable::Type::FundamentalTypeId == t->GetTypeId())
    {
    switch (cxx::FundamentalType::SafeDownCast(t->GetCxxType().GetType())->GetId())
      {
      case cxx::FundamentalType::UnsignedChar:
      case cxx::FundamentalType::UnsignedShortInt:
      case cxx::FundamentalType::UnsignedInt:
      case cxx::FundamentalType::UnsignedLongInt:
      case cxx::FundamentalType::SignedChar:
      case cxx::FundamentalType::Char:
      case cxx::FundamentalType::ShortInt:
      case cxx::FundamentalType::Int:
      case cxx::FundamentalType::LongInt:
      case cxx::FundamentalType::Bool:
      case cxx::FundamentalType::Float:
      case cxx::FundamentalType::Double:
      case cxx::FundamentalType::Void:
      case cxx::FundamentalType::UnsignedLongLongInt:
      case cxx::FundamentalType::LongLongInt:
        return true;
      break;

      case cxx::FundamentalType::WChar_t:
      case cxx::FundamentalType::LongDouble:
      case cxx::FundamentalType::ComplexFloat:
      case cxx::FundamentalType::ComplexDouble:
      case cxx::FundamentalType::ComplexLongDouble:
      case cxx::FundamentalType::NumberOfTypes:
      default:
      break;
      }
    }

  return false;
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::TypeIsWrappable(const cable::Type* t)
{
  bool wrappable = false;
  cable::Class* enum_class;

  switch (t->GetTypeId())
    {
    case cable::Type::EnumerationTypeId:
      wrappable = true;
	  enum_class = cable::Class::SafeDownCast(cable::EnumerationType::SafeDownCast(t)->GetEnumeration()->GetContext());
	  if (enum_class) 
	  {
		  //if the enumeration has class scope, then the class must be wrappable too
		  wrappable = ClassIsWrappable(enum_class);
	  }
    break;

    case cable::Type::FundamentalTypeId:
      wrappable = FundamentalTypeIsWrappable(t);
    break;

    case cable::Type::ArrayTypeId:
      wrappable = false;//TypeIsWrappable(cable::ArrayType::SafeDownCast(t)->GetTarget());
    break;

    case cable::Type::ClassTypeId:
      wrappable = ClassIsWrappable(cable::ClassType::SafeDownCast(t)->GetClass());
    break;

    case cable::Type::PointerTypeId:
      {
      cable::Type *nested_type = cable::PointerType::SafeDownCast(t)->GetTarget();
      cable::Type::TypeIdType nested_type_id = nested_type->GetTypeId();

      // Only pointers to enums, fundamentals and objects are wrappable
      // and then only if the nested type is also wrappable...
      //
      // A function pointer is "nearly" wrappable (as a delegate definition)
      // if its return type and all of its argument types are wrappable.
      // And it has a typedef that names it within this class (because
      // that typedef is the thing that generates the delegate def in
      // the C# class...)
      //
      // (This is what prevents pointers to pointers from being wrapped
      // in the general case...)
      //
      if (cable::Type::EnumerationTypeId == nested_type_id ||
        cable::Type::FundamentalTypeId == nested_type_id ||
        cable::Type::ClassTypeId == nested_type_id)
        {
        wrappable = TypeIsWrappable(nested_type);
        }
      else if (cable::Type::FunctionTypeId == nested_type_id)
        {
        gxsys_stl::string s;

        // Only really wrappable if nested_type is wrappable and an
        // equivalent typedef name exists:
        //
        wrappable = TypeIsWrappable(nested_type) &&
          EquivalentTypedefNameExists(this->GetTargetClass(),
            cable::FunctionType::SafeDownCast(nested_type), s);
        }
      else
        {
        wrappable = false; // as initialized...
        }
      }
    break;

    case cable::Type::ReferenceTypeId:
      {
      cable::Type *nested_type = cable::ReferenceType::SafeDownCast(t)->GetTarget();
      cable::Type::TypeIdType nested_type_id = nested_type->GetTypeId();

      wrappable = false; // as initialized...

      if (cable::Type::EnumerationTypeId == nested_type_id ||
        cable::Type::FundamentalTypeId == nested_type_id ||
        cable::Type::ClassTypeId == nested_type_id)
        {
        wrappable = TypeIsWrappable(nested_type);
        }
      else if (cable::Type::PointerTypeId == nested_type_id)
        {
        // References to pointers are allowed, but only if the pointer points
        // to a wrappable class...
        //
        cable::Type *doubly_nested_type = cable::PointerType::SafeDownCast(nested_type)->GetTarget();
        cable::Type::TypeIdType doubly_nested_type_id = doubly_nested_type->GetTypeId();

        if (cable::Type::ClassTypeId == doubly_nested_type_id)
          {
          wrappable = TypeIsWrappable(doubly_nested_type);
          }
        }
      }
    break;

    case cable::Type::OffsetTypeId:
    case cable::Type::MethodTypeId:
      wrappable = false;
    break;

    case cable::Type::FunctionTypeId:
      wrappable = FunctionTypeIsWrappable(cable::FunctionType::SafeDownCast(t));
    break;

    default:
      wrappable = false;
    break;
    }

  return wrappable;
}


//----------------------------------------------------------------------------
bool IsCxxMainStyleParamPair(const cable::FunctionType* ft, unsigned int i)
{
  // Special case C++ "main" style functions, as indicated by *this* *exact*
  // *signature* including arg names:
  //   (..., int argc, char* argv[], ...)
  //
  if (i < ft->GetNumberOfArguments()-1 &&
    gxsys_stl::string("argc") == ft->GetArgumentName(i) &&
    cable::Type::FundamentalTypeId == ft->GetArgument(i)->GetTypeId() &&
    cxx::FundamentalType::Int == cxx::FundamentalType::SafeDownCast(ft->GetArgument(i)->GetCxxType().GetType())->GetId() &&
    gxsys_stl::string("argv") == ft->GetArgumentName(i+1) &&
    IsCharPointerPointer(ft->GetArgument(i+1))
    )
    {
    return true;
    }

  return false;
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::FunctionTypeIsWrappable(const cable::FunctionType* ft)
{
  bool wrappable = true;

  cable::Type *argType = 0;
  cable::Type *retType = 0;
  unsigned int i = 0;
  unsigned int cArgs = ft->GetNumberOfArguments();

  retType = ft->GetReturns();
  wrappable = TypeIsWrappable(retType);

  for (i= 0; wrappable && i<cArgs; ++i)
    {
    argType = ft->GetArgument(i);
    wrappable = TypeIsWrappable(argType);

    // If not wrappable because argType is "char**" but it is a cxx main style
    // param pair, then go ahead and say it is wrappable...
    //
    if (!wrappable && IsCxxMainStyleParamPair(ft, i-1))
      {
      wrappable = true;
      }
    }

  return wrappable;
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::MethodWrappableAsEvent(const cable::Method* m, const cable::Context::Access& access)
{
  bool wrappableAsEvent = false;
  gxsys_stl::string atts;

  if (m)
    {
    if (cable::Context::Public == access)
      {
      if (cable::Function::FunctionId == m->GetFunctionId() ||
        cable::Function::MethodId == m->GetFunctionId())
        {
        wrappableAsEvent = HasAnnotation(m, "iwhEvent");
        }
      }
    }

  return wrappableAsEvent;
}

//----------------------------------------------------------------------------
bool MummyCsharpGenerator::MethodIsWrappable(const cable::Method* m, const cable::Context::Access& access)
{
  bool wrappable = false;
  bool hasDeprecatedAttribute = false;
  bool hasExcludeAttribute = false;
  bool isExcludedViaBtxEtx = false;

  if (m)
    {
    if (cable::Context::Public == access)
      {
      if (cable::Function::FunctionId == m->GetFunctionId() ||
        cable::Function::MethodId == m->GetFunctionId())
        {
        if (FunctionTypeIsWrappable(m->GetFunctionType()))
          {
          wrappable = true;
          }
        }
      }
    }

  if (m && wrappable)
    {
    hasDeprecatedAttribute = HasAttribute(m, "deprecated");
    hasExcludeAttribute = HasAnnotation(m, "iwhExclude");

    if (hasDeprecatedAttribute || hasExcludeAttribute)
      {
      wrappable = false;
      }
    }

  if (m && wrappable)
    {
    cable::Class* c = cable::Class::SafeDownCast(m->GetContext());

    // BTX ETX style exclusion can only be applied to methods of the current target
    // class... We need to add multiple HeaderFileReader support to enable BTX ETX style
    // exclusion fully (including parent class method exclusion for purposes of constructing
    // the initial "what virtual methods do I inherit?" table...)
    //
    if (c && c == this->GetTargetClass() &&
      this->GetHeaderFileReader(c)->IsLineExcluded(m->GetLine()))
      {
      isExcludedViaBtxEtx = true;
      wrappable = false;
      }
    }

  if (this->GetSettings()->GetVerbose())
    {
    if (m)
      {
      if (cable::Context::Public == access)
        {
        if (cable::Function::FunctionId == m->GetFunctionId() ||
          cable::Function::MethodId == m->GetFunctionId())
          {
          if (wrappable)
            {
            LogInfo(mi_VerboseInfo, << m->GetNameOfClass() << " '" << m->GetName()
              << "' is wrappable..." << gxsys_ios::endl);
            }
          else
            {
            if (hasDeprecatedAttribute)
              {
              LogWarning(mw_CouldNotWrap, << m->GetNameOfClass() << " '" << m->GetName()
                << "' could not be wrapped because it is marked with the 'deprecated' attribute..." << gxsys_ios::endl);
              }
            else if (hasExcludeAttribute)
              {
              LogWarning(mw_CouldNotWrap, << m->GetNameOfClass() << " '" << m->GetName()
				  << "' could not be wrapped because it is marked with the 'iwhExclude' annotation..." << gxsys_ios::endl);
              }
            else if (isExcludedViaBtxEtx)
              {
              LogWarning(mw_CouldNotWrap, << m->GetNameOfClass() << " '" << m->GetName()
                << "' could not be wrapped because it is in between begin/end exclude markers (BTX/ETX)..." << gxsys_ios::endl);
              }
            else
              {
              LogWarning(mw_CouldNotWrap, << m->GetNameOfClass() << " '" << m->GetName()
                << "' could not be wrapped because of its return type or one of its arguments' types..." << gxsys_ios::endl);
              }
            }
          }
        else
          {
          LogWarning(mw_CouldNotWrap, << m->GetNameOfClass() << " '" << m->GetName()
            << "' could not be wrapped because it's not a function or method..." << gxsys_ios::endl);
          }
        }
      else
        {
        LogWarning(mw_CouldNotWrap, << m->GetNameOfClass() << " '" << m->GetName()
          << "' is not considered for wrapping because of its non-public access level..." << gxsys_ios::endl);
        }
      }
    else
      {
      LogWarning(mw_CouldNotWrap, << "NULL m!!" << gxsys_ios::endl);
      }
    }

  return wrappable;
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::ClassIsWrappable(const cable::Class* c)
{
  return MummyGenerator::ClassIsWrappable(c);
}


//----------------------------------------------------------------------------
const cable::Class* MummyCsharpGenerator::GetWrappableParentClass(const cable::Class *c)
{
  const cable::Class* wrappableParent = GetParentClass(c);

  while (wrappableParent && !this->ClassIsWrappable(wrappableParent))
    {
    wrappableParent = GetParentClass(wrappableParent);
    }

  return wrappableParent;
}


//----------------------------------------------------------------------------
//
// WARNING: GetIsRefArg, GetPInvokeTypeString and GetCSharpTypeString need to
// stay in sync. Changes in one likely imply that changes in the other are
// also required...
//
// This function needs to stay in sync with the case in GetCSharpTypeString
// where "ref " is prepended to the C# type string... If "ref " is prepended
// then this function should return true.
//
bool MummyCsharpGenerator::GetIsRefArg(const cable::Type *t)
{
  cable::Type *nested_type = 0;

  if (cable::Type::ReferenceTypeId == t->GetTypeId())
    {
    nested_type = cable::ReferenceType::SafeDownCast(t)->GetTarget();
    cable::Type::TypeIdType nested_type_id = nested_type->GetTypeId();

    if (cable::Type::EnumerationTypeId == nested_type_id ||
      cable::Type::FundamentalTypeId == nested_type_id)
      {
      return true;
      }
    }

  return false;
}


//----------------------------------------------------------------------------
gxsys_stl::string /* MummyCsharpGenerator:: */ GetEnumerationTypeString(const cable::Type *t)
{
  gxsys_stl::string s;
  cable::Enumeration* e = cable::EnumerationType::SafeDownCast(t)->GetEnumeration();

  s = GetFullyQualifiedNameForCSharp(e);

  // Don't name unnamed enums with invalid C#/C++ identifiers like "$"...
  //
  if (strstr(s.c_str(), "$"))
    {
    s = "uint /* WARNING_unnamed_enum */";
    }

  return s;
}


//----------------------------------------------------------------------------
//
// WARNING: GetIsRefArg, GetPInvokeTypeString and GetCSharpTypeString need to
// stay in sync. Changes in one likely imply that changes in the other are
// also required...
//
gxsys_stl::string MummyCsharpGenerator::GetPInvokeTypeString(const cable::Type *t, bool forReturn, bool isArray, bool forDelegate)
{
  gxsys_stl::string s;
  cable::Type *nested_type = 0;

  switch (t->GetTypeId())
    {
    case cable::Type::EnumerationTypeId:
      s = GetEnumerationTypeString(t);
    break;

    case cable::Type::FundamentalTypeId:
      s = GetFundamentalTypeString(t);

      // C# byte maps automatically to C++ bool via PInvoke
      //
      if (s == "bool")
        {
        s = "byte";
        }
    break;

    case cable::Type::ArrayTypeId:
      s = "ERROR_ArrayTypeId_not_yet_implemented";
      LogError(me_InternalError, << s.c_str());
    break;

    case cable::Type::ClassTypeId:
      //s = cable::ClassType::SafeDownCast(t)->GetClass()->GetName();
      s = GetWrappedClassNameFullyQualified(cable::ClassType::SafeDownCast(t)->GetClass());
    break;

    case cable::Type::PointerTypeId:
      nested_type = cable::PointerType::SafeDownCast(t)->GetTarget();

      if (IsObject(nested_type))
        {
        if (forReturn || forDelegate)
          {
          s = "IntPtr";
          }
        else
          {
          cable::Class *c = cable::ClassType::SafeDownCast(nested_type)->GetClass();
          if (IsUtilityClass(c))
            {
            //s = c->GetName();
            s = GetWrappedClassNameFullyQualified(c);
            }
          else
            {
            s = "HandleRef";
            }
          }
        }
      else if (IsChar(nested_type))
        {
        if (forReturn)
          {
          s = "IntPtr";
          }
        else
          {
          s = "string";
          }
        }
      else if (IsVoid(nested_type))
        {
        s = "IntPtr";
        }
      else if (cable::Type::FundamentalTypeId == nested_type->GetTypeId())
        {
        if (isArray && !forReturn)
          {
          s = GetPInvokeTypeString(nested_type, forReturn, isArray, forDelegate);
          }
        else
          {
          s = "IntPtr";
          }
        }
      else if (cable::Type::PointerTypeId == nested_type->GetTypeId())
        {
        if (IsCharPointer(nested_type))
          {
          s = "[In, Out] string[]";
          }
        else
          {
          s = "IntPtr /* pointer-to-pointer */";
          }
        }
      else if (cable::Type::FunctionTypeId == nested_type->GetTypeId())
        {
        if (!EquivalentTypedefNameExists(this->GetTargetClass(),
          cable::FunctionType::SafeDownCast(nested_type), s))
          {
          s = "ERROR_No_equivalent_typedef_name_for_function_pointer";
          LogError(me_InternalError, << s.c_str());
          }
        }
      else
        {
        s = "ERROR_PointerTypeId_not_yet_implemented_for_nested_type";
        LogError(me_InternalError, << s.c_str());
        }
    break;

    case cable::Type::ReferenceTypeId:
      {
      nested_type = cable::ReferenceType::SafeDownCast(t)->GetTarget();
      cable::Type::TypeIdType nested_type_id = nested_type->GetTypeId();

      s = "ERROR_ReferenceTypeId_not_yet_implemented_for_nested_type";

      if (cable::Type::EnumerationTypeId == nested_type_id ||
        cable::Type::FundamentalTypeId == nested_type_id ||
        cable::Type::ClassTypeId == nested_type_id)
        {
        s = GetPInvokeTypeString(nested_type, forReturn, isArray, forDelegate);

        if (!isArray && (cable::Type::EnumerationTypeId == nested_type_id ||
          cable::Type::FundamentalTypeId == nested_type_id))
          {
          if (forReturn)
            {
            s = "IntPtr";
            }
          else
            {
            s = gxsys_stl::string("ref ") + s;
            }
          }
        }
      else if (cable::Type::PointerTypeId == nested_type_id)
        {
        // References to pointers are allowed, but only if the pointer points
        // to a wrappable class...
        //
        cable::Type *doubly_nested_type = cable::PointerType::SafeDownCast(nested_type)->GetTarget();
        cable::Type::TypeIdType doubly_nested_type_id = doubly_nested_type->GetTypeId();

        if (cable::Type::ClassTypeId == doubly_nested_type_id)
          {
          s = GetPInvokeTypeString(nested_type, forReturn, isArray, forDelegate);
          }
        }

      if (s == "ERROR_ReferenceTypeId_not_yet_implemented_for_nested_type")
        {
        LogError(me_InternalError, << s.c_str());
        }
      }
    break;

    case cable::Type::OffsetTypeId:
    case cable::Type::MethodTypeId:
    case cable::Type::FunctionTypeId:
    default:
      s = "ERROR_No_CSharp_type_for_cable_Type_TypeId";
      LogError(me_InternalError, << s.c_str());
    break;
    }

  return s;
}


//----------------------------------------------------------------------------
//
// WARNING: GetIsRefArg, GetPInvokeTypeString and GetCSharpTypeString need to
// stay in sync. Changes in one likely imply that changes in the other are
// also required...
//
gxsys_stl::string MummyCsharpGenerator::GetCSharpTypeString(const cable::Type *t, bool forReturn, bool isArray)
{
  gxsys_stl::string s;
  cable::Type *nested_type = 0;

  switch (t->GetTypeId())
    {
    case cable::Type::EnumerationTypeId:
      s = GetEnumerationTypeString(t);
    break;

    case cable::Type::FundamentalTypeId:
      s = GetFundamentalTypeString(t);
    break;

    case cable::Type::ArrayTypeId:
      s = "ERROR_ArrayTypeId_not_yet_implemented";
      LogError(me_InternalError, << s.c_str());
    break;

    case cable::Type::ClassTypeId:
      //s = cable::ClassType::SafeDownCast(t)->GetClass()->GetName();
      s = GetWrappedClassNameFullyQualified(cable::ClassType::SafeDownCast(t)->GetClass());
    break;

    case cable::Type::PointerTypeId:
      nested_type = cable::PointerType::SafeDownCast(t)->GetTarget();

      if (IsObject(nested_type))
        {
        s = GetCSharpTypeString(nested_type, forReturn, isArray);
        }
      else if (IsChar(nested_type))
        {
        s = "string";
        }
      else if (IsVoid(nested_type))
        {
        s = "IntPtr";
        }
      else if (cable::Type::FundamentalTypeId == nested_type->GetTypeId())
        {
        if (isArray)
          {
          s = GetCSharpTypeString(nested_type, forReturn, isArray);
          }
        else
          {
          s = "IntPtr";
          }
        }
      else if (cable::Type::PointerTypeId == nested_type->GetTypeId())
        {
        s = GetCSharpTypeString(nested_type, forReturn, isArray);

        if (IsCharPointer(nested_type))
          {
          s += "[]";
          }
        else
          {
          s += " /* pointer-to-pointer */ ";
          }
        }
      else if (cable::Type::FunctionTypeId == nested_type->GetTypeId())
        {
        s = GetCSharpTypeString(nested_type, forReturn, isArray);
        }
      else
        {
        s = "ERROR_PointerTypeId_not_yet_implemented_for_nested_type";
        LogError(me_InternalError, << s.c_str());
        }
    break;

    case cable::Type::ReferenceTypeId:
      {
      nested_type = cable::ReferenceType::SafeDownCast(t)->GetTarget();
      cable::Type::TypeIdType nested_type_id = nested_type->GetTypeId();

      s = "ERROR_ReferenceTypeId_not_yet_implemented_for_nested_type";

      if (cable::Type::EnumerationTypeId == nested_type_id ||
        cable::Type::FundamentalTypeId == nested_type_id ||
        cable::Type::ClassTypeId == nested_type_id)
        {
        s = GetCSharpTypeString(nested_type, forReturn, isArray);

        if (!isArray && (cable::Type::EnumerationTypeId == nested_type_id ||
          cable::Type::FundamentalTypeId == nested_type_id))
          {
          if (forReturn)
            {
            s = "IntPtr";
            }
          else if (s == "bool")
            {
            // Special case: transform "ref bool" args to "ref byte" args so
            // that we can call the PInvoke method, which changes all "bool"
            // to "byte" -- we don't do it in the non-ref case because it works
            // automatically and "bool" is a nicer C# signature. But in the ref
            // case, we need:
            //
            s = gxsys_stl::string("ref byte");
            }
          else
            {
            s = gxsys_stl::string("ref ") + s;
            }
          }
        }
      else if (cable::Type::PointerTypeId == nested_type_id)
        {
        // References to pointers are allowed, but only if the pointer points
        // to a wrappable class...
        //
        cable::Type *doubly_nested_type = cable::PointerType::SafeDownCast(nested_type)->GetTarget();
        cable::Type::TypeIdType doubly_nested_type_id = doubly_nested_type->GetTypeId();

        if (cable::Type::ClassTypeId == doubly_nested_type_id)
          {
          s = GetCSharpTypeString(doubly_nested_type, forReturn, isArray);
          }
        }

      if (s == "ERROR_ReferenceTypeId_not_yet_implemented_for_nested_type")
        {
        LogError(me_InternalError, << s.c_str());
        }
      }
    break;

    case cable::Type::FunctionTypeId:
      if (!EquivalentTypedefNameExists(this->GetTargetClass(),
        cable::FunctionType::SafeDownCast(t), s))
        {
        s = "ERROR_No_equivalent_typedef_name_for_function_pointer";
        LogError(me_InternalError, << s.c_str());
        }
    break;

    case cable::Type::OffsetTypeId:
    case cable::Type::MethodTypeId:
    default:
      s = "ERROR_No_CSharp_type_for_cable_Type_TypeId";
      LogError(me_InternalError, << s.c_str());
    break;
    }

  return s;
}


//----------------------------------------------------------------------------
class MethodInstance
{
public:
  const cable::Class* Class;
  const cable::Method* Method;

  MethodInstance(const cable::Class* c, const cable::Method* m) :
    Class(c), Method(m)
    {
    }
};


//----------------------------------------------------------------------------
gxsys_stl::map<gxsys_stl::string, MethodInstance> OtherMethods;
gxsys_stl::map<gxsys_stl::string, MethodInstance> StaticMethods;
gxsys_stl::map<gxsys_stl::string, MethodInstance> VirtualMethods;
gxsys_stl::map<gxsys_stl::string, MethodInstance> WrappedMethods;
gxsys_stl::map<gxsys_stl::string, MethodInstance> WrappedEnums; // in this case all MethodInstance.Method members are null


//----------------------------------------------------------------------------
void MummyCsharpGenerator::ClearLookupEntries()
{
  OtherMethods.clear();
  StaticMethods.clear();
  VirtualMethods.clear();
  WrappedMethods.clear();
  WrappedEnums.clear();

  // Pretend that "System.Object" is in our inheritance chain and that its
  // virtual method signatures exist in our lookup map:
  //
//  VirtualMethods.insert(gxsys_stl::make_pair("Equals(object)", MethodInstance(0, 0)));
//  VirtualMethods.insert(gxsys_stl::make_pair("Finalize()", MethodInstance(0, 0)));
//  VirtualMethods.insert(gxsys_stl::make_pair("GetHashCode()", MethodInstance(0, 0)));
//  VirtualMethods.insert(gxsys_stl::make_pair("GetType()", MethodInstance(0, 0)));
//  VirtualMethods.insert(gxsys_stl::make_pair("MemberwiseClone()", MethodInstance(0, 0)));
//  VirtualMethods.insert(gxsys_stl::make_pair("Object()", MethodInstance(0, 0)));
//  VirtualMethods.insert(gxsys_stl::make_pair("ToString()", MethodInstance(0, 0)));
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::AddLookupEntries(const cable::Class* c)
{
  if (c)
    {
    const cable::Class *parent = GetWrappableParentClass(c);

    if (parent)
      {
      AddLookupEntries(parent);
      }

    for(cable::Context::Iterator it = c->Begin(); it != c->End(); ++it)
      {
      cable::Method *m = cable::Method::SafeDownCast(*it);

      if (m && MethodIsWrappable(m, it.GetAccess()))
        {
        if (m->GetVirtual())
          {
          VirtualMethods.insert(gxsys_stl::make_pair(GetMethodSignature(c, m), MethodInstance(c, m)));
          }
        else if (m->GetStatic())
          {
          StaticMethods.insert(gxsys_stl::make_pair(GetMethodSignature(c, m), MethodInstance(c, m)));
          }
        else
          {
          OtherMethods.insert(gxsys_stl::make_pair(GetMethodSignature(c, m), MethodInstance(c, m)));
          }
        }

      if (!m)
        {
        cable::Enumeration *e = cable::Enumeration::SafeDownCast(*it);

        if (e && (cable::Context::Public == it.GetAccess()))
          {
          WrappedEnums.insert(gxsys_stl::make_pair(GetWrappedEnumName(e), MethodInstance(c, 0)));
          }
        }
      }
    }
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::DumpLookupEntries()
{
  gxsys_stl::map<gxsys_stl::string, MethodInstance>::iterator it;

  LogInfo(mi_Info, << "<DumpLookupEntries>");
  LogInfo(mi_Info, << "inherited static methods:");
  for (it= StaticMethods.begin(); it!=StaticMethods.end(); ++it)
    {
    LogInfo(mi_Info, << "  " << it->first);
    }

  LogInfo(mi_Info, << "inherited virtual methods:");
  for (it= VirtualMethods.begin(); it!=VirtualMethods.end(); ++it)
    {
    LogInfo(mi_Info, << "  " << it->first);
    }

  LogInfo(mi_Info, << "inherited other methods:");
  for (it= OtherMethods.begin(); it!=OtherMethods.end(); ++it)
    {
    LogInfo(mi_Info, << "  " << it->first);
    }

  LogInfo(mi_Info, << "wrapped methods:");
  for (it= WrappedMethods.begin(); it!=WrappedMethods.end(); ++it)
    {
    LogInfo(mi_Info, << "  " << it->first);
    }

  LogInfo(mi_Info, << "wrapped enums:");
  for (it= WrappedEnums.begin(); it!=WrappedEnums.end(); ++it)
    {
    LogInfo(mi_Info, << "  " << it->first);
    }

  LogInfo(mi_Info, << "</DumpLookupEntries>");
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::OtherMethodRedefined(const gxsys_stl::string &signature)
{
  gxsys_stl::map<gxsys_stl::string, MethodInstance>::iterator it =
    OtherMethods.find(signature);
  return it != OtherMethods.end();
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::StaticMethodRedefined(const gxsys_stl::string &signature)
{
  gxsys_stl::map<gxsys_stl::string, MethodInstance>::iterator it =
    StaticMethods.find(signature);
  return it != StaticMethods.end();
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::VirtualMethodOverridden(const gxsys_stl::string &signature)
{
  gxsys_stl::map<gxsys_stl::string, MethodInstance>::iterator it =
    VirtualMethods.find(signature);
  return it != VirtualMethods.end();
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::WrappedMethodExists(const gxsys_stl::string &signature)
{
  gxsys_stl::map<gxsys_stl::string, MethodInstance>::iterator it =
    WrappedMethods.find(signature);
  return it != WrappedMethods.end();
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::WrappedEnumExists(const gxsys_stl::string &name)
{
  gxsys_stl::map<gxsys_stl::string, MethodInstance>::iterator it =
    WrappedEnums.find(name);
  return it != WrappedEnums.end();
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::IsFactoryMethod(const cable::Class *c, const cable::Method *m)
{
  if (m->GetStatic() &&
    0 == m->GetFunctionType()->GetNumberOfArguments() &&
    gxsys_stl::string(m->GetName()) == this->GetSettings()->GetFactoryMethod(c))
    {
    return true;
    }

  return false;
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::IsDisposalMethod(const cable::Class *c, const cable::Method *m)
{
  if (m->GetVirtual() &&
    0 == m->GetFunctionType()->GetNumberOfArguments() &&
    gxsys_stl::string(m->GetName()) == this->GetSettings()->GetDisposalMethod(c))
    {
    return true;
    }

  return false;
}


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::MethodReturnValueIsCounted(const cable::Class *c, const cable::Method *m)
{
  if (this->IsFactoryMethod(c, m))
    {
    return true;
    }

  if (HasAnnotation(m, "iwhCounted"))
    {
    return true;
    }

  gxsys_stl::string countedMethodsRegex(this->GetSettings()->GetCountedMethodsRegex(c));
  if (!countedMethodsRegex.empty())
    {
    gxsys::RegularExpression re;
    re.compile(countedMethodsRegex.c_str());
    if (re.find(m->GetName()))
      {
      LogVerboseInfo(
        << "MethodReturnValueIsCounted match:" << gxsys_ios::endl
        << "  countedMethodsRegex: " << countedMethodsRegex << gxsys_ios::endl
        << "  method: " << c->GetName() << "::" << m->GetName()
        );
      return true;
      }
    }

  return false;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummyCsharpGenerator::GetExportLayerFunctionName(const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname)
{
  gxsys_stl::string s(GetFullyQualifiedName(c, "_"));
  s += "_";
  s += mname;

  if (this->IsFactoryMethod(c, m) || this->IsDisposalMethod(c, m) || mname == "new" || mname == "delete")
    {
    // No suffix...
    }
  else
    {
    // Append a unique-fying suffix...
    char idStr[32];
    unsigned int methodId = 0;

    gxsys_stl::map<const cable::Method*, unsigned int>::iterator it = this->MethodIdMap.find(m);
    if (it != this->MethodIdMap.end())
      {
      methodId = it->second;
      }
    else
      {
      // Any methods passed in to here should have been inserted into the
      // MethodIdMap during a previous call to GatherWrappedMethods...
      // What happened...!?!?
      //
      LogError(me_InternalError, << "Could not find method in MethodIdMap.");
      }

    sprintf(idStr, "%02u", methodId);
    s += "_";
    s += idStr;
    }

  return s;
}


//----------------------------------------------------------------------------
//
// GetMethodSignature is very similar to EmitCSharpMethodDeclaration.
// Changes in either may need to be made in both places...
//
void MummyCsharpGenerator::EmitCSharpMethodDeclaration(gxsys_ios::ostream &os, const cable::Class *c, const cable::Method *m, bool asProperty, bool useArg0AsReturn, const gxsys_stl::string& accessLevel)
{
  gxsys_stl::string signature(GetMethodSignature(c, m));

  gxsys_stl::string arraySize;
  gxsys_stl::string s;
  cable::FunctionType *ft = m->GetFunctionType();
  unsigned int cArgs = ft->GetNumberOfArguments();
  unsigned int i = 0;
  cable::Type *argType = 0;
  cable::Type *retType = ft->GetReturns();

  i = RETURN_VALUE;

  // If emitting a declaration based on a "property set" method,
  // the type of the first argument to the set method is the type
  // of the C# property... typically retType will be "void" when
  // this is the case...
  //
  if (asProperty && useArg0AsReturn)
    {
    if (!IsVoid(retType))
      {
      LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_PropSetReturnsNonVoid,
        "Method transformed to a property 'set' returns non-void. The return value will be ignored in generated code.");
      }

    if (cArgs!=1)
      {
      LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_PropSetUnexpectedArgCount,
        "Unexpected number of arguments for method transformed to a property 'set' - cArgs: " << cArgs);
      }

    // :-)
    //
    // Pretend return type and attributes come from arg0 for the
    // rest of this method call:
    //
    if (cArgs!=0)
      {
      i = 0;
      retType = ft->GetArgument(0);
      }
    }

  // Do attributes indicate an array?
  //
  arraySize = GetMethodArgumentArraySize(c, m, ft, i);


  // Public, protected, private, internal, hmmmm...?
  //
  Emit(os, accessLevel.c_str());
  Emit(os, " ");


  if (m->GetStatic())
    {
    // C# static == C++ static:
    //
    if (StaticMethodRedefined(signature))
      {
      Emit(os, "new ");
      }

    Emit(os, "static ");
    }

  if (!asProperty)
    {
    if (m->GetVirtual())
      {
      // C# : "virtual" or "override" ?
      //
      if (VirtualMethodOverridden(signature))
        {
        Emit(os, "override ");
        }
      else
        {
        // If I am virtual, but my parent is not then:
        //
        if (OtherMethodRedefined(signature))
          {
          Emit(os, "new ");
          }
  
        Emit(os, "virtual ");
        }
      }
    else
      {
      if (OtherMethodRedefined(signature))
        {
        Emit(os, "new ");
        }
      }
    }

  // Return type. (Has special transformations for supporting collections
  // and enumerators. Method name + class supports target interface ==
  // special generated code...)
  //
  gxsys_stl::string mname = m->GetName();

  if ((mname == "GetEnumerator") &&
    this->HasTargetInterface("IEnumerable"))
    {
    Emit(os, "System.Collections.IEnumerator");
    }
  else if ((mname == "GetCurrent") &&
    this->HasTargetInterface("IEnumerator"))
    {
    Emit(os, "object");
    }
  else
    {
    Emit(os, GetCSharpTypeString(retType, true, arraySize != "").c_str());
    if (arraySize != "")
      {
      Emit(os, "[]");
      }
    }

  Emit(os, " ");

  // Use the wrapped method name or the property name:
  //
  s = GetWrappedMethodName(m);
  if (asProperty)
    {
    s = ExtractDerivedName(s.c_str(), m, this->GetSettings()->GetVerbose());
    }

  Emit(os, s.c_str());

  if (!asProperty)
    {
  // Open args:
  Emit(os, "(");

  // The C# args:
  for (i= 0; i<cArgs; ++i)
    {
    // If it's a CxxMain param pair, only declare the 2nd (string[] argv) part
    // of the pair... So, do nothing here and skip the 1st (int argc) part...
    //
    if (IsCxxMainStyleParamPair(ft, i))
      {
      }
    else
      {
      argType = ft->GetArgument(i);

      // Is arg an array?
      //
      arraySize = GetMethodArgumentArraySize(c, m, ft, i);

      // arg type:
      Emit(os, GetCSharpTypeString(argType, false, arraySize != "").c_str());

      // array notation:
      if (arraySize != "")
        {
        Emit(os, "[]");
        }

      // arg name:
      Emit(os, " ");
      Emit(os, GetArgName(ft, i));

      if (i<cArgs-1)
        {
        Emit(os, ", ");
        }
      }
    }

  // Close args:
  Emit(os, ")");
    }
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::EmitCSharpDllImportDeclaration(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, const char *f, bool emitExceptionParams)
{
  gxsys_stl::string arraySize;
  cable::FunctionType *ft = m->GetFunctionType();
  unsigned int cArgs = ft->GetNumberOfArguments();
  unsigned int cArgsEmitted = 0;
  unsigned int i = 0;
  cable::Type *argType = 0;
  cable::Type *retType = ft->GetReturns();

  Emit(os, "[DllImport(");
  Emit(os, dllname);
  Emit(os, ", EntryPoint = \"");
  Emit(os, f);
  Emit(os, "\")]");
  Emit(os, "\n");

  // Does method return an array?
  //
  arraySize = GetMethodArgumentArraySize(c, m, ft, RETURN_VALUE);

  EmitIndent(os);
  Emit(os, "internal static extern ");

  if (mname == "new")
    {
    Emit(os, "IntPtr ");
    Emit(os, f);
    Emit(os, "(ref uint mteStatus, ref uint mteIndex, ref uint rawRefCount");
    cArgsEmitted += 3;

    if (emitExceptionParams)
      {
      Emit(os, ", ref uint mteExceptionIndex, ref IntPtr clonedException");
      cArgsEmitted += 2;
      }

    Emit(os, ");");
    }
  else if (mname == "delete")
    {
    Emit(os, "void ");
    Emit(os, f);
    Emit(os, "(HandleRef pThis");
    cArgsEmitted += 1;

    if (emitExceptionParams)
      {
      Emit(os, ", ref uint mteExceptionIndex, ref IntPtr clonedException");
      cArgsEmitted += 2;
      }

    Emit(os, ");");
    }
  else
    {
    Emit(os, GetPInvokeTypeString(retType, true, arraySize != "", false).c_str());
    Emit(os, " ");
    Emit(os, f);
    Emit(os, "(");

    if (!m->GetStatic())
      {
      Emit(os, "HandleRef pThis");
      cArgsEmitted += 1;

      if (cArgs!=0)
        {
        Emit(os, ", ");
        }
      }

    for (i= 0; i<cArgs; ++i)
      {
      argType = ft->GetArgument(i);

      // Is arg an array?
      //
      arraySize = GetMethodArgumentArraySize(c, m, ft, i);

      // array MarshalAs directive:
      if (arraySize != "")
        {
        Emit(os, "[MarshalAs(UnmanagedType.LPArray, SizeConst = ");
        Emit(os, arraySize.c_str());
        Emit(os, ")] ");
        }

      // arg type:
      Emit(os, GetPInvokeTypeString(argType, false, arraySize != "", false).c_str());

      // array notation:
      if (arraySize != "")
        {
        Emit(os, "[]");
        }

      // arg name:
      Emit(os, " ");
      Emit(os, GetArgName(ft, i));

      cArgsEmitted += 1;

      if (i<cArgs-1)
        {
        Emit(os, ", ");
        }
      }

    // Add "generated args" to method signatures that return object pointers:
    //
    if (!HasMapToType(retType) && IsObjectPointer(retType))
      {
      if (cArgsEmitted)
        {
        Emit(os, ", ");
        }

      Emit(os, "ref uint mteStatus, ref uint mteIndex, ref uint rawRefCount");
      cArgsEmitted += 3;
      }

    // And to those that handle wrapped exceptions:
    //
    if (emitExceptionParams)
      {
      if (cArgsEmitted)
        {
        Emit(os, ", ");
        }

      Emit(os, "ref uint mteExceptionIndex, ref IntPtr clonedException");
      cArgsEmitted += 2;
      }

    // Close args:
    //
    Emit(os, ");");
    }
}


//----------------------------------------------------------------------------
gxsys_stl::string GetQualifiedEventName(const cable::Method *m)
{
  const cable::Class *c = cable::ClassType::SafeDownCast(
    cable::PointerType::SafeDownCast(
    m->GetFunctionType()->GetReturns()
    )->GetTarget())->GetClass();

  gxsys_stl::string eventName;

  if (c && c->GetContext())
  {
    eventName = c->GetContext()->GetName();
    eventName += GetWrappedClassName(c);
  }
  else
  {
    eventName = "ERROR_invalid_input_to_GetQualifiedEventName";
    LogError(me_InvalidArg, << eventName.c_str());
  }

  return eventName;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetEventName(const cable::Method *m)
{
  gxsys_stl::string eventName = GetWrappedClassName(
    cable::ClassType::SafeDownCast(cable::PointerType::SafeDownCast(
    m->GetFunctionType()->GetReturns()
    )->GetTarget())->GetClass()
    );

  return eventName;
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::EmitCSharpEvent(gxsys_ios::ostream& os, const char *, const cable::Class* c, const cable::Method* m, const gxsys_stl::string& eventName)
{
  // Caller provides eventName because caller is the one with the knowledge
  // about whether there are multiple events with the same unqualified name
  // due to classes with the same name existing in multiple namespaces.
  // So, if there are not, eventName will be from "GetEventName", but if
  // there are, eventName will be from "GetQualifiedEventName"...
  //
  gxsys_stl::string e(eventName);
  gxsys_stl::string eFull;
  gxsys_stl::string mname(m->GetName());

  // An "event method" is expected to return a pointer to the event class
  // that it is supposed to wrap... If that's not true, the code generated
  // by this following block is likely not to compile! So make sure it's
  // true in your sources to be wrapped.
  //
  cable::FunctionType *ft = m->GetFunctionType();
  cable::Type *retType = ft->GetReturns();

  if (retType->GetTypeId() != cable::Type::PointerTypeId)
    {
    LogFileLineErrorMsg(m->GetFile(), m->GetLine(), me_EventMethodIncorrectReturnType,
      "iwhEvent method '" << mname << "' does not return a pointer to an event object");
    return;
    }

  if (cable::PointerType::SafeDownCast(retType)->GetTarget()->GetTypeId() !=
    cable::Type::ClassTypeId)
    {
    LogFileLineErrorMsg(m->GetFile(), m->GetLine(), me_EventMethodIncorrectReturnType,
      "iwhEvent method '" << mname << "' does not return a pointer to an event object");
    return;
    }

  eFull = GetWrappedClassNameFullyQualified(cable::ClassType::SafeDownCast(
    cable::PointerType::SafeDownCast(retType)->GetTarget())->GetClass());

  // Assumptions:
  //
  //   (These could all be codified and made into non-assumptions, but for now
  //    they all *are* assumptions...)
  //
  // There's a type called "Handler" in the Event class (typedef to a function pointer in unmanaged C++)
  // The typedef signature is "void blah(object sender, object args)"
  // There's a corresponding EventArgs class named the same but with "EventArgs" at the end
  // There's an AddHandler method in the Event class
  // There's a RemoveHandler method in the Event class
  // The handler is id'able by a C# "uint"

  // Make this next section collapsible inside a "region":
  //
  EmitIndent(os);
  Emit(os, "#region ");
  Emit(os, e.c_str());
  Emit(os, " event implementation details\n");
	Emit(os, "\n");

  // A data member (*Impl) to hold the managed listeners:
  //
  EmitIndent(os);
	Emit(os, "private ");
  Emit(os, e.c_str());
  Emit(os, "EventHandler ");
  Emit(os, e.c_str());
  Emit(os, "Impl;\n");

  // A data member (*Instance) to cache the managed wrapper of the unmanaged
  // event object:
  //
  EmitIndent(os);
	Emit(os, "private ");
  Emit(os, eFull.c_str());
  Emit(os, " ");
  Emit(os, e.c_str());
  Emit(os, "Instance;\n");

  // A data member (*RelayHandler) and an id (*RelayHandlerId) for the handler
  // when we are actively connected...
  //
  EmitIndent(os);
	Emit(os, "private ");
  Emit(os, eFull.c_str());
  Emit(os, ".Handler ");
  Emit(os, e.c_str());
  Emit(os, "RelayHandler;\n");
  EmitIndent(os);
	Emit(os, "private uint ");
  Emit(os, e.c_str());
  Emit(os, "RelayHandlerId;\n");
  Emit(os, "\n");

  // A listener to receive events from the unmanaged code and relay them on to
  // managed listeners. Conforms to event's "unmanaged" delegate signature.
  // (The delegate named "Handler" in the event class itself.) One improvement
  // we could make here would be to generate RelayHandler's signature based
  // on the actual signature of "Handler."
  //
  EmitIndent(os);
	Emit(os, "private void ");
  Emit(os, e.c_str());
  Emit(os, "RelayHandlerMethod(IntPtr rawSender, IntPtr rawArgs)\n");
  EmitIndent(os);
  Emit(os, "{\n");
  EmitIndent(os, 2);
  Emit(os, "if (this.");
  Emit(os, e.c_str());
  Emit(os, "Impl != null)\n");
  EmitIndent(os, 2);
  Emit(os, "{\n");
  EmitIndent(os, 3);
  Emit(os, eFull.c_str());
  Emit(os, "EventArgs rv = null;\n");
  EmitIndent(os, 3);
  Emit(os, "if (IntPtr.Zero != rawArgs)\n");
  EmitIndent(os, 3);
  Emit(os, "{\n");




  const cable::Class* cRetType = cable::ClassType::SafeDownCast(
    cable::PointerType::SafeDownCast(retType)->GetTarget()
    )->GetClass();

  gxsys_stl::string registerMethod = this->GetSettings()->GetRegisterMethod(cRetType);
  gxsys_stl::string unRegisterMethod = this->GetSettings()->GetUnRegisterMethod(cRetType);




  // Technique #1 : using "new" (and ignoring potential issues with
  // the mummy Runtime table...
  //
  EmitIndent(os, 4);
  Emit(os, "rv = new ");
  Emit(os, eFull.c_str());
  Emit(os, "EventArgs(rawArgs, ");

  if (!registerMethod.empty())
    {
    Emit(os, "true");
    }
  else
    {
    Emit(os, "false");
    }

  Emit(os, ", false);\n");

  if (!registerMethod.empty())
    {
    EmitIndent(os, 4);
    Emit(os, "rv.");
    Emit(os, registerMethod.c_str());
    Emit(os, ";\n");
    }




  // Technique #2 : using "CreateWrappedObject" (and forcing a call
  // back across the boundary to "Register" the EventArgs if the wrapper
  // was just created...)
  //
  // This imposes an additional assumption/constraint on Event and EventArgs
  // classes. They must share Register/UnRegister methods for this to work
  // properly...
  //
  // Either that, or we need to get a "cable::Class*" to the EventArgs class
  // somehow (which we may or may not be able to do depending on how the
  // gccxml step and the input header files were crafted...)
  //
  //unsigned int indent = 3;
  //EmitIndent(os, indent+1);
  //Emit(os, "bool mteCreated;\n");
  //EmitIndent(os, indent+1);
  //Emit(os, "rv = (");
  //Emit(os, eFull.c_str());
  //Emit(os, "EventArgs)\n");
  //EmitIndent(os, indent+2);
  //Emit(os, "Kitware.mummy.Runtime.Methods.CreateWrappedObject(\n");
  //EmitIndent(os, indent+3);
  //Emit(os, "\"class ");
  //Emit(os, eFull.c_str());
  //Emit(os, "EventArgs\", rawArgs, ");

  //if (!unRegisterMethod.empty())
  //  {
  //  Emit(os, "true");
  //  }
  //else
  //  {
  //  Emit(os, "false");
  //  }

  //Emit(os, ", out mteCreated");
  //Emit(os, ");\n");

  //if (!registerMethod.empty())
  //  {
  //  //if (this->MethodReturnValueIsCounted(c, m))
  //  //  {
  //  //  EmitIndent(os, indent+1);
  //  //  Emit(os, "Returned object is counted already, (factory method or iwhCounted hint),\n");
  //  //  EmitIndent(os, indent+1);
  //  //  Emit(os, "// no 'rv.");
  //  //  Emit(os, registerMethod.c_str());
  //  //  Emit(os, "' call is necessary...\n");
  //  //  }
  //  //else
  //  //  {
  //    EmitIndent(os, indent+1);
  //    Emit(os, "if (mteCreated)\n");
  //    EmitIndent(os, indent+1);
  //    Emit(os, "{\n");
  //    EmitIndent(os, indent+2);
  //    Emit(os, "rv.");
  //    Emit(os, registerMethod.c_str());
  //    Emit(os, ";\n");
  //    EmitIndent(os, indent+1);
  //    Emit(os, "}\n");
  //  //  }
  //  }




  EmitIndent(os, 3);
  Emit(os, "}\n");
  EmitIndent(os, 3);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "Impl(this, rv);\n");
  EmitIndent(os, 2);
  Emit(os, "}\n");
  EmitIndent(os);
	Emit(os, "}\n");
	Emit(os, "\n");

  // Methods to add and remove the RelayHandler:
  //
  EmitIndent(os);
	Emit(os, "private void Add");
  Emit(os, e.c_str());
  Emit(os, "RelayHandler()\n");
  EmitIndent(os);
  Emit(os, "{\n");
  EmitIndent(os, 2);
  Emit(os, "if (0 == this.");
  Emit(os, e.c_str());
  Emit(os, "RelayHandlerId)\n");
  EmitIndent(os, 2);
  Emit(os, "{\n");

  EmitIndent(os, 3);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "RelayHandler =\n");
  EmitIndent(os, 4);
  Emit(os, "new ");
  Emit(os, eFull.c_str());
  Emit(os, ".Handler(\n");
  EmitIndent(os, 5);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "RelayHandlerMethod);\n");
	Emit(os, "\n");

  EmitIndent(os, 3);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "Instance =\n");
  EmitIndent(os, 4);
  Emit(os, "this.");
  Emit(os, mname.c_str());
  Emit(os, "();\n");
	Emit(os, "\n");

  EmitIndent(os, 3);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "RelayHandlerId =\n");
  EmitIndent(os, 4);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "Instance.AddHandler(\n");
  EmitIndent(os, 5);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "RelayHandler);\n");

  EmitIndent(os, 2);
  Emit(os, "}\n");
  EmitIndent(os);
  Emit(os, "}\n");
	Emit(os, "\n");

  EmitIndent(os);
	Emit(os, "private void Remove");
  Emit(os, e.c_str());
  Emit(os, "RelayHandler()\n");
  EmitIndent(os);
  Emit(os, "{\n");
  EmitIndent(os, 2);
  Emit(os, "if (0 != this.");
  Emit(os, e.c_str());
  Emit(os, "RelayHandlerId)\n");
  EmitIndent(os, 2);
  Emit(os, "{\n");

  EmitIndent(os, 3);
  Emit(os, "if (System.IntPtr.Zero != this.");
  Emit(os, e.c_str());
  Emit(os, "Instance.GetCppThis().Handle)\n");
  EmitIndent(os, 3);
  Emit(os, "{\n");
  EmitIndent(os, 4);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "Instance.RemoveHandler(\n");
  EmitIndent(os, 5);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "RelayHandlerId);\n");
  EmitIndent(os, 3);
  Emit(os, "}\n");
	Emit(os, "\n");

  EmitIndent(os, 3);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "Instance = null;\n");

  EmitIndent(os, 3);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "RelayHandler = null;\n");

  EmitIndent(os, 3);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "RelayHandlerId = 0;\n");

  EmitIndent(os, 2);
  Emit(os, "}\n");
  EmitIndent(os);
  Emit(os, "}\n");
	Emit(os, "\n");

  // End the private details "region":
  //
  EmitIndent(os);
  Emit(os, "#endregion\n");
	Emit(os, "\n");
	Emit(os, "\n");

  // A delegate definition that managed C# handlers must conform to in order
  // to listen to this event:
  //
  EmitIndent(os);
  Emit(os, "/// <summary>\n");
  EmitIndent(os);
  Emit(os, "/// Delegate signature for the ");
  Emit(os, e.c_str());
  Emit(os, " event.\n");
  EmitIndent(os);
  Emit(os, "/// </summary>\n");
  EmitIndent(os);
  Emit(os, "public delegate void ");
  Emit(os, e.c_str());
  Emit(os, "EventHandler(");
  Emit(os, c->GetName());
  Emit(os, " sender, ");
  Emit(os, eFull.c_str());
  Emit(os, "EventArgs args);\n");
  Emit(os, "\n");
  Emit(os, "\n");

  // Documentation:
  gxsys_stl::vector<gxsys_stl::string> docblock;
  this->GetHeaderFileReader(c)->GetCommentBlockBefore(m->GetLine(), docblock, this->ClassLineNumber);
  EmitDocumentationBlock(os, docblock, 1);

  // The actual event with add and remove handlers.
  //
  EmitIndent(os);
	Emit(os, "public event ");
  Emit(os, e.c_str());
  Emit(os, "EventHandler ");
  Emit(os, e.c_str());
  Emit(os, "\n");

  EmitIndent(os);
	Emit(os, "{\n");

  EmitIndent(os, 2);
  Emit(os, "add\n");
  EmitIndent(os, 2);
	Emit(os, "{\n");
  EmitIndent(os, 3);
  Emit(os, "if (this.");
  Emit(os, e.c_str());
  Emit(os, "Impl == null)\n");
  EmitIndent(os, 3);
  Emit(os, "{\n");
  EmitIndent(os, 4);
	Emit(os, "Add");
  Emit(os, e.c_str());
  Emit(os, "RelayHandler();\n");
  EmitIndent(os, 3);
  Emit(os, "}\n");
	Emit(os, "\n");
  EmitIndent(os, 3);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "Impl += value;\n");
  EmitIndent(os, 2);
	Emit(os, "}\n");
	Emit(os, "\n");

  EmitIndent(os, 2);
	Emit(os, "remove\n");
  EmitIndent(os, 2);
	Emit(os, "{\n");
  EmitIndent(os, 3);
  Emit(os, "this.");
  Emit(os, e.c_str());
  Emit(os, "Impl -= value;\n");
	Emit(os, "\n");
  EmitIndent(os, 3);
  Emit(os, "if (this.");
  Emit(os, e.c_str());
  Emit(os, "Impl == null)\n");
  EmitIndent(os, 3);
  Emit(os, "{\n");
  EmitIndent(os, 4);
	Emit(os, "Remove");
  Emit(os, e.c_str());
  Emit(os, "RelayHandler();\n");
  EmitIndent(os, 3);
  Emit(os, "}\n");
  EmitIndent(os, 2);
  Emit(os, "}\n");

  EmitIndent(os);
	Emit(os, "}\n");
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::EmitCSharpProperty(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *propGetMethod, const cable::Method *propSetMethod, bool emitExceptionParams)
{
  gxsys_stl::string fGet;
  gxsys_stl::string fSet;
  unsigned int indent = 3;
  const cable::Method *declMethod = propGetMethod;

  // Declare the DllImport functions.
  //
  if (propGetMethod)
    {
    fGet = GetExportLayerFunctionName(c, propGetMethod, propGetMethod->GetName());
    EmitIndent(os);
    EmitCSharpDllImportDeclaration(os, dllname, c, propGetMethod, propGetMethod->GetName(), fGet.c_str(), emitExceptionParams);
    Emit(os, "\n");
    Emit(os, "\n");
    }

  if (propSetMethod)
    {
    fSet = GetExportLayerFunctionName(c, propSetMethod, propSetMethod->GetName());
    EmitIndent(os);
    EmitCSharpDllImportDeclaration(os, dllname, c, propSetMethod, propSetMethod->GetName(), fSet.c_str(), emitExceptionParams);
    Emit(os, "\n");
    Emit(os, "\n");

    if (!declMethod)
      {
      declMethod = propSetMethod;
      }
    }

  if (!declMethod)
    {
    // What? no propGetMethod or propSetMethod? Caller has made a mistake somewhere...
    //
    LogError(me_InternalError, << "No declMethod in MummyCsharpGenerator::EmitCSharpProperty.");
    return;
    }

  // Documentation:
  gxsys_stl::vector<gxsys_stl::string> docblock;
  this->GetHeaderFileReader(c)->GetCommentBlockBefore(declMethod->GetLine(), docblock, this->ClassLineNumber);
  EmitDocumentationBlock(os, docblock, 1);

  // Declaration:
  EmitIndent(os);
  EmitCSharpMethodDeclaration(os, c, declMethod, true, declMethod == propSetMethod, "public");
  Emit(os, "\n");

  // Open body:
  EmitIndent(os);
  Emit(os, "{");
  Emit(os, "\n");

  // The "get" body:
  if (propGetMethod)
    {
    EmitIndent(os, 2);
    Emit(os, "get\n");
    EmitIndent(os, 2);
    Emit(os, "{\n");
    EmitIndent(os, 3);
    Emit(os, "// iwhPropGet\n");
    EmitCSharpMethodBody(os, 3, c, propGetMethod, fGet, 0, emitExceptionParams);
    indent = 3;
    EmitIndent(os, 2);
    Emit(os, "}\n");
    }

  // The "set" body:
  if (propSetMethod)
    {
    if (propGetMethod)
      {
      Emit(os, "\n");
      }

    EmitIndent(os, 2);
    Emit(os, "set\n");
    EmitIndent(os, 2);
    Emit(os, "{\n");
    EmitIndent(os, 3);
    Emit(os, "// iwhPropSet\n");
    EmitCSharpMethodBody(os, 3, c, propSetMethod, fSet, "value", emitExceptionParams);
    indent = 3;
    EmitIndent(os, 2);
    Emit(os, "}\n");
    }

  // Close body:
  EmitIndent(os);
  Emit(os, "}");
  Emit(os, "\n");
}


//----------------------------------------------------------------------------
void EmitThrowClonedException(gxsys_ios::ostream &os, unsigned int indent)
{
  EmitIndent(os, indent);
  Emit(os, "if (IntPtr.Zero != clonedException)\n");
  EmitIndent(os, indent);
  Emit(os, "{\n");

  EmitIndent(os, indent+1);
  Emit(os, "bool mteCreatedException;\n");
  EmitIndent(os, indent+1);
  Emit(os, "System.Exception wrappedException = (System.Exception)\n");
  EmitIndent(os, indent+2);
  Emit(os, "Kitware.mummy.Runtime.Methods.CreateWrappedObject(\n");
  EmitIndent(os, indent+3);
  Emit(os, "0, mteExceptionIndex, 0, clonedException, true, out mteCreatedException);\n");
  EmitIndent(os, indent+1);
  Emit(os, "throw wrappedException;\n");

  //EmitIndent(os, indent+1);
  //Emit(os, "System.Exception exc = new System.Exception(\"received clonedException from unmanaged layer...\");\n");
  //EmitIndent(os, indent+1);
  //Emit(os, "throw exc;\n");

  EmitIndent(os, indent);
  Emit(os, "}\n");
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::EmitCSharpMethodBody(gxsys_ios::ostream &os, unsigned int indent, const cable::Class *c, const cable::Method *m, gxsys_stl::string& f, const char *impliedArg0, bool emitExceptionParams)
{
  // need qualified name?
  gxsys_stl::string cname(c->GetName());

  gxsys_stl::string retArraySize;
  gxsys_stl::string argArraySize;
  cable::FunctionType *ft = m->GetFunctionType();
  unsigned int cArgs = ft->GetNumberOfArguments();
  unsigned int cArgsEmitted = 0;
  unsigned int i = 0;
  cable::Type *argType = 0;
  cable::Type *retType = ft->GetReturns();
  bool voidReturn = false;
  gxsys_stl::string rvType;
  gxsys_stl::string rvpType;
  bool argIsRef = false;
  gxsys_stl::string argTypeString;
  gxsys_stl::string emittedArg;
  bool callGetCppThis = false;

  // Does method return anything?
  //
  if (IsVoid(retType))
    {
    voidReturn = true;
    }

  // Does method return an array?
  //
  retArraySize = GetMethodArgumentArraySize(c, m, ft, RETURN_VALUE);

  rvpType = GetPInvokeTypeString(retType, true, retArraySize != "", false);
  rvType = GetCSharpTypeString(retType, true, retArraySize != "");


  // Body:
  //


  // Set up to handle wrapped exceptions if this class may throw them:
  //
  if (emitExceptionParams)
    {
    EmitIndent(os, indent);
    Emit(os, "uint mteExceptionIndex = 0;\n");
    EmitIndent(os, indent);
    Emit(os, "IntPtr clonedException = IntPtr.Zero;\n");
    Emit(os, "\n");
    }

  // Delegate the call through the PInvoke/DllImport layer:
  //
  EmitIndent(os, indent);

  if (!voidReturn)
    {
    if (retArraySize != "")
      {
      Emit(os, rvpType.c_str());
      Emit(os, " rvp = "); // rvp == return value pointer
      }
    else if (IsObjectPointer(retType))
      {
      Emit(os, rvType.c_str());
      Emit(os, " rv = null;\n");

      Emit(os, "\n");

      EmitIndent(os, indent);
      Emit(os, "uint mteStatus = 0;\n");
      EmitIndent(os, indent);
      Emit(os, "uint mteIndex = UInt32.MaxValue;\n");
      EmitIndent(os, indent);
      Emit(os, "uint rawRefCount = 0;\n");

      EmitIndent(os, indent);
      Emit(os, rvpType.c_str());
      Emit(os, " rvp = "); // rvp == return value pointer
      }
    else
      {
      Emit(os, rvType.c_str());
      Emit(os, " rv = "); // rv == return value
      }
    }

  // Open any special marshalling blocks required:
  //
  if (IsCharPointer(retType))
    {
    Emit(os, "Marshal.PtrToStringAnsi(");
    }

  // Call the DllImport function:
  //
  Emit(os, f.c_str());
  Emit(os, "(");

  // Arguments, 'this' first, then loop over C++ method formal params:
  //
  if (!m->GetStatic())
    {
    Emit(os, "this.GetCppThis()");
    cArgsEmitted += 1;

    if (cArgs!=0)
      {
      Emit(os, ", ");
      }
    }

  for (i= 0; i<cArgs; ++i)
    {
    // If it's a CxxMain param pair, use the length of the 2nd (string[] argv)
    // part as the value of the 1st (int argc) part...
    //
    if (IsCxxMainStyleParamPair(ft, i))
      {
      Emit(os, GetArgName(ft, i+1));
      Emit(os, ".Length");
      }
    else
      {
      argType = ft->GetArgument(i);

      // Is arg an array?
      //
      argArraySize = GetMethodArgumentArraySize(c, m, ft, i);

      argIsRef = false;
      if (argArraySize == "" && GetIsRefArg(argType))
        {
        argIsRef = true;
        Emit(os, "ref ");
        }

      argTypeString = GetCSharpTypeString(argType, true, argArraySize != "");
      if (!argIsRef && (argTypeString == "bool"))
        {
        Emit(os, "(byte)(");
        }

      if (impliedArg0 && 0==i)
        {
        emittedArg = impliedArg0;
        }
      else
        {
        emittedArg = GetArgName(ft, i);
        }

      Emit(os, emittedArg.c_str());

      if (argIsRef)
        {
        LogInfo(mi_InfoRefArgEncountered,
          << "reference arg: "
          << cname << "." << m->GetName()
          << " " << argType->GetCxxType().GetName() << " " << emittedArg
          << " (arg " << i << ")"
          );
        }

      callGetCppThis = false;

      if (IsObjectPointer(argType))
        {
        const cable::Class* argClass = cable::ClassType::SafeDownCast(
          cable::PointerType::SafeDownCast(argType)->GetTarget())->GetClass();
        if (!IsUtilityClass(argClass))
          {
          callGetCppThis = true;
          }
        }
      else if (IsObjectPointerReference(argType))
        {
        const cable::PointerType* ptrClass = cable::PointerType::SafeDownCast(
          cable::ReferenceType::SafeDownCast(argType)->GetTarget());
        const cable::Class* argClass = cable::ClassType::SafeDownCast(
          ptrClass->GetTarget())->GetClass();
        if (!IsUtilityClass(argClass))
          {
          callGetCppThis = true;
          }
        }

      if (callGetCppThis)
        {
        Emit(os, " == null ? new HandleRef() : ");
        Emit(os, emittedArg.c_str());
        Emit(os, ".GetCppThis()");
        }

      if (!argIsRef && (argTypeString == "bool"))
        {
        Emit(os, " ? 1 : 0)");
        }
      }

    cArgsEmitted += 1;

    if (i<cArgs-1)
      {
      Emit(os, ", ");
      }
    }

  if (IsObjectPointer(retType))
    {
    if (cArgsEmitted)
      {
      Emit(os, ", ");
      }

    Emit(os, "ref mteStatus, ref mteIndex, ref rawRefCount");
    cArgsEmitted += 3;
    }

  if (emitExceptionParams)
    {
    if (cArgsEmitted)
      {
      Emit(os, ", ");
      }

    Emit(os, "ref mteExceptionIndex, ref clonedException");
    cArgsEmitted += 2;
    }

  Emit(os, ")");

  // Close special marshalling for object casting or char * to string mapping
  // or handle bool/byte specially for PInvoke:
  //
  if (IsObjectPointer(retType))
    {
    const cable::Class* cRetType = cable::ClassType::SafeDownCast(
      cable::PointerType::SafeDownCast(retType)->GetTarget()
      )->GetClass();

    gxsys_stl::string registerMethod = this->GetSettings()->GetRegisterMethod(cRetType);
    gxsys_stl::string unRegisterMethod = this->GetSettings()->GetUnRegisterMethod(cRetType);

    Emit(os, ";\n");

    EmitIndent(os, indent);
    Emit(os, "if (IntPtr.Zero != rvp)\n");
    EmitIndent(os, indent);
    Emit(os, "{\n");

    EmitIndent(os, indent+1);
    Emit(os, "bool mteCreated;\n");
    EmitIndent(os, indent+1);
    Emit(os, "rv = (");
    Emit(os, rvType.c_str());
    Emit(os, ")\n");
    EmitIndent(os, indent+2);
    Emit(os, "Kitware.mummy.Runtime.Methods.CreateWrappedObject(\n");
    EmitIndent(os, indent+3);
    Emit(os, "mteStatus, mteIndex, rawRefCount, rvp, ");

    if (!unRegisterMethod.empty())
      {
      Emit(os, "true");
      }
    else
      {
      Emit(os, "false");
      }

    Emit(os, ", out mteCreated");
    Emit(os, ");\n");

    if (!registerMethod.empty())
      {
      if (this->MethodReturnValueIsCounted(c, m))
        {
        EmitIndent(os, indent+1);
        Emit(os, "// Returned object is counted already, (factory method or iwhCounted hint),\n");
        EmitIndent(os, indent+1);
        Emit(os, "// no 'rv.");
        Emit(os, registerMethod.c_str());
        Emit(os, "' call is necessary...\n");
        }
      else
        {
        EmitIndent(os, indent+1);
        Emit(os, "if (mteCreated)\n");
        EmitIndent(os, indent+1);
        Emit(os, "{\n");
        EmitIndent(os, indent+2);
        Emit(os, "rv.");
        Emit(os, registerMethod.c_str());
        Emit(os, ";\n");
        EmitIndent(os, indent+1);
        Emit(os, "}\n");
        }
      }

    EmitIndent(os, indent);
    Emit(os, "}\n");

    Emit(os, "\n");
    }
  else if (IsCharPointer(retType))
    {
    Emit(os, ");\n");
    }
  else if (rvType == "bool")
    {
    Emit(os, " == 0 ? false : true;\n");
    }
  else
    {
    Emit(os, ";\n");
    }


  if (emitExceptionParams)
    {
    Emit(os, "\n");
    EmitThrowClonedException(os, indent);
    Emit(os, "\n");
    }


  // Specially marshal array return values:
  if (retArraySize != "")
    {
    EmitIndent(os, indent);
    Emit(os, rvType.c_str());
    Emit(os, "[] rv = null;");
    Emit(os, "\n");

    EmitIndent(os, indent);
    Emit(os, "if (IntPtr.Zero != rvp)");
    Emit(os, "\n");
    EmitIndent(os, indent);
    Emit(os, "{");
    Emit(os, "\n");
      EmitIndent(os, indent+1);
      Emit(os, "rv = new ");
      Emit(os, rvType.c_str());
      Emit(os, "[");
      Emit(os, retArraySize.c_str());
      Emit(os, "];");
      Emit(os, "\n");

      // Special case "uint" since Marshal.Copy does not have
      // a "uint" overload...
      //
      if (rvType == "uint")
        {
        EmitIndent(os, indent+1);
        Emit(os, "int[] rv2 = new int[");
        Emit(os, retArraySize.c_str());
        Emit(os, "];");
        Emit(os, "\n");

        EmitIndent(os, indent+1);
        Emit(os, "Marshal.Copy(rvp, rv2, 0, rv.Length);");
        Emit(os, "\n");

        EmitIndent(os, indent+1);
        Emit(os, "for (int rv2i = 0; rv2i < ");
        Emit(os, retArraySize.c_str());
        Emit(os, "; ++rv2i)");
        Emit(os, "\n");

        EmitIndent(os, indent+1);
        Emit(os, "{");
        Emit(os, "\n");

        EmitIndent(os, indent+2);
        Emit(os, "rv[rv2i] = (uint)rv2[rv2i];");
        Emit(os, "\n");

        EmitIndent(os, indent+1);
        Emit(os, "}");
        Emit(os, "\n");
        }
      else
        {
        EmitIndent(os, indent+1);
        Emit(os, "Marshal.Copy(rvp, rv, 0, rv.Length);");
        Emit(os, "\n");
        }

    EmitIndent(os, indent);
    Emit(os, "}");
    Emit(os, "\n");
    }

  // Return statement:
  if (!voidReturn)
    {
    EmitIndent(os, indent);
    Emit(os, "return rv;");
    Emit(os, "\n");
    }
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::EmitCSharpMethod(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, const gxsys_stl::string& accessLevel, bool emitExceptionParams)
{
  gxsys_stl::string f(GetExportLayerFunctionName(c, m, mname));

  // First declare the DllImport function. This gets called within the method body.
  //
  EmitIndent(os);
  EmitCSharpDllImportDeclaration(os, dllname, c, m, mname, f.c_str(), emitExceptionParams);
  Emit(os, "\n");
  Emit(os, "\n");

  // Documentation:
  gxsys_stl::vector<gxsys_stl::string> docblock;
  this->GetHeaderFileReader(c)->GetCommentBlockBefore(m->GetLine(), docblock, this->ClassLineNumber);
  EmitDocumentationBlock(os, docblock, 1);

  // Declaration:
  EmitIndent(os);
  EmitCSharpMethodDeclaration(os, c, m, false, false, accessLevel);
  Emit(os, "\n");

  // Open body:
  EmitIndent(os);
  Emit(os, "{");
  Emit(os, "\n");

  // Body:
  EmitCSharpMethodBody(os, 2, c, m, f, 0, emitExceptionParams);

  // Close body:
  EmitIndent(os);
  Emit(os, "}");
  Emit(os, "\n");
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::EmitCSharpEnums(gxsys_ios::ostream &os, const cable::Class *c)
{
  gxsys_stl::vector<gxsys_stl::string> docblock;

  for (cable::Context::Iterator it = c->Begin(); it != c->End(); ++it)
    {
    cable::Enumeration *e = cable::Enumeration::SafeDownCast(*it);

    if (e && (cable::Context::Public == it.GetAccess()))
      {
      gxsys_stl::string ename(GetWrappedEnumName(e));

      LogVerboseInfo(<< "public enum - wrapped name: " << ename);

      Emit(os, "\n");
      Emit(os, "\n");

      docblock.clear();
      this->GetHeaderFileReader(c)->GetCommentBlockBefore(e->GetLine(), docblock, this->ClassLineNumber);
      EmitDocumentationBlock(os, docblock, 1);

      EmitIndent(os);
      Emit(os, "public ");
      if (WrappedEnumExists(ename))
        {
        Emit(os, "new ");
        }
      Emit(os, "enum ");
      Emit(os, ename.c_str());
      Emit(os, "\n");

      EmitIndent(os);
      Emit(os, "{\n");

      for (cable::Enumeration::Iterator eit = e->Begin(); eit != e->End(); ++eit)
        {
        EmitIndent(os, 2);
        Emit(os, "/// <summary>enum member</summary>\n");

        EmitIndent(os, 2);
        Emit(os, *eit);
        Emit(os, " = ");
        EmitInt(os, eit.GetValue());
        Emit(os, ",");
        Emit(os, "\n");

        Emit(os, "\n");
        }

      EmitIndent(os);
      Emit(os, "}\n");
      }
    }
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::EmitCSharpConstructor(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, bool emitExceptionParams)
{
  // need qualified name?
  gxsys_stl::string cname(c->GetName());
  gxsys_stl::string f;

  if (this->GetSettings()->GetUseShadow(c))
    {
    // Special case shadow class factory method since m might be NULL
    // (like it *is* with vtkCommand...)
    //
    f = cname + "Shadow_CreateShadow";

    EmitIndent(os);
    Emit(os, "[DllImport(");
    Emit(os, dllname);
    Emit(os, ", EntryPoint = \"");
    Emit(os, f.c_str());
    Emit(os, "\")]");
    Emit(os, "\n");

    EmitIndent(os);
    Emit(os, "static extern IntPtr ");
    Emit(os, f.c_str());
    Emit(os, "(IntPtr primary);\n");

    Emit(os, "\n");

    // Documentation:
    gxsys_stl::vector<gxsys_stl::string> docblock;
    this->GetHeaderFileReader(c)->GetCommentBlockBefore(
      (m ? m->GetLine() : c->GetLine()), docblock, this->ClassLineNumber);
    EmitDocumentationBlock(os, docblock, 1);

    EmitIndent(os);
    Emit(os, "public ");
    Emit(os, cname.c_str());
    Emit(os, "() : this(IntPtr.Zero, false, false)\n");

    EmitIndent(os);
    Emit(os, "{\n");

    EmitIndent(os, 2);
    Emit(os, "IntPtr primary = Marshal.GetIDispatchForObject(this);\n");

    EmitIndent(os, 2);
    Emit(os, "this.SetCppThis(");
    Emit(os, f.c_str());
    Emit(os, "(primary), true, false);\n");

    EmitIndent(os, 2);
    Emit(os, "Marshal.Release(primary);\n");

    EmitIndent(os);
    Emit(os, "}\n");
    }
  else
    {
    f = GetExportLayerFunctionName(c, m, mname);

    // Explanation of the "emitDefaultFactoryMethod" MummySettings.xml attribute.
    // ==========================================================================
    //
    // Either: emit the factory method itself, including the DllImport declaration,
    // and then emit the default C# constructor, too, which gives two ways to get an
    // instance of an object of class c...
    //    (emitDefaultFactoryMethod="true" in MummySettings.xml)
    //
    // Or: emit just the DllImport declaration and the default C# constructor and
    // *skip* emitting the factory method itself, which only gives one way to get an
    // instance of class c.
    //    (emitDefaultFactoryMethod="false" in or absent from MummySettings.xml)
    //
    // We code for both ways because we have some clients who want abstract C# classes
    // to be really abstract and *uncreatable* (even via a separate factory method) at
    // this class level. Only concrete subclasses can even be instantiated.
    //
    // On the other hand, we also have clients (think VTK and its ubiquitous New method)
    // that want the factory method behavior specifically so that abstract class instances
    // *can* be created, even though behind the scenes a concrete subclass of the factory
    // method's choice is the thing actually being instantiated.
    //
    if (this->GetSettings()->GetEmitDefaultFactoryMethod(c))
      {
      EmitCSharpMethod(os, dllname, c, m, mname, "public", emitExceptionParams);
      }
    else
      {
      EmitIndent(os);
      EmitCSharpDllImportDeclaration(os, dllname, c, m, mname, f.c_str(), emitExceptionParams);
      }

    Emit(os, "\n");
    Emit(os, "\n");

    // Documentation:
    gxsys_stl::vector<gxsys_stl::string> docblock;
    this->GetHeaderFileReader(c)->GetCommentBlockBefore(m->GetLine(), docblock, this->ClassLineNumber);
    EmitDocumentationBlock(os, docblock, 1);

    EmitIndent(os);
    Emit(os, "public ");
    Emit(os, cname.c_str());
    Emit(os, "()\n");
    EmitIndent(os, 2);
    Emit(os, ": base(IntPtr.Zero, false, false)\n");

    EmitIndent(os);
    Emit(os, "{\n");

    EmitIndent(os, 2);
    Emit(os, "// mummy generated default C# constructor\n");
    EmitIndent(os, 2);
    Emit(os, "uint mteStatus = 0;\n");
    EmitIndent(os, 2);
    Emit(os, "uint mteIndex = UInt32.MaxValue;\n");
    EmitIndent(os, 2);
    Emit(os, "uint rawRefCount = 0;\n");
    Emit(os, "\n");

    if (emitExceptionParams)
      {
      EmitIndent(os, 2);
      Emit(os, "uint mteExceptionIndex = UInt32.MaxValue;\n");
      EmitIndent(os, 2);
      Emit(os, "IntPtr clonedException = IntPtr.Zero;\n");
      Emit(os, "\n");
      }

    EmitIndent(os, 2);
    Emit(os, "IntPtr rawCppThis = ");
    Emit(os, f.c_str());
    Emit(os, "(ref mteStatus, ref mteIndex, ref rawRefCount");

    if (emitExceptionParams)
      {
      Emit(os, ", ref mteExceptionIndex, ref clonedException");
      }

    Emit(os, ");\n");

    if (emitExceptionParams)
      {
      Emit(os, "\n");
      EmitThrowClonedException(os, 2);
      Emit(os, "\n");
      }

    EmitIndent(os, 2);
    Emit(os, "this.SetCppThis(rawCppThis, true, (0==mteStatus || rawRefCount<2 ? false : true));\n");

    EmitIndent(os);
    Emit(os, "}\n");
    }
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::EmitCSharpRegister(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, bool emitExceptionParams)
{
  EmitCSharpMethod(os, dllname, c, m, mname, "public", emitExceptionParams);
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::EmitCSharpDispose(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, const unsigned int eventCount, bool emitExceptionParams)
{
  cable::FunctionType *ft = 0;
  cable::Type *argType = 0;
  unsigned int cArgs = 0;
  unsigned int i = 0;
  gxsys_stl::string f;

  if (m)
    {
    ft = m->GetFunctionType();
    cArgs = ft->GetNumberOfArguments();

    // need qualified name?
    f = GetExportLayerFunctionName(c, m, mname);

    EmitIndent(os);
    EmitCSharpDllImportDeclaration(os, dllname, c, m, mname, f.c_str(), emitExceptionParams);
    Emit(os, "\n");
    Emit(os, "\n");

    // Documentation:
    gxsys_stl::vector<gxsys_stl::string> docblock;
    this->GetHeaderFileReader(c)->GetCommentBlockBefore(m->GetLine(), docblock, this->ClassLineNumber);
    EmitDocumentationBlock(os, docblock, 1);
    }
  else
    {
    EmitIndent(os);
    Emit(os, "/// <summary>\n");
    EmitIndent(os);
    Emit(os, "/// Automatically generated protected Dispose method - called from\n");
    EmitIndent(os);
    Emit(os, "/// public Dispose or the C# destructor. DO NOT call directly.\n");
    EmitIndent(os);
    Emit(os, "/// </summary>\n");
    }


  EmitIndent(os);
  Emit(os, "protected override void Dispose(bool disposing)\n");
  EmitIndent(os);
  Emit(os, "{\n");


  // If we are going to emit any code other than "base.Dispose(disposing);"
  // then make sure it is wrapped in a try/finally block so that we still call
  // base.Dispose if one of these other bits of code we're calling throws
  // an exception...
  //
  bool openedTryFinally = false;
  unsigned int indent = 2;
  if ((0 != eventCount) || m)
    {
    openedTryFinally = true;
    indent = 3;

    EmitIndent(os, indent-1);
    Emit(os, "try\n");
    EmitIndent(os, indent-1);
    Emit(os, "{\n");
    }


  if (0 != eventCount)
    {
    EmitIndent(os, indent);
    Emit(os, "this.RemoveAllRelayHandlers();\n");
    }

  if (m)
    {
    EmitIndent(os, indent);
    Emit(os, "if (this.GetCallDisposalMethod())\n");
    EmitIndent(os, indent);
    Emit(os, "{\n");

    if (emitExceptionParams)
      {
      EmitIndent(os, indent+1);
      Emit(os, "uint mteExceptionIndex = 0;\n");
      EmitIndent(os, indent+1);
      Emit(os, "IntPtr clonedException = IntPtr.Zero;\n");
      Emit(os, "\n");
      }

    EmitIndent(os, indent+1);
    Emit(os, f.c_str());
    Emit(os, "(this.GetCppThis()");

    for (i = 0; i<cArgs; ++i)
      {
      argType = ft->GetArgument(i);

      if (IsObjectPointer(argType))
        {
        Emit(os, ", new HandleRef()");
        }
      else if (IsVoidPointer(argType))
        {
        Emit(os, ", System.IntPtr.Zero");
        }
      else
        {
        Emit(os, ", 0");
        }
      }

    if (emitExceptionParams)
      {
      Emit(os, ", ref mteExceptionIndex, ref clonedException");
      }

    Emit(os, ");\n");
    EmitIndent(os, indent+1);
    Emit(os, "this.ClearCppThis();\n");

    if (emitExceptionParams)
      {
      Emit(os, "\n");
      EmitThrowClonedException(os, indent+1);
      }

    EmitIndent(os, indent);
    Emit(os, "}\n");
    Emit(os, "\n");
    }

  if (openedTryFinally)
    {
    EmitIndent(os, indent-1);
    Emit(os, "}\n");
    EmitIndent(os, indent-1);
    Emit(os, "finally\n");
    EmitIndent(os, indent-1);
    Emit(os, "{\n");
    }

  EmitIndent(os, indent);
  Emit(os, "base.Dispose(disposing);\n");

  if (openedTryFinally)
    {
    EmitIndent(os, indent-1);
    Emit(os, "}\n");
    }

  EmitIndent(os);
  Emit(os, "}\n");
}


//----------------------------------------------------------------------------
struct SortByFieldOffset
{
  bool operator()(const cable::Field* f1, const cable::Field* f2)
  {
    return f1->GetOffset() < f2->GetOffset();
  }
};


//----------------------------------------------------------------------------
void MummyCsharpGenerator::EmitCSharpWrapperClassAsStruct(gxsys_ios::ostream &os, const cable::Class *c)
{
  gxsys_stl::vector<cable::Field*> fields;
  gxsys_stl::vector<cable::Field*>::iterator fit;
  gxsys_stl::string derivedName;
  gxsys_stl::string fieldType;
  gxsys_stl::vector<gxsys_stl::string> docblock;
  bool isPartial = this->GetSettings()->GetPartialClass(c);
  bool fieldAccess = !HasAnnotation(c, "iwhNoFieldAccess");

  // First iterate and collect all the fields in a local vector:
  //
  for (cable::Context::Iterator it = c->Begin(); it != c->End(); ++it)
    {
    cable::Field* f = cable::Field::SafeDownCast(*it);
    if (f)
      {
      fields.push_back(f);
      }
    }

  // Sort the vector so that we can emit the fields in the same order
  // in which they appear in the original C++ struct/class:
  //
  gxsys_stl::sort(fields.begin(), fields.end(), SortByFieldOffset());

  // Emit class opening:
  //
  Emit(os, "public ");
  if (isPartial)
    {
    Emit(os, "partial ");
    }
  Emit(os, "struct ");
  Emit(os, GetWrappedClassName(c).c_str());
  Emit(os, "\n");
  Emit(os, "{\n");

  // Enums:
  //
  EmitCSharpEnums(os, c);

  // Now iterate and emit a private data member for each field:
  //
  for (fit = fields.begin(); fit != fields.end(); ++fit)
    {
    cable::Field* f = *fit;
    if (f)
      {
      derivedName = ExtractDerivedName(f->GetName(), f, this->GetSettings()->GetVerbose());
      fieldType = GetCSharpTypeString(f->GetType(), false, false);

      // "bool" is special - help it marshal properly to a C++ bool struct
      // data member...
      //
      // Using C# bool here causes an InteropServices.MarshalDirectiveException
      // with a message that says the type cannot be marshalled via PInvoke.
      // So we use C# byte instead and make it look like a bool through the
      // public accessors emitted in the next step.
      //
      if (fieldType == "bool")
        {
        fieldType = "byte";
        }

      EmitIndent(os);
      Emit(os, "private ");
      Emit(os, fieldType.c_str());
      Emit(os, " m_");
      Emit(os, derivedName.c_str());
      Emit(os, ";");
      Emit(os, "\n");
      }
    }

  // Iterate and emit public accessors for each private data member:
  //
  for (fit = fields.begin(); fieldAccess && fit != fields.end(); ++fit)
    {
    cable::Field* f = *fit;
    if (f)
      {
      derivedName = ExtractDerivedName(f->GetName(), f, false);
      fieldType = GetCSharpTypeString(f->GetType(), false, false);

      Emit(os, "\n");

      this->GetHeaderFileReader(c)->GetCommentBlockBefore(f->GetLine(), docblock, this->ClassLineNumber);
      EmitDocumentationBlock(os, docblock, 1);
      docblock.clear();

      EmitIndent(os);
      Emit(os, "public ");
      Emit(os, fieldType.c_str());
      Emit(os, " ");
      Emit(os, derivedName.c_str());
      Emit(os, "\n");

      EmitIndent(os);
      Emit(os, "{");
      Emit(os, "\n");

      EmitIndent(os, 2);
      Emit(os, "get\n");

      EmitIndent(os, 2);
      Emit(os, "{\n");

      EmitIndent(os, 3);
      Emit(os, "return this.m_");
      Emit(os, derivedName.c_str());
      if (fieldType == "bool")
        {
        // "bool" is special -- see above comments...
        //
        Emit(os, " == 0 ? false : true");
        }
      Emit(os, ";\n");

      EmitIndent(os, 2);
      Emit(os, "}\n");

      Emit(os, "\n");

      EmitIndent(os, 2);
      Emit(os, "set\n");

      EmitIndent(os, 2);
      Emit(os, "{\n");

      EmitIndent(os, 3);
      Emit(os, "this.m_");
      Emit(os, derivedName.c_str());
      Emit(os, " = ");
      if (fieldType == "bool")
        {
        // "bool" is special -- see above comments...
        //
        Emit(os, "(byte)(value ? 1 : 0)");
        }
      else
        {
        Emit(os, "value");
        }
      Emit(os, ";\n");

      EmitIndent(os, 2);
      Emit(os, "}\n");

      EmitIndent(os);
      Emit(os, "}");
      Emit(os, "\n");
      }
    }

  // Caller emits closing brace for struct so that he can emit
  // extraCSharpCode if necessary before the closing brace...
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::GatherWrappedMethods(
  const cable::Class *c,
  gxsys_stl::vector<cable::Method*>& wrapped_methods,
  cable::Method*& factoryM,
  cable::Method*& disposalM,
  cable::Method*& registerM,
  cable::Method*& unRegisterM,
  bool includeParentMethods
  )
{
  // When recursion hits the top of the class hierarchy, we're done:
  //
  if (!c)
    {
    return;
    }

  gxsys_stl::string factoryMethod(this->GetSettings()->GetFactoryMethod(c));
  gxsys_stl::string disposalMethod(this->GetSettings()->GetDisposalMethod(c));
  gxsys_stl::string registerMethod(this->GetSettings()->GetRegisterMethod(c));
  gxsys_stl::string unRegisterMethod(this->GetSettings()->GetUnRegisterMethod(c));

  // Reset state only if we are iterating the methods of the *target* class.
  // (And only track factory method if iterating the target class...)
  //
  if (c == this->GetTargetClass())
    {
    this->CurrentMethodId = 0;
    this->MethodIdMap.clear();
    WrappedMethods.clear();
    wrapped_methods.clear();
    factoryM = 0;
    disposalM = 0;
    registerM = 0;
    unRegisterM = 0;
    }

  // If including parents, do so first so that the list of methods is in
  // "superclass first" order... And so that tracked pointers get set
  // to the most derived override possible.
  //
  if (includeParentMethods)
    {
    this->GatherWrappedMethods(GetWrappableParentClass(c), wrapped_methods,
      factoryM, disposalM, registerM, unRegisterM, true);
    }

  // Iterate class c's methods, adding wrappable ones to wrapped_methods and
  // tracking "special" methods as we encounter them:
  //
  for (cable::Context::Iterator it = c->Begin(); it != c->End(); ++it)
    {
    cable::Method *m = cable::Method::SafeDownCast(*it);

    if (m && MethodIsWrappable(m, it.GetAccess()))
      {
      gxsys_stl::string signature(GetMethodSignature(c, m));

      if ((c == this->GetTargetClass()) && signature == factoryMethod + "()")
        {
        LogVerboseInfo(<< "Found factory method, signature: " << signature);
        factoryM = m;
        this->MethodIdMap.insert(gxsys_stl::make_pair(m, ++this->CurrentMethodId));
        }
      else if (signature == disposalMethod + "()")
        {
        LogVerboseInfo(<< "Found disposal method, signature: " << signature);
        disposalM = m;
        this->MethodIdMap.insert(gxsys_stl::make_pair(m, ++this->CurrentMethodId));
        }
      else if (gxsys::SystemTools::StringStartsWith(registerMethod.c_str(),
        (gxsys_stl::string(m->GetName())+"(").c_str()))
        {
        LogVerboseInfo(<< "Found register method, signature: " << signature);
        registerM = m;
        this->MethodIdMap.insert(gxsys_stl::make_pair(m, ++this->CurrentMethodId));
        }
      else if (gxsys::SystemTools::StringStartsWith(unRegisterMethod.c_str(),
        (gxsys_stl::string(m->GetName())+"(").c_str()))
        {
        LogVerboseInfo(<< "Found unregister method, signature: " << signature);
        unRegisterM = m;
        this->MethodIdMap.insert(gxsys_stl::make_pair(m, ++this->CurrentMethodId));
        }
      else if (!WrappedMethodExists(signature))
        {
        WrappedMethods.insert(gxsys_stl::make_pair(signature, MethodInstance(c, m)));
        wrapped_methods.push_back(m);
        this->MethodIdMap.insert(gxsys_stl::make_pair(m, ++this->CurrentMethodId));
        }
      else
        {
        if (this->GetSettings()->GetVerbose())
          {
          //Emit(os, "//WARNING: ");
          //Emit(os, GetAccessString(it.GetAccess()));
          //Emit(os, " ");
          //Emit(os, (*it)->GetNameOfClass());
          //Emit(os, " '");
          //Emit(os, (*it)->GetName());
          //Emit(os, "' wrappable method *NOT WRAPPED* because its signature matches a method that was already wrapped...\n");
          }
        }
      }

    if (this->GetSettings()->GetVerbose())
      {
      if (!m)
        {
        LogInfo(mi_VerboseInfo, << GetAccessString(it.GetAccess())
          << " "
          << (*it)->GetNameOfClass()
          << " '"
          << (*it)->GetName()
          << "' not wrapped because it's not a method...\n"
          );
        }
      }
    }
}


//----------------------------------------------------------------------------
struct SortByMethodDeclarationLineNumber
{
  bool operator()(const cable::Method* m1, const cable::Method* m2)
  {
    return m1->GetLine() < m2->GetLine();
  }
};


//----------------------------------------------------------------------------
bool MummyCsharpGenerator::ValidateWrappedMethods(
  const cable::Class *,
  gxsys_stl::vector<cable::Method*>& wrapped_methods,
  cable::Method*&,
  cable::Method*&,
  cable::Method*&,
  cable::Method*&
  )
{
  bool valid = true;
  gxsys_stl::vector<cable::Method*> wrapped_methods_local;
  gxsys_stl::vector<cable::Method*>::iterator mit;
  cable::Method* m = 0;
  gxsys::RegularExpression reGet;
  gxsys::RegularExpression reSet;

  reGet.compile("^[Gg]et");
  reSet.compile("^[Ss]et");

  // Make a local copy of the wrapped_methods vector so we can sort it by
  // "method declaration line number"...
  //
  gxsys_stl::copy(wrapped_methods.begin(), wrapped_methods.end(),
    gxsys_stl::back_inserter(wrapped_methods_local));

  // Sort the vector so that we can emit the fields in the same order
  // in which they appear in the original C++ struct/class:
  //
  gxsys_stl::sort(wrapped_methods_local.begin(), wrapped_methods_local.end(),
    SortByMethodDeclarationLineNumber());

  for (mit = wrapped_methods_local.begin(); mit != wrapped_methods_local.end(); ++mit)
    {
    m = *mit;

    cable::FunctionType *ft = m->GetFunctionType();
    unsigned int cArgs = ft->GetNumberOfArguments();
    unsigned int cReqArgs = ft->GetNumberOfRequiredArguments();
    cable::Type *retType = ft->GetReturns();
    bool voidReturn = false;
    bool iwhPropGetExempt = false;

    if (IsVoid(retType))
      {
      voidReturn = true;
      }

    if (cArgs != cReqArgs)
      {
      // Method has at least one default arg... Warn that mummy is ignoring
      // any default argument values...
      //
      LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_DefaultArgumentValuesIgnored,
        "ignoring default argument values for method '" << m->GetName() << "'.");
      }

    if (reGet.find(m->GetName()))
      {
      //
      // It's a "getter" : warn if it returns "void" or if it's not a const
      // method or if it's missing the iwhPropGet hint...
      //

      if (gxsys_stl::string("GetEnumerator") == m->GetName())
        {
        iwhPropGetExempt = true;
        }

      // The mw_PropGetReturnsVoid and mw_PropGetHasArgs warnings are based on
      // the desire that a simple getter method should return one and only one
      // thing by return value, not through one or more "byref" arguments...
      //
      if (voidReturn)
        {
        LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_PropGetReturnsVoid,
          "'Getter' method '" << m->GetName() << "' returns void.");
        }

      if (cArgs)
        {
        LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_PropGetHasArgs,
          "'Getter' method '" << m->GetName() << "' has arguments. Should it?");
        }

      if (!iwhPropGetExempt && !HasAnnotation(m, "iwhPropGet"))
        {
        if (!voidReturn && 0==cArgs)
          {
          LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_SeriousMissingPropGetHint,
            "'Getter' method '" << m->GetName() << "' is a perfect candidate for the 'iwhPropGet' hint. Add the 'iwhPropGet' hint to eliminate this warning.");
          }
        else
          {
          LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_MissingPropGetHint,
            "'Getter' method '" << m->GetName() << "' does not have the 'iwhPropGet' hint. Should it?");
          }
        }

      if (!m->GetConst() && !m->GetStatic())
        {
        LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_PropGetNotConst,
          "'Getter' method '" << m->GetName() << "' is not const. Should it be const?");
        }
      }

    if (reSet.find(m->GetName()))
      {
      // It's a "setter" : warn if it's missing the iwhPropSet hint:
      //
      if (!HasAnnotation(m, "iwhPropSet"))
        {
        if (voidReturn && 1==cArgs)
          {
          LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_SeriousMissingPropSetHint,
            "'Setter' method '" << m->GetName() << "' is a perfect candidate for the 'iwhPropSet' hint. Add the 'iwhPropSet' hint to eliminate this warning.");
          }
        else
          {
          LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_MissingPropSetHint,
            "'Setter' method '" << m->GetName() << "' does not have the 'iwhPropSet' hint. Should it?");
          }
        }

      if (cArgs!=1)
        {
        LogFileLineWarningMsg(m->GetFile(), m->GetLine(), mw_PropSetUnexpectedArgCount,
          "'Setter' method '" << m->GetName() << "' has " << cArgs << " arguments. Should it have exactly one argument instead?");
        }
      }
    }

  return valid;
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::BuildPropGetsAndSetsMap(
  gxsys_stl::vector<cable::Method*>& wrapped_methods,
  gxsys_stl::map<gxsys_stl::string, gxsys_stl::pair<cable::Method*, cable::Method*> >& wrapped_properties
  )
{
  gxsys_stl::vector<cable::Method*>::iterator mit;
  gxsys_stl::map<gxsys_stl::string, gxsys_stl::pair<cable::Method*, cable::Method*> >::iterator gsit;
  bool addingPropGet = false;
  bool addingPropSet = false;
  cable::Method* propGetMethod = 0;
  cable::Method* propSetMethod = 0;
  gxsys_stl::string propName;

  for (mit = wrapped_methods.begin(); mit != wrapped_methods.end(); ++mit)
    {
    addingPropGet = HasAnnotation(*mit, "iwhPropGet");
    addingPropSet = addingPropGet ? false : HasAnnotation(*mit, "iwhPropSet");

    if (addingPropGet || addingPropSet)
      {
      if (addingPropGet)
        {
        propGetMethod = *mit;
        }
      else
        {
        propGetMethod = 0;
        }

      if (addingPropSet)
        {
        propSetMethod = *mit;
        }
      else
        {
        propSetMethod = 0;
        }

      propName = ExtractDerivedName(GetWrappedMethodName(*mit).c_str(), *mit,
        this->GetSettings()->GetVerbose());

      gsit = wrapped_properties.find(propName);

      if (gsit == wrapped_properties.end())
        {
        // propName not in our map yet, add it:
        //
        wrapped_properties.insert(gxsys_stl::make_pair(propName,
          gxsys_stl::make_pair(propGetMethod, propSetMethod)));
        }
      else
        {
        // We already have an entry for propName...
        // This should be the "other" half of the pair.
        // So, if we are adding the *get*, then save the
        // existing "set" that's already in the map and
        // vice versa.
        //
        if (addingPropGet)
          {
          propSetMethod = gsit->second.second;
          }

        if (addingPropSet)
          {
          propGetMethod = gsit->second.first;
          }

        // The iterator points to the real map entry. Just
        // overwrite it:
        //
        gsit->second = gxsys_stl::make_pair(propGetMethod, propSetMethod);
        }
      }
    }


  // Analyze wrapped_properties and report anything suspicious.
  // Or just report info if verbose...
  //
  bool verbose = this->GetSettings()->GetVerbose();
  if (!wrapped_properties.empty())
    {
    gxsys_stl::string comment;

    if (verbose)
      {
      LogInfo(mi_VerboseInfo, << "Properties:");
      }

    for (gsit = wrapped_properties.begin(); gsit != wrapped_properties.end(); ++gsit)
      {
      if (gsit->second.first!=0 && gsit->second.second!=0)
        {
        comment = "ReadWrite property.";
        }
      else if (gsit->second.first!=0)
        {
        comment = "ReadOnly property.";
        }
      else if (gsit->second.second!=0)
        {
        LogFileLineWarningMsg(gsit->second.second->GetFile(),
          gsit->second.second->GetLine(),
          mw_WriteOnlyProperty,
          "A WriteOnly property '" << gsit->first <<
          "' is very unusual - did you forget to mark the 'Get' method with 'iwhPropGet'?");
        }
      else
        {
        LogError(me_InternalError, "Property with no 'get' and no 'set'... Impossible!");
        }

      if (verbose)
        {
        LogInfo(mi_VerboseInfo, << comment << "  propName: " << gsit->first
          << "  propGetMethod: " << gsit->second.first
          << "  propSetMethod: " << gsit->second.second
          );
        }
      }
    }
}


//----------------------------------------------------------------------------
void MummyCsharpGenerator::EmitCSharpWrapperClass(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c)
{
  // Gather wrapped elements:
  //
  gxsys_stl::vector<cable::Method*> wrapped_methods;
  gxsys_stl::vector<cable::Method*>::iterator mit;
  cable::Method *factoryM = 0;
  cable::Method *disposalM = 0;
  cable::Method *registerM = 0;
  cable::Method *unRegisterM = 0;
  bool verbose = this->GetSettings()->GetVerbose();
  gxsys_stl::string atts(c->GetAttributes());
  gxsys_stl::string mname;

  // The "package" directive from the gccxml input is used as a base
  // namespace. If it's not empty, prepend it to the class's namespace.
  //
  gxsys_stl::string target_namespace;
  gxsys_stl::string base_namespace(this->GetSettings()->GetPackage());
  gxsys_stl::string class_namespace(GetFullyQualifiedNameForCSharp(c->GetContext()));

  // C++ global scope means "no namespace please"
  //
  if (class_namespace == "::")
    {
    class_namespace = "";
    }

  if (base_namespace == "")
    {
    target_namespace = class_namespace;
    }
  else if (class_namespace == "")
    {
    target_namespace = base_namespace;
    }
  else
    {
    target_namespace = base_namespace + "." + class_namespace;
    }


  // Emit code:
  //
  EmitMummyVersionComments(os, "//");


  // If the class maps directly to a builtin type, then DO NOT emit any code.
  //
  gxsys_stl::string mapToType = ExtractMapToType(c);
  if (mapToType != "")
    {
    Emit(os, "\n");
    Emit(os, "//----------------------------------------------------------------------------\n");
    Emit(os, "// Unmanaged class '");
    Emit(os, GetFullyQualifiedNameForCPlusPlus(c).c_str());
    Emit(os, "' maps directly to type '");
    Emit(os, mapToType.c_str());
    Emit(os, "'.\n");
    Emit(os, "// No code generated for '");
    Emit(os, GetFullyQualifiedNameForCPlusPlus(c).c_str());
    Emit(os, "'...\n");

    if (verbose)
      {
      LogInfo(mi_VerboseInfo, << "Skipping code generation because class maps directly to native type.");
      }

    return;
    }


  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");
  Emit(os, "using System;\n");
  Emit(os, "using System.Runtime.InteropServices; // DllImport and HandleRef both live here\n");
  Emit(os, "\n");


  // If this project depends on "using" any other C# namespaces, they will be listed
  // as "Reference" elements in MummySettings.xml...
  //
  gxsys_stl::vector<gxsys_stl::string> refs;
  this->GetSettings()->GetReferences(refs);
  if (refs.size())
    {
    Emit(os, "// References\n");
    gxsys_stl::vector<gxsys_stl::string>::iterator rit;
    for (rit = refs.begin(); rit != refs.end(); ++rit)
      {
      Emit(os, "using ");
      Emit(os, rit->c_str());
      Emit(os, ";\n");
      }
    Emit(os, "\n");
    }


  // Open the (optional) namespace:
  //
  if (target_namespace != "")
    {
    Emit(os, "namespace ");
    Emit(os, target_namespace.c_str());
    Emit(os, "\n");
    Emit(os, "{\n");
    Emit(os, "\n");
    }


  // Documentation:
  //
  gxsys_stl::vector<gxsys_stl::string> docblock;
  this->ClassLineNumber = c->GetLine();

  if (gxsys::SystemTools::StringStartsWith(GetFullyQualifiedNameForCPlusPlus(c).c_str(), "vtk"))
    {
    this->GetHeaderFileReader(c)->GetFirstCommentBlock(docblock);
    }
  else
    {
    this->GetHeaderFileReader(c)->GetCommentBlockBefore(c->GetLine(), docblock, 1);
    }

  EmitDocumentationBlock(os, docblock, 0, true);


  if (IsUtilityClass(c))
    {
    // Verify no virtual methods... If any, emit error:
    //
    cable::Method* um = 0;
    bool bVirtual = false;
    for (cable::Context::Iterator umit = c->Begin(); !bVirtual && umit != c->End(); ++umit)
      {
      um = cable::Method::SafeDownCast(*umit);
      if (um && um->GetVirtual())
        {
        bVirtual = true;
        }
      }

    if (bVirtual)
      {
      LogFileLineErrorMsg(um->GetFile(), um->GetLine(), me_NoVirtualMethodsAllowed,
        "A utility class cannot have any virtual methods. The '" << um->GetName() <<
        "' method should not be virtual.");
      return;
      }

    // Utility classes get wrapped as structs:
    //
    EmitCSharpWrapperClassAsStruct(os, c);
    }
  else
    {
    if (verbose)
      {
      LogInfo(mi_VerboseInfo, << "Calling GatherWrappedMethods...");
      }

    this->GatherWrappedMethods(c, wrapped_methods, factoryM, disposalM, registerM, unRegisterM, false);

    if (verbose)
      {
      DumpLookupEntries();
      }

    this->ValidateWrappedMethods(c, wrapped_methods, factoryM, disposalM, registerM, unRegisterM);

    // Filter out prop gets and sets, putting them in their very own data structure.
    // Key in the map is the name of the property. 1st method in pair is propget,
    // 2nd method is propset.
    //
    gxsys_stl::map<gxsys_stl::string, gxsys_stl::pair<cable::Method*, cable::Method*> > wrapped_properties;
    this->BuildPropGetsAndSetsMap(wrapped_methods, wrapped_properties);

    // Now remove any entries found in the props *map* from the methods *vector*.
    // Otherwise, we'd end up with all of the properties "repeated" as methods, too.
    //
    gxsys_stl::map<gxsys_stl::string, gxsys_stl::pair<cable::Method*, cable::Method*> >::iterator gsit;
    for (gsit = wrapped_properties.begin(); gsit != wrapped_properties.end(); ++gsit)
      {
      if (gsit->second.first)
        {
        mit = gxsys_stl::find(wrapped_methods.begin(), wrapped_methods.end(),
          gsit->second.first);
        if (mit != wrapped_methods.end())
          {
          wrapped_methods.erase(mit);
          }
        else
          {
          LogWarning(mw_InternalWarning, << "Unexpected unfound propget method...");
          }
        }

      if (gsit->second.second)
        {
        mit = gxsys_stl::find(wrapped_methods.begin(), wrapped_methods.end(),
          gsit->second.second);
        if (mit != wrapped_methods.end())
          {
          wrapped_methods.erase(mit);
          }
        else
          {
          LogWarning(mw_InternalWarning, << "Unexpected unfound propset method...");
          }
        }
      }


    ClassWrappingSettings cws;
    if (!this->GetSettings()->FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
      {
      LogError(me_NoClassWrappingSettings,
        << "error: no ClassWrappingSettings for class " << GetFullyQualifiedNameForCPlusPlus(c).c_str());
      }

    const cable::Class *parent = GetWrappableParentClass(c);
    bool isPartial = cws.partialClass;
    bool emitExceptionParams = !cws.exceptionBaseClass.empty();

    // Class declaration:
    //
    Emit(os, "public ");
    if (c->GetAbstract())
      {
      Emit(os, "abstract ");
      }
    if (isPartial)
      {
      Emit(os, "partial ");
      }
    Emit(os, "class ");
    Emit(os, GetWrappedClassName(c).c_str());
    Emit(os, " : ");
    if (parent)
      {
      Emit(os, GetWrappedClassNameFullyQualified(parent).c_str());
      }
    else
      {
      gxsys_stl::string wrappedObjectBase(cws.wrappedObjectBase);

      if (wrappedObjectBase.empty())
        {
        wrappedObjectBase = "Kitware.mummy.Runtime.WrappedObject";
        }

      Emit(os, wrappedObjectBase.c_str());
      }


    // Any interface(s)? david.cole::fix - allow potentially many interfaces,
    // extract a list here if necessary rather than just one string...
    //
    gxsys_stl::string iface(ExtractImplementsInterface(atts));

    if (!iface.empty())
      {
      if (iface == "IEnumerable")
        {
        this->AddTargetInterface(iface);
        Emit(os, ", System.Collections.IEnumerable");
        }
      else if (iface == "IEnumerator")
        {
        this->AddTargetInterface(iface);
        Emit(os, ", System.Collections.IEnumerator");
        }
      else
        {
        this->AddTargetInterface(iface);
        Emit(os, ", ");
        Emit(os, iface.c_str());
        }
      }

    Emit(os, "\n");


    // Open class:
    //
    Emit(os, "{\n");


    // david.cole::fix - GetFullyQualifiedNameForCSharp should handle
    // target_namespace being set in the gccxml input file, but it currently
    // does not. It relies on exact mapping of the C++ namespaces, which does
    // not allow for "pushing" stuff in the global C++ namespace into a C#
    // namespace as we do in the Vehicles example and Kitware.VTK wrappers...
    //
    gxsys_stl::string fullCSharpName;
    if (target_namespace != "")
      {
      fullCSharpName = target_namespace + "." + GetWrappedClassName(c);
      }
    else
      {
      fullCSharpName = GetWrappedClassName(c);
      }


    // Register type info with the mummy.Runtime for *all* classes,
    // even abstract classes. Type registration needs to occur even
    // if the first call to the dll is a static method on an abstract
    // class (which forces the static constructor to run prior to
    // entering the static method...)
    //
    EmitIndent(os);
    Emit(os, "/// <summary>\n");
    EmitIndent(os);
    Emit(os, "/// Automatically generated type registration mechanics.\n");
    EmitIndent(os);
    Emit(os, "/// </summary>\n");
    EmitIndent(os);
    Emit(os, "public new static readonly string MRClassNameKey = \"");
    Emit(os, GetFullyQualifiedCPlusPlusTypeIdName(c).c_str());
    Emit(os, "\";\n");
    Emit(os, "\n");

    EmitIndent(os);
    Emit(os, "/// <summary>\n");
    EmitIndent(os);
    Emit(os, "/// Automatically generated type registration mechanics.\n");
    EmitIndent(os);
    Emit(os, "/// </summary>\n");
    EmitIndent(os);
    Emit(os, "public new const string MRFullTypeName = \"");
    Emit(os, fullCSharpName.c_str());
    Emit(os, "\";\n");
    Emit(os, "\n");

    EmitIndent(os);
    Emit(os, "/// <summary>\n");
    EmitIndent(os);
    Emit(os, "/// Automatically generated type registration mechanics.\n");
    EmitIndent(os);
    Emit(os, "/// </summary>\n");
    EmitIndent(os);
    Emit(os, "static ");
    Emit(os, GetWrappedClassName(c).c_str());
    Emit(os, "()\n");
    EmitIndent(os);
    Emit(os, "{\n");

    EmitIndent(os, 2);
    Emit(os, "Kitware.mummy.Runtime.Methods.RegisterType(\n");
    EmitIndent(os, 3);
    Emit(os, "System.Reflection.Assembly.GetExecutingAssembly(),\n");
    EmitIndent(os, 3);
    Emit(os, "MRClassNameKey,\n");
    EmitIndent(os, 3);
    Emit(os, "System.Type.GetType(MRFullTypeName)\n");
    EmitIndent(os, 3);
    Emit(os, ");\n");

    EmitIndent(os);
    Emit(os, "}\n");

    Emit(os, "\n");
    Emit(os, "\n");


    // Count events *now* before EmitCSharpDisposalMethod.
    // But emit code for the events further below.
    //
    // Also, examine the full set of events to be generated for this class.
    // For any duplicate names, use GetQualifiedEventName instead of
    // GetEventName in the methodEventNames map.
    //
    unsigned int eventCount = 0;
    gxsys_stl::map<gxsys_stl::string, int> event_name_counter;
    gxsys_stl::map<const cable::Method*, gxsys_stl::string> methodEventNames;
    gxsys_stl::string eventName;

    // Count events and fill in event_name_counter as we go:
    //
    for (mit = wrapped_methods.begin(); mit != wrapped_methods.end(); ++mit)
      {
      if (MethodWrappableAsEvent(*mit, cable::Context::Public))
        {
        ++eventCount;

        eventName = GetEventName(*mit);

        event_name_counter[eventName]++;
        }
      }

    // Use event_name_counter to build a map of cable::Method* to event names.
    // If a method's GetEventName is a duplicate according to event_name_counter
    // then use GetQualifiedEventName for that method's event name... With this
    // data, we can simply look up the proper event name when given the
    // cable::Method* later when we are calling EmitCSharpEvent.
    //
    for (mit = wrapped_methods.begin(); mit != wrapped_methods.end(); ++mit)
      {
      if (MethodWrappableAsEvent(*mit, cable::Context::Public))
        {
        eventName = GetEventName(*mit);

        if (1 == event_name_counter[eventName])
          {
          methodEventNames[*mit] = eventName;
          }
        else
          {
          methodEventNames[*mit] = GetQualifiedEventName(*mit);
          }
        }
      }

    // Now verify that we have non-duplicate event names. Reset event_name_counter
    // and this time analyze whether there are duplicate strings within the
    // methodEventNames map.
    //
    gxsys_stl::map<const cable::Method*, gxsys_stl::string>::iterator menIt;
    event_name_counter.clear();
    for (mit = wrapped_methods.begin(); mit != wrapped_methods.end(); ++mit)
      {
      if (MethodWrappableAsEvent(*mit, cable::Context::Public))
        {
        menIt = methodEventNames.find(*mit);

        if (menIt != methodEventNames.end())
          {
          eventName = menIt->second;
          event_name_counter[eventName]++;

          if (event_name_counter[eventName] > 1)
            {
            LogFileLineWarningMsg((*mit)->GetFile(), (*mit)->GetLine(), mw_DuplicateGeneratedName,
              << "duplicate event name found: " << eventName.c_str()
              << " (a C# compile error will likely follow...)"
              );
            }
          }
        else
          {
          LogFileLineErrorMsg((*mit)->GetFile(), (*mit)->GetLine(), me_InternalError,
            << "event method not found in methodEventNames map...");
          }
        }
      }


    // Constructor(s):
    //
    gxsys_stl::string ctorModifier(this->GetSettings()->GetCsharpConstructorModifier(c));
    if (ctorModifier.empty())
      {
      ctorModifier = "public";
      }
    EmitIndent(os);
    Emit(os, "/// <summary>\n");
    EmitIndent(os);
    Emit(os, "/// Automatically generated constructor - called from generated code.\n");
    EmitIndent(os);
    Emit(os, "/// DO NOT call directly.\n");
    EmitIndent(os);
    Emit(os, "/// </summary>\n");
    EmitIndent(os);
    Emit(os, ctorModifier.c_str());
    Emit(os, " ");
    Emit(os, GetWrappedClassName(c).c_str());
    Emit(os, "(IntPtr rawCppThis, bool callDisposalMethod, bool strong) :\n");
    EmitIndent(os, 2);
    Emit(os, "base(rawCppThis, callDisposalMethod, strong)\n");
    EmitIndent(os);
    Emit(os, "{\n");
    EmitIndent(os);
    Emit(os, "}\n");


    // Factory method:
    //
    const cable::Method* fmp = 0;
    gxsys_stl::string factoryMethod = this->GetSettings()->GetFactoryMethod(c);
    const cable::Constructor* ctor = FindNonAbstractPublicDefaultConstructor(c);

    if (!factoryMethod.empty() && factoryMethod!="new")
      {
      if (factoryM)
        {
        fmp = factoryM;
        mname = fmp->GetName();
        }
      }
    else
      {
      fmp = ctor;
      mname = "new";
      }

    if (fmp || this->GetSettings()->GetUseShadow(c))
      {
      Emit(os, "\n");
      Emit(os, "\n");
      EmitCSharpConstructor(os, dllname, c, fmp, mname, emitExceptionParams);
      }


    // Register method:
    //
    if (registerM)
      {
      mname = registerM->GetName();

      Emit(os, "\n");
      Emit(os, "\n");
      EmitCSharpRegister(os, dllname, c, registerM, mname, emitExceptionParams);
      }


    // Disposal method:
    //
    // (disposalM/unRegisterM may be NULL, but we always emit a Dispose...
    // the guts of it vary based on disposalM/unRegisterM, but it's always
    // there and can be used as a cleaning spot for disconnecting any
    // outstanding event RelayHandler delegates...)
    //
    // If this is a ref-counted class, then use unRegisterM instead of disposalM:
    //
    const cable::Method* dmp = 0;
    gxsys_stl::string disposalMethod = this->GetSettings()->GetDisposalMethod(c);
    gxsys_stl::string unRegisterMethod = this->GetSettings()->GetUnRegisterMethod(c);

    if (!unRegisterMethod.empty())
      {
      if (unRegisterM)
        {
        dmp = unRegisterM;
        mname = dmp->GetName();
        }
      }
    else if (!disposalMethod.empty())
      {
      if (disposalM)
        {
        dmp = disposalM;
        mname = dmp->GetName();
        }
      }
    else
      {
      dmp = ctor;
      mname = "delete";
      }

    // Call EmitCSharpDispose even if dmp is NULL...
    //
    Emit(os, "\n");
    Emit(os, "\n");
    EmitCSharpDispose(os, dllname, c, dmp, mname, eventCount, emitExceptionParams);


    // Enums:
    //
    EmitCSharpEnums(os, c);


    // Delegates:
    //
    for (cable::Context::Iterator it = c->Begin(); it != c->End(); ++it)
      {
      cable::Typedef *t = cable::Typedef::SafeDownCast(*it);

      if (t && (cable::Context::Public == it.GetAccess()))
        {
        bool isDelegate = false;
        gxsys_stl::string tname(t->GetName());

        //isDelegate = HasAnnotation(t, "iwhDelegate");

        cable::PointerType *pt = cable::PointerType::SafeDownCast(t->GetType());
        cable::FunctionType *ft = 0;
        if (pt)
          {
          ft = cable::FunctionType::SafeDownCast(pt->GetTarget());
          }
        if (ft)
          {
          isDelegate = true;
          }

        if (isDelegate)
          {
          Emit(os, "\n");
          Emit(os, "\n");

          docblock.clear();
		  this->GetHeaderFileReader(c)->GetCommentBlockBefore(t->GetLine(), docblock, this->ClassLineNumber);
          EmitDocumentationBlock(os, docblock, 1);

          EmitIndent(os);
          Emit(os, "public delegate ");

          unsigned int cArgs = ft->GetNumberOfArguments();
          cable::Type *argType = 0;
          cable::Type *retType = ft->GetReturns();

          // The following chunk is a near-copy of the bottom of
          // EmitCSharpMethodDeclaration...
          // <Chunk>

          // Does method return an array?
          //
          gxsys_stl::string arraySize = "";
          atts = t->GetAttributes();
          if (atts != "")
            {
            // Use ExtractArraySize instead of GetMethodArgumentArraySize here.
            // This is a delegate definition and cannot be in the VTK hints file.
            // (since the hints file only covers "normal" C++ methods...)
            // GetMethodArgumentArraySize uses ExtractArraySize internally, and
            // if there is no inline hint, it then examines the externalHints
            // file to see if there's a match. In this case, there could be no
            // match anyhow (as evidenced by the fact that we do not have access
            // to a cable::Method here...) so simply use ExtractArraySize directly.
            // Same reasoning applies below with the next use of ExtractArraySize.
            //
            arraySize = ExtractArraySize(atts);
            }

          // Return type:
          Emit(os, GetPInvokeTypeString(retType, true, arraySize != "", true).c_str());
          if (arraySize != "")
            {
            Emit(os, "[]");
            }
          Emit(os, " ");

          // Use the typedef name:
          Emit(os, t->GetName());

          // Open args:
          Emit(os, "(");

          // The C# args:
          unsigned int i;
          for (i= 0; i<cArgs; ++i)
            {
            argType = ft->GetArgument(i);

            // Is arg an array?
            //
            arraySize = "";
            atts = ft->GetArgumentAttributes(i);
            if (atts != "")
              {
              // See comments above regarding direct use of ExtractArraySize
              // instead of GetMethodArgumentArraySize.
              //
              arraySize = ExtractArraySize(atts);
              }

            // arg type:
            Emit(os, GetPInvokeTypeString(argType, false, arraySize != "", true).c_str());

            // array notation:
            if (arraySize != "")
              {
              Emit(os, "[]");
              }

            // arg name:
            Emit(os, " ");
            Emit(os, GetArgName(ft, i));

            if (i<cArgs-1)
              {
              Emit(os, ", ");
              }
            }

          // Close args:
          Emit(os, ")");

          // </Chunk>


          Emit(os, ";\n");
          }
        }
      }


    // Events:
    //
    if (0 != eventCount)
      {
      for (mit = wrapped_methods.begin(); mit != wrapped_methods.end(); ++mit)
        {
        if (MethodWrappableAsEvent(*mit, cable::Context::Public))
          {
          Emit(os, "\n");
          Emit(os, "\n");

          menIt = methodEventNames.find(*mit);
          if (menIt != methodEventNames.end())
            {
            eventName = menIt->second;
            }
          else
            {
            eventName = GetEventName(*mit);
            LogFileLineErrorMsg((*mit)->GetFile(), (*mit)->GetLine(), me_InternalError,
              << "event method not found in methodEventNames map...");
            }

          EmitCSharpEvent(os, dllname, c, *mit, eventName);
          }
        }

      // Add a special "disconnect all" method that can be called at Dispose
      // time so that the unmanaged side does not end up with a stale pointer
      // to a garbage collected event RelayHandler...
      //
      Emit(os, "\n");
      Emit(os, "\n");
      EmitIndent(os);
      Emit(os, "/// <summary>\n");
      EmitIndent(os);
      Emit(os, "/// Method to disconnect all RelayHandler event members.\n");
      EmitIndent(os);
      Emit(os, "/// Called automatically from Dispose. DO NOT call directly.\n");
      EmitIndent(os);
      Emit(os, "/// </summary>\n");
      EmitIndent(os);
      Emit(os, "private void RemoveAllRelayHandlers()\n");
      EmitIndent(os);
      Emit(os, "{\n");

      for (mit = wrapped_methods.begin(); mit != wrapped_methods.end(); ++mit)
        {
        if (MethodWrappableAsEvent(*mit, cable::Context::Public))
          {
          menIt = methodEventNames.find(*mit);
          if (menIt != methodEventNames.end())
            {
            eventName = menIt->second;
            }
          else
            {
            eventName = GetEventName(*mit);
            LogFileLineErrorMsg((*mit)->GetFile(), (*mit)->GetLine(), me_InternalError,
              << "event method not found in methodEventNames map...");
            }

          EmitIndent(os, 2);
          Emit(os, "this.Remove");
          Emit(os, eventName.c_str());
          Emit(os, "RelayHandler();\n");
          }
        }

      EmitIndent(os);
      Emit(os, "}\n");
      }


    // Properties:
    //
    for (gsit = wrapped_properties.begin(); gsit != wrapped_properties.end(); ++gsit)
      {
      Emit(os, "\n");
      Emit(os, "\n");
      EmitCSharpProperty(os, dllname, c, gsit->second.first, gsit->second.second, emitExceptionParams);
      }


    // Plain old methods:
    //
    for (mit = wrapped_methods.begin(); mit != wrapped_methods.end(); ++mit)
      {
      Emit(os, "\n");
      Emit(os, "\n");
      EmitCSharpMethod(os, dllname, c, *mit, (*mit)->GetName(), "public", emitExceptionParams);
      }
    }


  // Hand written shtuff:
  //
  // If there is extraCSharpCode, emit it *within* the class definition.
  // If it's there, it's the name of a file that we are to include in
  // its entirety...
  //
  gxsys_stl::string extraCode = this->GetSettings()->GetExtraCsharpCode(c);
  if (extraCode != "")
    {
    Emit(os, "\n");
    Emit(os, "\n");
    Emit(os, "// Begin extraCsharpCode\n");
    Emit(os, "\n");
    EmitFile(os, extraCode.c_str());
    Emit(os, "\n");
    Emit(os, "// End extraCsharpCode\n");
    Emit(os, "\n");
    }


  // Close the struct/class:
  //
  Emit(os, "}\n");


  // Close the namespace:
  //
  if (target_namespace != "")
    {
    Emit(os, "\n");
    Emit(os, "}\n");
    }
}
