//----------------------------------------------------------------------------
//
//  $Id: MummyUtilities.cxx 470 2009-06-12 17:43:02Z hoffman $
//
//  $Author: hoffman $
//  $Date: 2009-06-12 13:43:02 -0400 (Fri, 12 Jun 2009) $
//  $Revision: 470 $
//
//  Copyright (C) 2006-2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MummyUtilities.h"
#include "MummyLog.h"

#include "cableClass.h"
#include "cableClassType.h"
#include "cableConstructor.h"
#include "cableFunctionType.h"
#include "cableNamed.h"
#include "cablePointerType.h"
#include "cableReferenceType.h"
#include "cableType.h"
#include "cableTypedef.h"

#include "cxxFundamentalType.h"

#include "gxsys/RegularExpression.hxx"
#include "gxsys/SystemTools.hxx"
#include "gxsys/ios/fstream"
#include "gxsys/ios/sstream"
#include "gxsys/stl/map"

#ifdef _WIN32
#include <windows.h> // only for "OutputDebugString"
#endif // _WIN32

#include "string.h" // strlen

#ifdef IWH_USE_GCCXML_ATTRIBUTE
const std::string ANNOTATION_TEXT = "gccxml";
#else
const std::string ANNOTATION_TEXT = "annotate";
#endif

const std::string ANNOTATION_FORMAT("%s(%s");
const std::string ANNOTATION_REGULAR_EXPRESSION_FORMAT("(%s\\(%s)([^\\)]*)(\\))");

//----------------------------------------------------------------------------
void Trace(const char *s)
{
#ifdef _WIN32
	OutputDebugString(s);
#endif // _WIN32

	//LogInfo(mi_Info, << s);
}


//----------------------------------------------------------------------------
void Emit(gxsys_ios::ostream &os, const char *s)
{
	//Trace(s);
	os << s;
}


//----------------------------------------------------------------------------
void EmitInt(gxsys_ios::ostream &os, const int i)
{
	os << i;
}


//----------------------------------------------------------------------------
void EmitUint(gxsys_ios::ostream &os, const unsigned int i)
{
	os << i;
}


//----------------------------------------------------------------------------
void EmitIndent(gxsys_ios::ostream &os, const unsigned int n)
{
  // See also: indentString in EmitDocumentationBlock. If we change it here,
  // we probably will want to change it there also...
  //
	unsigned int i = 0;
	for (i= 0; i<n; ++i)
	{
		Emit(os, "   ");
	}
}


//----------------------------------------------------------------------------
void EmitFile(gxsys_ios::ostream &os, const char *filename)
{
	gxsys_ios::ifstream file(filename);
	if (file)
	{
		char line[4100];
		while (!file.eof())
		{
			line[0] = 0;
			file.getline(line, 4099);
			os << line << gxsys_ios::endl;
		}
	}
}


//----------------------------------------------------------------------------
void WriteToFile(const char *filename, const char *s)
{
	gxsys_ios::ofstream file;
	file.open(filename, gxsys_ios::ios_base::out | gxsys_ios::ios_base::trunc);
	if (file)
	{
		file << s;
		file.flush();
		file.close();
	}
}


//----------------------------------------------------------------------------
bool IsChar(const cable::Type *t)
{
	return IsFundamental(t, cxx::FundamentalType::Char);
}


//----------------------------------------------------------------------------
bool IsFundamental(const cable::Type *t, cxx::FundamentalType::Id tid)
{
	if (cable::Type::FundamentalTypeId == t->GetTypeId())
	{
		const cxx::FundamentalType *cxxft = cxx::FundamentalType::SafeDownCast(
			t->GetCxxType().GetType());

		if (cxxft)
		{
			return tid == cxxft->GetId();
		}
	}

	return false;
}


//----------------------------------------------------------------------------
bool IsObject(const cable::Type *t)
{
	return cable::Type::ClassTypeId == t->GetTypeId();
}


//----------------------------------------------------------------------------
bool IsVoid(const cable::Type *t)
{
	return IsFundamental(t, cxx::FundamentalType::Void);
}


