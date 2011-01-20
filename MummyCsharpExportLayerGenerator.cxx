//----------------------------------------------------------------------------
//
//  $Id: MummyCsharpExportLayerGenerator.cxx 2 2007-12-17 18:15:56Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2007-12-17 13:15:56 -0500 (Mon, 17 Dec 2007) $
//  $Revision: 2 $
//
//  Copyright (C) 2006-2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MummyCsharpExportLayerGenerator.h"
#include "MummyCsharpGenerator.h"
#include "MummyLog.h"
#include "MummySettings.h"

#include "cableClass.h"
#include "cableClassType.h"
#include "cableConstructor.h"
#include "cableFunctionType.h"
#include "cableMethod.h"
#include "cablePointerType.h"
#include "cableReferenceType.h"
#include "cxxCvQualifiedType.h"
#include "cxxFunctionType.h"
#include "cxxPointerType.h"
#include "cxxType.h"

#include "gxsys/stl/set"
#include "gxsys/SystemTools.hxx"


//----------------------------------------------------------------------------
MummyCsharpExportLayerGenerator::MummyCsharpExportLayerGenerator()
{
  this->CsharpGenerator = 0;
}


//----------------------------------------------------------------------------
MummyCsharpExportLayerGenerator::~MummyCsharpExportLayerGenerator()
{
}


//----------------------------------------------------------------------------
bool MummyCsharpExportLayerGenerator::GenerateWrappers()
{
  this->EmitClassForExportLayer(*GetStream(), GetTargetClass());
  return false;
}


//----------------------------------------------------------------------------
MummyCsharpGenerator* MummyCsharpExportLayerGenerator::GetCsharpGenerator()
{
  return this->CsharpGenerator;
}


//----------------------------------------------------------------------------
void MummyCsharpExportLayerGenerator::SetCsharpGenerator(MummyCsharpGenerator* generator)
{
  this->CsharpGenerator = generator;
}


//----------------------------------------------------------------------------
//bool MummyCsharpExportLayerGenerator::FundamentalTypeIsWrappable(const cable::Type* t)
//{
//  return this->GetCsharpGenerator()->FundamentalTypeIsWrappable(t);
//}


//----------------------------------------------------------------------------
//bool MummyCsharpExportLayerGenerator::TypeIsWrappable(const cable::Type* t)
//{
//  return this->GetCsharpGenerator()->TypeIsWrappable(t);
//}


//----------------------------------------------------------------------------
//bool MummyCsharpExportLayerGenerator::FunctionTypeIsWrappable(const cable::FunctionType* ft)
//{
//  return this->GetCsharpGenerator()->FunctionTypeIsWrappable(ft);
//}


//----------------------------------------------------------------------------
//bool MummyCsharpExportLayerGenerator::MethodIsWrappable(const cable::Method* m, const cable::Context::Access& access)
//{
//  return this->GetCsharpGenerator()->MethodIsWrappable(m, access);
//}


//----------------------------------------------------------------------------
//bool MummyCsharpExportLayerGenerator::ClassIsWrappable(const cable::Class* c)
//{
//  return this->GetCsharpGenerator()->ClassIsWrappable(c);
//}


//----------------------------------------------------------------------------
const char *MummyCsharpExportLayerGenerator::GetArgName(cable::FunctionType *ftype, unsigned int i)
{
  return this->GetCsharpGenerator()->GetArgName(ftype, i);
}


//----------------------------------------------------------------------------
gxsys_stl::string MummyCsharpExportLayerGenerator::GetExportLayerFunctionName(const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname)
{
  return this->GetCsharpGenerator()->GetExportLayerFunctionName(c, m, mname);
}


//----------------------------------------------------------------------------
gxsys_stl::string GetExportLayerActualType(cable::Type *t, bool stripConst)
{
  gxsys_stl::string s(t->GetCxxType().GetName());

  if (stripConst)
    {
    s = t->GetCxxType().GetType()->GenerateName("", false, false);
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetExportLayerMappedType(cable::Type *t, bool stripConst)
{
  gxsys_stl::string s;

  if (HasMapToType(t))
    {
    // 't' should be either a class or a reference to a class...
    //
    cable::Class* c = 0;

    if (cable::Type::ClassTypeId == t->GetTypeId())
      {
      c = cable::ClassType::SafeDownCast(t)->GetClass();
      }
    else if (cable::Type::ReferenceTypeId == t->GetTypeId())
      {
      c = cable::ClassType::SafeDownCast(
        cable::ReferenceType::SafeDownCast(t)->GetTarget())->GetClass();
      }

    if (c)
      {
      s = ExtractMapToType(c);
      }

    // Abstract "string" type translates to "const char*" in the export layer:
    //
    if (s == "string")
      {
      if (stripConst)
        {
        s = "char*";
        }
      else
        {
        s = "const char*";
        }
      }
    else
      {
      s = "ERROR_unknown_mapped_to_type_or_null_class_type_in_the_export_layer";
      LogError(me_InternalError, << s.c_str());
      }
    }
  else
    {
    s = GetExportLayerActualType(t, stripConst);
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummyCsharpExportLayerGenerator::GetArgTypeAndNameString(cable::Type *argType, const char *name, bool stripConst)
{
  gxsys_stl::string s;

  if ((argType->GetTypeId() == cable::Type::PointerTypeId) &&
    (cable::PointerType::SafeDownCast(argType)->GetTarget()->GetTypeId() == cable::Type::FunctionTypeId)
    )
    {
    if (EquivalentTypedefNameExists(this->GetTargetClass(), cable::FunctionType::SafeDownCast(cable::PointerType::SafeDownCast(argType)->GetTarget()), s))
      {
      s = gxsys_stl::string(GetFullyQualifiedNameForCPlusPlus(this->GetTargetClass())) + "::" + s;
      }
    else
      {
      s = "ERROR - cannot generate argTypeAndName string for function pointer - add a typedef for the function pointer...";
      LogError(me_InternalError, << s.c_str());
      }
    }
  else
    {
    s = GetExportLayerMappedType(argType, stripConst);
    }

  if (name)
    {
    s += " ";
    s += name;
    }

  return s;
}


//----------------------------------------------------------------------------
void MummyCsharpExportLayerGenerator::EmitClassMethodDeclarationForExportLayer(gxsys_ios::ostream &os, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, bool emitExceptionParams)
{
  gxsys_stl::string cname(GetFullyQualifiedNameForCPlusPlus(c));
  gxsys_stl::string s;
  cable::FunctionType *ft = m->GetFunctionType();
  unsigned int cArgs = ft->GetNumberOfArguments();
  unsigned int cArgsEmitted = 0;
  unsigned int i = 0;
  cable::Type *retType = ft->GetReturns();
  cable::Type *argType = 0;
  cable::Class* cByRefClass = 0;

  // Extern "C":
  Emit(os, "extern \"C\" ");

  // Export declspec:
  Emit(os, "MUMMY_DLL_EXPORT\n");

  // Special case "new" and "delete" -- the rest are strictly according to the
  // method definition as given by 'm'...
  //
  if (mname == "new")
    {
    // Return type:
    Emit(os, cname.c_str());
    Emit(os, "* ");

    // Exported function name:
    Emit(os, GetExportLayerFunctionName(c, m, mname).c_str());

    Emit(os, "(unsigned int* mteStatus, unsigned int* mteIndex, unsigned int* rawRefCount");
    cArgsEmitted += 3;

    if (emitExceptionParams)
      {
      Emit(os, ", unsigned int* mteExceptionIndex, void** clonedException");
      cArgsEmitted += 2;
      }

    Emit(os, ")\n");
    }
  else if (mname == "delete")
    {
    // Return type:
    Emit(os, "void");
    Emit(os, " ");

    // Exported function name:
    Emit(os, GetExportLayerFunctionName(c, m, mname).c_str());

    Emit(os, "(");
    Emit(os, cname.c_str());
    Emit(os, "* pThis");
    cArgsEmitted += 1;

    if (emitExceptionParams)
      {
      Emit(os, ", unsigned int* mteExceptionIndex, void** clonedException");
      cArgsEmitted += 2;
      }

    Emit(os, ")\n");
    }
  else
    {
    // Return type:
    Emit(os, GetArgTypeAndNameString(retType, 0, false).c_str());
    Emit(os, " ");

    // Exported function name:
    Emit(os, GetExportLayerFunctionName(c, m, mname).c_str());

    // Open args:
    Emit(os, "(");

    // The "this" arg:
    if (!m->GetStatic())
      {
      Emit(os, cname.c_str());
      Emit(os, "* pThis");
      cArgsEmitted += 1;

      if (cArgs!=0)
        {
        Emit(os, ", ");
        }
      }

    // The real args:
    for (i= 0; i<cArgs; ++i)
      {
      argType = ft->GetArgument(i);

      // Utility classes get marshalled as copies across the boundary from C#
      // even if in C++ they are & (byref) arguments... So they need to be
      // declared as just plain old stack-based by value arguments here.
      //
      cByRefClass = 0;

      if (cable::Type::ReferenceTypeId == argType->GetTypeId())
        {
        cable::ClassType* classType = cable::ClassType::SafeDownCast(
          cable::ReferenceType::SafeDownCast(argType)->GetTarget());
        if (classType)
          {
          cByRefClass = classType->GetClass();
          }
        }

      if (cByRefClass && IsUtilityClass(cByRefClass))
        {
        Emit(os, GetFullyQualifiedNameForCPlusPlus(cByRefClass).c_str());
        Emit(os, " ");
        Emit(os, GetArgName(ft, i));
        }
      else
        {
        Emit(os, GetArgTypeAndNameString(argType, GetArgName(ft, i), false).c_str());
        }

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

      Emit(os, "unsigned int* mteStatus, unsigned int* mteIndex, unsigned int* rawRefCount");
      cArgsEmitted += 3;
      }

    if (emitExceptionParams)
      {
      if (cArgsEmitted)
        {
        Emit(os, ", ");
        }

      Emit(os, "unsigned int* mteExceptionIndex, void** clonedException");
      cArgsEmitted += 2;
      }

    // Close args:
    Emit(os, ")\n");
    }
}


//----------------------------------------------------------------------------
void MummyCsharpExportLayerGenerator::EmitSpecialHandlingForObjectPointerReturns(gxsys_ios::ostream &os, const gxsys_stl::string& cname, const cable::Method *, const gxsys_stl::string&, const unsigned int indent)
{
  ClassWrappingSettings cws;
  if (!this->GetSettings()->FindClassWrappingSettings(cname.c_str(), &cws))
    {
    LogError(me_NoClassWrappingSettings,
      << "error: no ClassWrappingSettings for class " << cname.c_str());
    }

  EmitIndent(os, indent);
  Emit(os, "if (0 != rv)\n");
  EmitIndent(os, indent+1);
  Emit(os, "{\n");

  gxsys_stl::string registerMethod = cws.registerMethod;
  gxsys_stl::string registerBaseClass = cws.registerBaseClass;
  gxsys_stl::string getRefCountMethod = cws.getRefCountMethod;
  gxsys_stl::string getMummyTypeEntryMethod = cws.getMummyTypeEntryMethod;
  gxsys_stl::string setMummyTypeEntryMethod = cws.setMummyTypeEntryMethod;

  if (!registerMethod.empty())
    {
    EmitIndent(os, indent+1);
    Emit(os, registerBaseClass.c_str());
    Emit(os, "* ro = (");
    Emit(os, registerBaseClass.c_str());
    Emit(os, "*) (void*) rv;\n");
    }


  if (!registerMethod.empty() && !getMummyTypeEntryMethod.empty())
    {
    // If object supports MummyTypeEntry caching:
    //
    EmitIndent(os, indent+1);
    Emit(os, "Kitware::mummy::TypeEntry* entry = ro->");
    Emit(os, getMummyTypeEntryMethod.c_str());
    Emit(os, "();\n");
    EmitIndent(os, indent+1);
    Emit(os, "if (!entry)\n");
    EmitIndent(os, indent+2);
    Emit(os, "{\n");
    EmitIndent(os, indent+2);
    Emit(os, "entry = Kitware::mummy::Runtime::GetTypeEntry((void*) rv, typeid(*ro).name());\n");
    EmitIndent(os, indent+2);
    Emit(os, "ro->");
    Emit(os, setMummyTypeEntryMethod.c_str());
    Emit(os, "(entry);\n");
    EmitIndent(os, indent+2);
    Emit(os, "*mteStatus = 1;\n");
    EmitIndent(os, indent+2);
    Emit(os, "}\n");
    EmitIndent(os, indent+1);
    Emit(os, "else\n");
    EmitIndent(os, indent+2);
    Emit(os, "{\n");
    EmitIndent(os, indent+2);
    Emit(os, "*mteStatus = 2;\n");
    EmitIndent(os, indent+2);
    Emit(os, "}\n");
    }
  else
    {
    // ...otherwise always call Runtime::GetTypeEntry:
    //
    EmitIndent(os, indent+1);
    Emit(os, "Kitware::mummy::TypeEntry* entry = Kitware::mummy::Runtime::GetTypeEntry(\n");

    EmitIndent(os, indent+2);
    Emit(os, "(void*) rv, typeid(*");
    if (!registerMethod.empty())
      {
      // Prefer "ro" (if we have it) to "rv" simply because
      // typeid(*obj).name() is really really stupid.
      //
      Emit(os, "ro");
      }
    else
      {
      Emit(os, "rv");
      }
    Emit(os, ").name());\n");

    EmitIndent(os, indent+1);
    Emit(os, "*mteStatus = 0;\n");
    }

  // So here we always have an entry:
  //
  EmitIndent(os, indent+1);
  Emit(os, "*mteIndex = entry->GetIndex();\n");

  if (!registerMethod.empty() && !getRefCountMethod.empty())
    {
    EmitIndent(os, indent+1);
    Emit(os, "*rawRefCount = (unsigned int) ro->");
    Emit(os, getRefCountMethod.c_str());
    Emit(os, ";\n");
    }
  else
    {
    EmitIndent(os, indent+1);
    Emit(os, "*rawRefCount = (unsigned int) -86;\n");
    }

  EmitIndent(os, indent+1);
  Emit(os, "}\n");
}


//----------------------------------------------------------------------------
void MummyCsharpExportLayerGenerator::EmitClassMethodForExportLayer(gxsys_ios::ostream &os, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname)
{
  gxsys_stl::string cname(GetFullyQualifiedNameForCPlusPlus(c));
  cable::FunctionType *ft = m->GetFunctionType();
  unsigned int cArgs = ft->GetNumberOfArguments();
  unsigned int i = 0;
  unsigned int indent = 1;
  cable::Type *retType = ft->GetReturns();
  cable::PointerType::Pointer fakeRetType;
  bool voidReturn = false;
  bool mappedReturnType = false;
  bool emitExceptionBlock = false;
  gxsys_stl::string rvTypeStr;
  bool needsInit = false;
  gxsys_stl::string initExpression;

  ClassWrappingSettings cws;
  if (!this->GetSettings()->FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    LogError(me_NoClassWrappingSettings,
      << "error: no ClassWrappingSettings for class " << GetFullyQualifiedNameForCPlusPlus(c).c_str());
    }

  emitExceptionBlock = !cws.exceptionBaseClass.empty();

  // If mname is "new" fake retType:
  //
  if (mname == "new")
    {
    fakeRetType = cable::PointerType::New();
    fakeRetType->SetTarget(c->GetClassType());
    fakeRetType->CreateCxxType(this->GetSourceRepresentation()->GetTypeSystem());
    retType = fakeRetType;
    }

  // Is there a return value? Do we need to map/transform it to a different type?
  //
  if (IsVoid(retType))
    {
    voidReturn = true;
    }
  else if (HasMapToType(retType))
    {
    mappedReturnType = true;
    }

  // Declaration:
  EmitClassMethodDeclarationForExportLayer(os, c, m, mname, emitExceptionBlock);

  // Open body:
  Emit(os, "{\n");

  // Declare return value "rv":
  if (!voidReturn)
    {
    if (cable::Type::ReferenceTypeId == retType->GetTypeId())
      {
      rvTypeStr = gxsys_stl::string("ref<") + GetArgTypeAndNameString(
        cable::ReferenceType::SafeDownCast(retType)->GetTarget(), 0, false) + ">";
      needsInit = false;
      }
    else if (mappedReturnType)
      {
      rvTypeStr = GetArgTypeAndNameString(retType, 0, true);
      needsInit = true;

      initExpression = "cast";
      // david.cole::fix -- this works for the only mapped type we currently
      // support ("string" / "const char*") -- it needs to be generalized
      // to generate correct code if we ever introduce more mapped types...
      }
    else
      {
      rvTypeStr = GetArgTypeAndNameString(retType, 0, true);
      needsInit = true;

      initExpression = GetCPlusPlusZeroInitializerExpression(retType);
      }

    EmitIndent(os);
    Emit(os, rvTypeStr.c_str());
    Emit(os, " rv");

    if (needsInit)
      {
      if (initExpression == "memset")
        {
        Emit(os, ";\n");
        EmitIndent(os);
        Emit(os, "memset(&rv, 0, sizeof(rv))");
        }
      else if (initExpression == "cast")
        {
        Emit(os, " = (");
        Emit(os, rvTypeStr.c_str());
        Emit(os, ") 0");
        }
      else
        {
        Emit(os, " = ");
        Emit(os, initExpression.c_str());
        }
      }

    Emit(os, ";");
    Emit(os, "\n");
    Emit(os, "\n");
    }

  // Open exception try block:
  if (emitExceptionBlock)
    {
    EmitIndent(os);
    Emit(os, "try\n");
    EmitIndent(os);
    Emit(os, "{\n");

    indent++;
    }

  // Delegate the call to the real underlying object:
  EmitIndent(os, indent);

  if (!voidReturn)
    {
    if (mappedReturnType)
      {
      Emit(os, GetExportLayerActualType(retType, true).c_str());
      Emit(os, " rvmi = ");
      }
    else
      {
      Emit(os, "rv = ");
      }
    }

  // Special case "new" and "delete" -- the rest are strictly according to the
  // method definition as given by 'm'...
  //
  if (mname == "new")
    {
    Emit(os, "new ");
    Emit(os, cname.c_str());
    Emit(os, ";");
    Emit(os, "\n");
    }
  else if (mname == "delete")
    {
    Emit(os, "delete pThis;");
    Emit(os, "\n");
    }
  else
    {
    if (!m->GetStatic())
      {
      Emit(os, "pThis->");
      }
    else
      {
      Emit(os, cname.c_str());
      Emit(os, "::");
      }

    Emit(os, mname.c_str());
    Emit(os, "(");
    for (i= 0; i<cArgs; ++i)
      {
      Emit(os, GetArgName(ft, i));

      if (i<cArgs-1)
        {
        Emit(os, ", ");
        }
      }
    Emit(os, ");");
    Emit(os, "\n");
    }


  // Map return type from "rvmi" to "rv":
  //
  if (mappedReturnType)
    {
    gxsys_stl::string map_to_type = GetMapToType(retType);
    gxsys_stl::string mapping_method = GetStringMethod(retType);

    Emit(os, "\n");

    EmitIndent(os, indent);
    Emit(os, GetExportLayerMappedType(retType, false).c_str());
    Emit(os, " rvm = rvmi.");
    Emit(os, mapping_method.c_str());
    Emit(os, "();\n");

    if (map_to_type == "string")
      {
      EmitIndent(os, indent);
      Emit(os, "size_t n = 0;\n");
      Emit(os, "\n");
      EmitIndent(os, indent);
      Emit(os, "if (rvm)\n");
      EmitIndent(os, indent+1);
      Emit(os, "{\n");
      EmitIndent(os, indent+1);
      Emit(os, "n = strlen(rvm);\n");
      EmitIndent(os, indent+1);
      Emit(os, "}\n");
      Emit(os, "\n");
      EmitIndent(os, indent);
      Emit(os, "rv = (char*) MUMMY_STRING_ALLOC(n+1);\n");
      EmitIndent(os, indent);
      Emit(os, "if (rv)\n");
      EmitIndent(os, indent+1);
      Emit(os, "{\n");
      EmitIndent(os, indent+1);
      Emit(os, "strncpy(rv, rvm, n);\n");
      EmitIndent(os, indent+1);
      Emit(os, "rv[n] = 0;\n");
      EmitIndent(os, indent+1);
      Emit(os, "}\n");
      Emit(os, "\n");
      }
    else
      {
      LogFileLineErrorMsg(m->GetFile(), m->GetLine(), me_UnknownMapToType,
        << "Unknown iwhMapToType for return type of method '" << m->GetName() << "'");
      }
    }
  else if (IsObjectPointer(retType))
    {
    const cable::Class* cRetType = cable::ClassType::SafeDownCast(
      cable::PointerType::SafeDownCast(retType)->GetTarget()
      )->GetClass();

    this->EmitSpecialHandlingForObjectPointerReturns(os, GetFullyQualifiedNameForCPlusPlus(cRetType), m, mname, indent);
    }

  // Close exception try block:
  if (emitExceptionBlock)
    {
    EmitIndent(os);
    Emit(os, "}\n");
    EmitIndent(os);
    Emit(os, "catch(");
    Emit(os, cws.exceptionBaseClass.c_str());
    Emit(os, "& mel_exc)\n");
    EmitIndent(os);
    Emit(os, "{\n");

    EmitIndent(os, 2);
    Emit(os, cws.exceptionBaseClass.c_str());
    Emit(os, "* mel_exc_clone = mel_exc.");
    Emit(os, cws.exceptionCloneMethod.c_str());
    Emit(os, "();\n");

    EmitIndent(os, 2);
    Emit(os, "Kitware::mummy::TypeEntry* entry = Kitware::mummy::Runtime::GetTypeEntry(\n");

    EmitIndent(os, 3);
    Emit(os, "(void*) mel_exc_clone, typeid(*mel_exc_clone).name());\n");

    EmitIndent(os, 2);
    Emit(os, "*clonedException = mel_exc_clone;\n");

    EmitIndent(os, 2);
    Emit(os, "*mteExceptionIndex = entry->GetIndex();\n");

    EmitIndent(os);
    Emit(os, "}\n");
    }

  // Return statement:
  if (!voidReturn)
    {
    Emit(os, "\n");
    EmitIndent(os);
    Emit(os, "return rv;\n");
    }

  // Close body:
  Emit(os, "}\n");
}


//----------------------------------------------------------------------------
void MummyCsharpExportLayerGenerator::EmitClassForExportLayer(gxsys_ios::ostream &os, const cable::Class *c)
{
  gxsys_stl::string header(c->GetFile());

  ClassWrappingSettings cws;
  if (!this->GetSettings()->FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    LogError(me_NoClassWrappingSettings,
      << "error: no ClassWrappingSettings for class " << GetFullyQualifiedNameForCPlusPlus(c).c_str());
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

    LogVerboseInfo(<< "Skipping code generation because class maps directly to native type.");

    return;
    }

  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");
  Emit(os, "// You can use this symbol in your header files if there are bits of your\n");
  Emit(os, "// code that should be conditionally compiled in this export layer.\n");
  Emit(os, "//\n");
  Emit(os, "#define MUMMY_EXPORT_LAYER_CXX\n");
  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");
  Emit(os, "// Do not emit the silly C4996 warning for strncpy use:\n");
  Emit(os, "//\n");
  Emit(os, "#ifndef _CRT_SECURE_NO_DEPRECATE\n");
  Emit(os, "#define _CRT_SECURE_NO_DEPRECATE\n");
  Emit(os, "#endif\n");
  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");
  Emit(os, "// Forward declare functions that generated code might call...\n");
  Emit(os, "//\n");
  Emit(os, "#ifdef _WIN64\n");
  Emit(os, "extern \"C\" __declspec(dllimport) void* __stdcall CoTaskMemAlloc(unsigned __int64 cb);\n");
  Emit(os, "#define MUMMY_STRING_ALLOC ::CoTaskMemAlloc\n");
  Emit(os, "#else\n");
  Emit(os, "#ifdef _WIN32\n");
  Emit(os, "extern \"C\" __declspec(dllimport) void* __stdcall CoTaskMemAlloc(unsigned long cb);\n");
  Emit(os, "#define MUMMY_STRING_ALLOC ::CoTaskMemAlloc\n");
  Emit(os, "#endif\n");
  Emit(os, "#endif\n");
  Emit(os, "\n");
  Emit(os, "#ifndef MUMMY_STRING_ALLOC\n");
  Emit(os, "#define MUMMY_STRING_ALLOC malloc\n");
  Emit(os, "#endif\n");
  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");
  Emit(os, "// Macro for exporting functions implemented in this file...\n");
  Emit(os, "//\n");
  Emit(os, "#ifdef _WIN32\n");
  Emit(os, "#define MUMMY_DLL_EXPORT __declspec(dllexport)\n");
  Emit(os, "#else\n");
  Emit(os, "#define MUMMY_DLL_EXPORT\n");
  Emit(os, "#endif\n");
  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");
  Emit(os, "#include \"");
  Emit(os, header.c_str());
  Emit(os, "\"\n");
  Emit(os, "\n");


  if (IsUtilityClass(c))
    {
    // No export layer necessary...
    }
  else
    {
    // Gather wrapped elements:
    //
    gxsys_stl::vector<cable::Method*> wrapped_methods;
    cable::Method *factoryM = 0;
    cable::Method *disposalM = 0;
    cable::Method *registerM = 0;
    cable::Method *unRegisterM = 0;

    this->GetCsharpGenerator()->GatherWrappedMethods(c, wrapped_methods, factoryM, disposalM, registerM, unRegisterM, false);

    const cable::Constructor* ctor = FindNonAbstractPublicDefaultConstructor(c);

    bool hasMethodsThatReturnRefs = false;
    for(gxsys_stl::vector<cable::Method*>::iterator mit = wrapped_methods.begin();
      !hasMethodsThatReturnRefs && mit != wrapped_methods.end(); ++mit)
      {
      cable::FunctionType *ft = (*mit)->GetFunctionType();
      cable::Type *retType = ft->GetReturns();
      if (cable::Type::ReferenceTypeId == retType->GetTypeId())
        {
        hasMethodsThatReturnRefs = true;
        }
      }

    if (hasMethodsThatReturnRefs)
      {
      Emit(os, "//----------------------------------------------------------------------------\n");
      Emit(os, "// Thanks to Brad King for the magic of the ref converter helper class:\n");
      Emit(os, "template <typename T>\n");
      Emit(os, "class ref\n");
      Emit(os, "{\n");
      Emit(os, "public:\n");
      EmitIndent(os);
      Emit(os, "ref() throw(): p_(0) {}\n");
      EmitIndent(os);
      Emit(os, "ref<T>& operator=(T& r) throw() { this->p_ = &r; return *this; }\n");
      EmitIndent(os);
      Emit(os, "operator T&() throw() { return *this->p_; }\n");
      Emit(os, "\n");
      Emit(os, "private:\n");
      EmitIndent(os);
      Emit(os, "T* p_;\n");
      Emit(os, "};\n");
      }


    // Emit include statements for any ref counted base classes involved in method return
    // values. Collect them into a set and then emit the set; has the nice effect of
    // eliminating duplicates and emitting the include statements in sorted order...
    //
    gxsys_stl::set<gxsys_stl::string> includeStatements;

    gxsys_stl::string registerInclude;
    if (ctor)
      {
      registerInclude = this->GetSettings()->GetRegisterInclude(c);
      if (!registerInclude.empty())
        {
        includeStatements.insert(registerInclude);
        }
      }

    for(gxsys_stl::vector<cable::Method*>::iterator mit = wrapped_methods.begin();
      mit != wrapped_methods.end(); ++mit)
      {
      cable::FunctionType *ft = (*mit)->GetFunctionType();
      cable::Type *retType = ft->GetReturns();
      if (!HasMapToType(retType) && IsObjectPointer(retType))
        {
        const cable::Class* cRetType = cable::ClassType::SafeDownCast(
          cable::PointerType::SafeDownCast(retType)->GetTarget()
          )->GetClass();
        registerInclude = this->GetSettings()->GetRegisterInclude(cRetType);
        if (!registerInclude.empty())
          {
          includeStatements.insert(registerInclude);
          }
        }
      }

    includeStatements.insert("#include \"MummyRuntime.h\"");
    includeStatements.insert("#include \"string.h\" // for possible memset use");

    if (!cws.exceptionInclude.empty())
      {
      includeStatements.insert(cws.exceptionInclude);
      }

    if (!includeStatements.empty())
      {
      Emit(os, "\n");
      Emit(os, "//----------------------------------------------------------------------------\n");

      for(gxsys_stl::set<gxsys_stl::string>::iterator sit = includeStatements.begin();
        sit != includeStatements.end(); ++sit)
        {
        Emit(os, sit->c_str());
        Emit(os, "\n");
        }
      }


    // Special methods first, then iterate the remainder in wrapped_methods:
    //
    if (factoryM)
      {
      Emit(os, "\n");
      Emit(os, "\n");
      Emit(os, "//----------------------------------------------------------------------------\n");
      EmitClassMethodForExportLayer(os, c, factoryM, factoryM->GetName());
      }

    if (disposalM)
      {
      Emit(os, "\n");
      Emit(os, "\n");
      Emit(os, "//----------------------------------------------------------------------------\n");
      EmitClassMethodForExportLayer(os, c, disposalM, disposalM->GetName());
      }

    if (registerM)
      {
      Emit(os, "\n");
      Emit(os, "\n");
      Emit(os, "//----------------------------------------------------------------------------\n");
      EmitClassMethodForExportLayer(os, c, registerM, registerM->GetName());
      }

    if (unRegisterM)
      {
      Emit(os, "\n");
      Emit(os, "\n");
      Emit(os, "//----------------------------------------------------------------------------\n");
      EmitClassMethodForExportLayer(os, c, unRegisterM, unRegisterM->GetName());
      }

    // If there's a public default constructor for concrete class 'c', emit "_new"
    // and "_delete" export layer functions:
    //
    if (ctor)
      {
      Emit(os, "\n");
      Emit(os, "\n");
      Emit(os, "//----------------------------------------------------------------------------\n");
      EmitClassMethodForExportLayer(os, c, ctor, "new");

      Emit(os, "\n");
      Emit(os, "\n");
      Emit(os, "//----------------------------------------------------------------------------\n");
      EmitClassMethodForExportLayer(os, c, ctor, "delete");
      }

    for(gxsys_stl::vector<cable::Method*>::iterator mit = wrapped_methods.begin();
      mit != wrapped_methods.end(); ++mit)
      {
      Emit(os, "\n");
      Emit(os, "\n");
      Emit(os, "//----------------------------------------------------------------------------\n");
      EmitClassMethodForExportLayer(os, c, *mit, (*mit)->GetName());
      }
    }

  // If there is extraExportLayerCode, emit it at the bottom of the file.
  // If it's there, it's the name of a file that we are to include in
  // its entirety...
  //
  gxsys_stl::string extraCode = this->GetSettings()->GetExtraExportLayerCode(c);
  if (extraCode != "")
    {
    Emit(os, "\n");
    Emit(os, "\n");
    Emit(os, "//----------------------------------------------------------------------------\n");
    Emit(os, "// Begin extraExportLayerCode\n");
    Emit(os, "\n");
    EmitFile(os, extraCode.c_str());
    Emit(os, "\n");
    Emit(os, "// End extraExportLayerCode\n");
    }
}