//----------------------------------------------------------------------------
bool HasMapToType(const cable::Type *t)
{
	if (IsObject(t))
	{
		return HasAnnotation(cable::ClassType::SafeDownCast(t)->GetClass(),"iwhMapToType");
	}

	if ((cable::Type::ReferenceTypeId == t->GetTypeId()) &&
		IsObject(cable::ReferenceType::SafeDownCast(t)->GetTarget()))
	{
		return HasAnnotation(cable::ClassType::SafeDownCast(cable::ReferenceType::SafeDownCast(t)->GetTarget())->GetClass(),"iwhMapToType");
	}

	return false;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetMapToType(const cable::Type *t)
{
	gxsys_stl::string s;

	if (IsObject(t))
	{
		s = ExtractMapToType(cable::ClassType::SafeDownCast(t)->GetClass());
	}
	else if ((cable::Type::ReferenceTypeId == t->GetTypeId()) &&
		IsObject(cable::ReferenceType::SafeDownCast(t)->GetTarget()))
	{
		s = ExtractMapToType(cable::ClassType::SafeDownCast(
			cable::ReferenceType::SafeDownCast(t)->GetTarget())->GetClass());
	}

	return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetStringMethod(const cable::Type *t)
{
	gxsys_stl::string s;

	if (IsObject(t))
	{
		s = ExtractStringMethod(cable::ClassType::SafeDownCast(t)->GetClass());
	}
	else if ((cable::Type::ReferenceTypeId == t->GetTypeId()) &&
		IsObject(cable::ReferenceType::SafeDownCast(t)->GetTarget()))
	{
		s = ExtractStringMethod(cable::ClassType::SafeDownCast(
			cable::ReferenceType::SafeDownCast(t)->GetTarget())->GetClass());
	}

	return s;
}


//----------------------------------------------------------------------------
bool IsCharPointer(const cable::Type *t)
{
	return IsFundamentalPointer(t, cxx::FundamentalType::Char);
}


//----------------------------------------------------------------------------
bool IsCharPointerPointer(const cable::Type *t)
{
	return IsFundamentalPointerPointer(t, cxx::FundamentalType::Char);
}


//----------------------------------------------------------------------------
bool IsFundamentalPointer(const cable::Type *t, cxx::FundamentalType::Id tid)
{
	if (cable::Type::PointerTypeId == t->GetTypeId())
	{
		return IsFundamental(cable::PointerType::SafeDownCast(t)->GetTarget(), tid);
	}

	return false;
}


//----------------------------------------------------------------------------
bool IsFundamentalPointerPointer(const cable::Type *t, cxx::FundamentalType::Id tid)
{
	if (cable::Type::PointerTypeId == t->GetTypeId())
	{
		return IsFundamentalPointer(cable::PointerType::SafeDownCast(t)->GetTarget(), tid);
	}

	return false;
}


//----------------------------------------------------------------------------
bool IsObjectPointer(const cable::Type *t)
{
	if (cable::Type::PointerTypeId == t->GetTypeId())
	{
		return IsObject(cable::PointerType::SafeDownCast(t)->GetTarget());
	}

	return false;
}


//----------------------------------------------------------------------------
bool IsObjectPointerReference(const cable::Type *t)
{
	if (cable::Type::ReferenceTypeId == t->GetTypeId())
	{
		return IsObjectPointer(cable::ReferenceType::SafeDownCast(t)->GetTarget());
	}

	return false;
}


//----------------------------------------------------------------------------
bool IsVoidPointer(const cable::Type *t)
{
	return IsFundamentalPointer(t, cxx::FundamentalType::Void);
}


//----------------------------------------------------------------------------
const char *GetAccessString(cable::Context::Access access)
{
	if (cable::Context::Public == access)
		return "public";

	if (cable::Context::Protected == access)
		return "protected";

	if (cable::Context::Private == access)
		return "private";

	LogError(me_InvalidArg, << "ERROR_invalid_input_to_GetAccessString");
	return "ERROR_invalid_input_to_GetAccessString";
}


//----------------------------------------------------------------------------
const cable::Class *GetParentClass(const cable::Class *c)
{
	const cable::Class *parent = 0;

	if (c)
	{
		gxsys_stl::vector<cable::Class *> bases;
		size_t basecount = 0;
		gxsys_stl::vector<cable::Class *>::iterator it;

		c->GetBaseClasses(bases);
		basecount = bases.size();

		if (0 == basecount)
		{
		}
		else if (1 == basecount)
		{
			parent = *bases.begin();
		}
		else
		{
			LogError(
				me_MoreThanOneBaseClass,
				<< "GetParentClass returning 0 because there is more than one base for class '"
				<< c->GetName() << "'"
				);
		}
	}

	return parent;
}


//----------------------------------------------------------------------------
bool ClassIsA(const cable::Class *c, const gxsys_stl::string& parent)
{
	bool isa = false;
	const cable::Class *cIt = c;

	while (!isa && cIt != NULL)
	{
		if (GetFullyQualifiedNameForCPlusPlus(cIt) == parent)
		{
			isa = true;
		}
		else
		{
			cIt = GetParentClass(cIt);
		}
	}

	return isa;
}


//----------------------------------------------------------------------------
bool ValidateBaseClasses(const cable::Class *c)
{
	bool validated = false;
	gxsys_stl::vector<cable::Class *> bases;
	size_t basecount = 0;
	gxsys_stl::vector<cable::Class *>::iterator it;

	c->GetBaseClasses(bases);
	basecount = bases.size();

	if (0 == basecount)
	{
		validated = true;
	}
	else if (1 == basecount)
	{
		it = bases.begin();
		validated = ValidateBaseClasses(*it);
	}
	else
	{
		LogError(
			me_MoreThanOneBaseClass,
			<< "ValidateBaseClasses returning false because there is more than one base for class '"
			<< c->GetName() << "'"
			);
	}

	return validated;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetSimpleName(const cable::Named *n)
{
	gxsys_stl::string s;

	if (n)
	{
		s = n->GetName();
	}
	else
	{
		s = "ERROR_invalid_input_to_GetSimpleName";
		LogError(me_InvalidArg, << s.c_str());
	}

	return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetFullyQualifiedName(const cable::Named *n, const char *sep)
{
	gxsys_stl::string s;

	if (n && sep && n->GetContext())
	{
		s = GetFullyQualifiedName(n->GetContext(), sep);

		// Do not emit a "leading separator" at global scope:
		//
		if (s == "::")
		{
			s = "";
		}
		else
		{
			s += sep;
		}

		s += n->GetName();
	}
	else if (n && sep)
	{
		s = n->GetName();
	}
	else
	{
		s = "ERROR_invalid_input_to_GetFullyQualifiedName";
		LogError(me_InvalidArg, << s.c_str());
	}

	return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetFullyQualifiedLengthPrefixedName(const cable::Named *n, const char *sep)
{
	gxsys_stl::string s;

	if (n && sep && n->GetContext())
	{
		s = GetFullyQualifiedLengthPrefixedName(n->GetContext(), sep);

		// Do not emit a "leading separator" at global scope:
		//
		if (s == "::")
		{
			s = "";
		}
		else
		{
			s += sep;
		}

		gxsys_ios::ostringstream oss;
		oss << strlen(n->GetName());
		s += oss.str();

		s += n->GetName();
	}
	else if (n && sep)
	{
		s = n->GetName();
	}
	else
	{
		s = "ERROR_invalid_input_to_GetFullyQualifiedLengthPrefixedName";
		LogError(me_InvalidArg, << s.c_str());
	}

	return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetFullyQualifiedNameForCPlusPlus(const cable::Named *n)
{
	return GetFullyQualifiedName(n, "::");
}


//----------------------------------------------------------------------------
gxsys_stl::string GetFullyQualifiedNameForCSharp(const cable::Named *n)
{
	return GetFullyQualifiedName(n, ".");
}


//----------------------------------------------------------------------------
gxsys_stl::string GetFullyQualifiedCPlusPlusTypeIdName(const cable::Named *n)
{
	gxsys_stl::string s("ERROR_unknown_compiler_in_GetFullyQualifiedCPlusPlusTypeIdName");

#if defined(_MSC_VER)
	s = "class ";
	s += GetFullyQualifiedNameForCPlusPlus(n);
#elif defined(__GNUC__)
	s = GetFullyQualifiedLengthPrefixedName(n, "");
#endif

	return s;
}


//----------------------------------------------------------------------------
bool EquivalentTypedefNameExists(const cable::Class* c, const cable::FunctionType *target, gxsys_stl::string& s)
{
	s = "";

	// Look through the typedefs of the target class and return the name of the
	// first one found that matches the function pointer type inside of 't'...

	for (cable::Context::Iterator it = c->Begin(); it != c->End() && s == ""; ++it)
	{
		cable::Typedef *td = cable::Typedef::SafeDownCast(*it);

		if (td && (cable::Context::Public == it.GetAccess()))
		{
			gxsys_stl::string tname(td->GetName());

			cable::PointerType *pt = cable::PointerType::SafeDownCast(td->GetType());
			cable::FunctionType *ft = 0;
			if (pt)
			{
				ft = cable::FunctionType::SafeDownCast(pt->GetTarget());
			}

			if (ft && ft == target)
			{
				s = tname;
			}
		}
	}

	return (s != "");
}


//----------------------------------------------------------------------------
bool HasAttribute(const cable::SourceObject *o, const char *attr)
{
	gxsys_stl::string atts(o->GetAttributes());
	if (atts != "")
	{
		if (gxsys_stl::string::npos != atts.find(attr))
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------
bool HasAnnotation(const gxsys_stl::string& atts, const gxsys_stl::string& tag)
{
	if (atts != "")
	{
		char* text = new char[ANNOTATION_FORMAT.length() + ANNOTATION_TEXT.length() + tag.length()];
		sprintf(text, ANNOTATION_FORMAT.c_str(), ANNOTATION_TEXT.c_str(), tag.c_str());
		return (gxsys_stl::string::npos != atts.find(text));
	}

	return false;
}

//----------------------------------------------------------------------------
bool HasAnnotation(const cable::SourceObject *o, const gxsys_stl::string& tag)
{
	gxsys_stl::string atts(o->GetAttributes());
	return HasAnnotation(atts, tag);
}

//----------------------------------------------------------------------------
bool IsUtilityClass(const cable::Class *c)
{
	return HasAnnotation(c, "iwhUtility");
}

//----------------------------------------------------------------------------
gxsys_stl::string ExtractAttribute(const gxsys_stl::string& atts, const gxsys_stl::string& attBase)
{
	gxsys_stl::string extracted;
	if (HasAnnotation(atts, attBase))
	{
		char* text = new char[ANNOTATION_REGULAR_EXPRESSION_FORMAT.length() + ANNOTATION_TEXT.length() + attBase.length()];
		sprintf(text, ANNOTATION_REGULAR_EXPRESSION_FORMAT.c_str(), ANNOTATION_TEXT.c_str(), attBase.c_str());
		gxsys::RegularExpression re;
		re.compile(text);
		if (re.find(atts.c_str()))
		{
			extracted = re.match(2);
		}
	}

	return extracted;
}


//----------------------------------------------------------------------------
gxsys_stl::string ExtractAttribute(const cable::SourceObject *o, const gxsys_stl::string& attBase)
{
	gxsys_stl::string atts(o->GetAttributes());
	return ExtractAttribute(atts, attBase);
}


//----------------------------------------------------------------------------
gxsys_stl::string ExtractArraySize(const gxsys_stl::string& atts)
{
	return ExtractAttribute(atts, "iwhArraySize");
}


//----------------------------------------------------------------------------
gxsys_stl::string ExtractImplementsInterface(const gxsys_stl::string& atts)
{
	return ExtractAttribute(atts, "iwhImplementsInterface");
}


//----------------------------------------------------------------------------
gxsys_stl::string ExtractMapToType(const cable::SourceObject *o)
{
	return ExtractAttribute(o, "iwhMapToType");
}


//----------------------------------------------------------------------------
gxsys_stl::string ExtractStringMethod(const cable::SourceObject *o)
{
	return ExtractAttribute(o, "iwhStringMethod");
}


//----------------------------------------------------------------------------
gxsys_stl::string GetMappedTypeName(const cable::Class *c, bool fullyQualified)
{
	gxsys_stl::string typeName(ExtractMapToType(c));

	if (typeName == "string")
	{
		typeName = "string";
	}
	else
	{
		if (fullyQualified)
		{
			typeName = GetFullyQualifiedNameForCSharp(c);
		}
		else
		{
			typeName = GetSimpleName(c);
		}
	}

	return typeName;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetWrappedClassName(const cable::Class *c)
{
	return GetMappedTypeName(c, false);
}


//----------------------------------------------------------------------------
gxsys_stl::string GetWrappedClassNameFullyQualified(const cable::Class *c)
{
	return GetMappedTypeName(c, true);
}


//----------------------------------------------------------------------------
static gxsys_stl::map<int, int> suppressed_msg_values;


//----------------------------------------------------------------------------
void SuppressMsg(const int n)
{
	suppressed_msg_values[n] = n;
}


//----------------------------------------------------------------------------
bool ShouldLogMsg(const int n)
{
	gxsys_stl::map<int, int>::iterator it = suppressed_msg_values.find(n);
	return it == suppressed_msg_values.end();
}


//----------------------------------------------------------------------------
static gxsys_stl::vector<int> error_values;


//----------------------------------------------------------------------------
void LogMsg(const gxsys_stl::string&, const unsigned long, const gxsys_stl::string& label, const int n)
{
	if (label == "error")
	{
		if (n)
		{
			error_values.push_back(n);
		}
		else
		{
			gxsys_ios::cerr << "error: 'error' LogMsg n value should be non-zero...";
			error_values.push_back(me_InternalError);
		}
	}
}


//----------------------------------------------------------------------------
int GetErrorCount()
{
	return static_cast<int>(error_values.size());
}


//----------------------------------------------------------------------------
int GetNthErrorValue(int n)
{
	if (n >= 0 && n < GetErrorCount())
	{
		return error_values[n];
	}

	return 0;
}


//----------------------------------------------------------------------------
int GetFirstErrorValue()
{
	return GetNthErrorValue(0);
}


//----------------------------------------------------------------------------
int GetMostRecentErrorValue()
{
	return GetNthErrorValue(GetErrorCount()-1);
}


//----------------------------------------------------------------------------
const cable::Constructor* FindNonAbstractPublicDefaultConstructor(const cable::Class *c)
{
	const cable::Constructor* ctor = 0;

	if (!c->GetAbstract())
	{
		for (cable::Context::Iterator it = c->Begin(); 0 == ctor && it != c->End(); ++it)
		{
			cable::Constructor *ctorCandidate = cable::Constructor::SafeDownCast(*it);

			if (ctorCandidate &&
				cable::Context::Public == it.GetAccess() &&
				0 == ctorCandidate->GetFunctionType()->GetNumberOfRequiredArguments())
			{
				ctor = ctorCandidate;
				break;
			}
		}
	}

	return ctor;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetCPlusPlusZeroInitializerExpression(const cable::Type *t)
{
	gxsys_stl::string s;

	switch (t->GetTypeId())
	{
	case cable::Type::ClassTypeId:
		s = "memset";
		break;

	case cable::Type::EnumerationTypeId:
		s = "cast";
		break;

	case cable::Type::FundamentalTypeId:
	case cable::Type::PointerTypeId:
		s = "0";
		break;

	default:
		s = "error: unhandled cable::Type in GetCPlusPlusZeroInitializerExpression";
		LogError(me_InternalError, << s.c_str());
		break;
	}

	return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetCsharpZeroInitializerExpression(const cable::Type *t)
{
	gxsys_stl::string s;

	switch (t->GetTypeId())
	{
	case cable::Type::ClassTypeId:
		s = "null";
		break;

	case cable::Type::EnumerationTypeId:
		s = "cast";
		break;

	case cable::Type::FundamentalTypeId:
	case cable::Type::PointerTypeId:
		s = "0";
		break;

	default:
		s = "error: unhandled cable::Type in GetCsharpZeroInitializerExpression";
		LogError(me_InternalError, << s.c_str());
		break;
	}

	return s;
}


//----------------------------------------------------------------------------
bool BlockContains(const gxsys_stl::vector<gxsys_stl::string>& block, const char *value)
{
	gxsys_stl::vector<gxsys_stl::string>::const_iterator blockIt;

	for (blockIt = block.begin(); blockIt!=block.end(); ++blockIt)
	{
		if (strstr(blockIt->c_str(), value))
		{
			return true;
		}
	}

	return false;
}


//----------------------------------------------------------------------------
bool ShouldEmitComment(const char *comment)
{
	gxsys::RegularExpression re;
	re.compile("//[\\t ]*Description:[\\t ]*");
	if (re.find(comment))
	{
		return false;
	}

	return true;
}


//----------------------------------------------------------------------------
gxsys_stl::string EncodeStringForXml(const char *s)
{
	gxsys_stl::string encoded;

	if (s)
	{
		size_t i = 0;
		size_t n = strlen(s);

		for (i= 0; i<n; ++i)
		{
			switch (s[i])
			{
			case '&'  :  encoded.append("&amp;");   break;
			case '\'' :  encoded.append("&apos;");  break;
			case '>'  :  encoded.append("&gt;");    break;
			case '<'  :  encoded.append("&lt;");    break;
			case '"'  :  encoded.append("&quot;");  break;

			default   :  encoded += s[i];           break;
			}
		}
	}

	return encoded;
}


// Valid XML doc tags are:
//
//"c"
//"code"
//"description"
//"example"
//"exception "
//"include"
//"item"
//"list"
//"listheader"
//"para"
//"param"
//"paramref"
//"permission"
//"remarks"
//"returns"
//"see"
//"seealso"
//"summary"
//"term"
//"typeparam"
//"typeparamref"
//"value"


//----------------------------------------------------------------------------
void EmitDocumentationBlock(gxsys_ios::ostream &os, const gxsys_stl::vector<gxsys_stl::string>& block, const unsigned int indent, bool isClassDoc)
{
  // See also: EmitIndent. If we change indentString here,
  // we probably will want to change it there also...
  //
	gxsys::String indentString = "";
	for (unsigned int i= 0; i<indent; ++i)
	  {
		indentString+="   ";
	  }

	gxsys::String xmlTag = "summary";

	gxsys_stl::map<gxsys_stl::string, gxsys_stl::string> tagBody;

  gxsys_stl::vector<gxsys_stl::string>::const_iterator blockIt;
	gxsys_stl::string line;

	bool isXmlBlock =
    (BlockContains(block, "<summary>") && BlockContains(block, "</summary>")) ||
		(BlockContains(block, "<remarks>") && BlockContains(block, "</remarks>"));
	
	gxsys::RegularExpression descRE;
	descRE.compile(gxsys_stl::string("Description:").c_str());
	gxsys::RegularExpression nameRE;
	nameRE.compile(gxsys_stl::string("\\.NAME").c_str()); 
	gxsys::RegularExpression seeRE;
	seeRE.compile(gxsys_stl::string("\\.SECTION[\\t ]+[Ss][Ee][Ee][\\t ]+[Aa][Ll][Ss][Oo]").c_str());
	gxsys::RegularExpression secRE;
	secRE.compile(gxsys_stl::string("\\.SECTION").c_str());
	gxsys::RegularExpression noSpaceRE;
	noSpaceRE.compile(gxsys_stl::string("[^ ]+").c_str());

  for (blockIt = block.begin(); blockIt!=block.end(); ++blockIt)
	  {
		line = *blockIt;

		if (isClassDoc && !isXmlBlock)
		  {
			//If it matches a .NAME section add to summary
			if (nameRE.find(line))
			  {
				gxsys::SystemTools::ReplaceString(line,nameRE.match(0).c_str(),"");
				xmlTag = "summary";
			  }
			//If it matches a .SECTION see also section add to see also
			else if (seeRE.find(line))
			  {
				gxsys::SystemTools::ReplaceString(line,seeRE.match(0).c_str(),"");
				xmlTag = "seealso";
			  }
			//If it matches a .SECTION * section add to remarks
			else if (secRE.find(line))
			  {			
				gxsys::SystemTools::ReplaceString(line,secRE.match(0).c_str(),"");
				xmlTag = "remarks";
			  }
			//If it matches a Description: section add to summary
			else if (descRE.find(line))
			  {
				gxsys::SystemTools::ReplaceString(line,descRE.match(0).c_str(),"");
				xmlTag = "summary";
			  }
		  }

    if (ShouldEmitComment(line.c_str()))
	    {
			if (isXmlBlock)
      {
        EmitIndent(os, indent);

        if (gxsys::SystemTools::StringStartsWith(line.c_str(), "///"))
          {
          Emit(os, line.c_str());
          }
        else if (gxsys::SystemTools::StringStartsWith(line.c_str(), "//"))
          {
          Emit(os, "/");
          Emit(os, line.c_str());
          }
        else
          {
          Emit(os, "///");
          Emit(os, line.c_str());
          }
        Emit(os,"\n");
        }
      else
        {
				line = EncodeStringForXml(line.c_str());

        //handle code in comments
	  	  gxsys::SystemTools::ReplaceString(line,"\\code","<code>");
  			gxsys::SystemTools::ReplaceString(line,"\\endcode","</code>");

        if (gxsys::SystemTools::StringStartsWith(line.c_str(), "///"))
          {
          tagBody[xmlTag]+=indentString+line+"\n";
          }
        else if (gxsys::SystemTools::StringStartsWith(line.c_str(), "//"))
          {
          tagBody[xmlTag]+=indentString+"/"+line+"\n";
          }
        else
          {
          tagBody[xmlTag]+=indentString+"///"+line+"\n";
          }
        }
  		}
	  }

	if(tagBody["summary"] != "")
	  {
		gxsys::String beginTag = indentString+"/// <summary>\n";
		gxsys::String endTag = indentString+"/// </summary>\n";
		Emit(os,beginTag.c_str());
		Emit(os,(tagBody["summary"]).c_str());
		Emit(os,endTag.c_str());
	  }

	if(tagBody["remarks"] != "")
	  {
		gxsys::String beginTag = indentString+"/// <remarks>\n";
		gxsys::String endTag = indentString+"/// </remarks>\n";
		Emit(os,beginTag.c_str());
		Emit(os,(tagBody["remarks"]).c_str());
		Emit(os,endTag.c_str());
	  }

  if(tagBody["seealso"] != "")
	  {
		gxsys::String beginTag = indentString+"/// <seealso>\n";
		gxsys::String endTag = indentString+"/// </seealso>\n";
		Emit(os, beginTag.c_str());
		Emit(os, tagBody["seealso"].c_str());
		Emit(os, endTag.c_str());
	  }
}


//----------------------------------------------------------------------------
gxsys_stl::string ExtractDerivedName(const char *s, const cable::Named *n, bool verbose)
{
	gxsys_stl::string name;
	gxsys_stl::string derivedName;
	gxsys_stl::string c1to2;
	gxsys_stl::string c1to3;

	if (s)
	{
		name = s;
	}

	if (s && strlen(s)>2)
	{
		c1to2 = name.substr(0, 2);
	}

	if (s && strlen(s)>3)
	{
		c1to3 = name.substr(0, 3);
	}

	if ((c1to3 == "Get") || (c1to3 == "get"))
	{
		derivedName = name.substr(3);
	}
	else if ((c1to3 == "Set") || (c1to3 == "set"))
	{
		derivedName = name.substr(3);
	}
	else if (c1to2 == "m_")
	{
		derivedName = name.substr(2);
	}
	else
	{
		derivedName = name;

		if (n && verbose)
		{
			LogFileLineInfoMsg(n->GetFile(), n->GetLine(), mi_VerboseInfo,
				"ExtractDerivedName could not extract derivedName from name: "
				<< name << ". Using name as-is...");
		}
	}

	return derivedName;
}
