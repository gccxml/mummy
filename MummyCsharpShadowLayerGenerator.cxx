//----------------------------------------------------------------------------
//
//  $Id: MummyCsharpShadowLayerGenerator.cxx 442 2009-04-03 19:47:41Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2009-04-03 15:47:41 -0400 (Fri, 03 Apr 2009) $
//  $Revision: 442 $
//
//  Copyright (C) 2006-2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MummyCsharpShadowLayerGenerator.h"
#include "MummyCsharpGenerator.h"
#include "MummyLog.h"
#include "MummySettings.h"

#include "cableClass.h"
#include "cableClassType.h"
#include "cableFundamentalType.h"
#include "cableFunctionType.h"
#include "cableMethod.h"
#include "cablePointerType.h"
#include "cableType.h"

#include "gxsys/ios/sstream"


//----------------------------------------------------------------------------
MummyCsharpShadowLayerGenerator::MummyCsharpShadowLayerGenerator()
{
  this->CsharpGenerator = 0;
}


//----------------------------------------------------------------------------
MummyCsharpShadowLayerGenerator::~MummyCsharpShadowLayerGenerator()
{
}


//----------------------------------------------------------------------------
bool MummyCsharpShadowLayerGenerator::GenerateWrappers()
{
  this->EmitClassForShadowLayer(*GetStream(), GetTargetClass());
  return false;
}


//----------------------------------------------------------------------------
MummyCsharpGenerator* MummyCsharpShadowLayerGenerator::GetCsharpGenerator()
{
  return this->CsharpGenerator;
}


//----------------------------------------------------------------------------
void MummyCsharpShadowLayerGenerator::SetCsharpGenerator(MummyCsharpGenerator* generator)
{
  this->CsharpGenerator = generator;
}


//----------------------------------------------------------------------------
const char *MummyCsharpShadowLayerGenerator::GetArgName(cable::FunctionType *ftype, unsigned int i)
{
  return this->GetCsharpGenerator()->GetArgName(ftype, i);
}


//----------------------------------------------------------------------------
gxsys_stl::string GetShadowClassName(const cable::Class *c)
{
  gxsys_stl::string s(c->GetName());
  s += "Shadow";
  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string GetPrimaryDelegateCxxType(const cable::Class *c)
{
  return GetShadowClassName(c) + "_PrimaryType";
}


//----------------------------------------------------------------------------
gxsys_stl::string GetSecondaryDelegateCxxType(const cable::Class *c)
{
  return gxsys_stl::string(c->GetName()) + "*";
}


//----------------------------------------------------------------------------
void MummyCsharpShadowLayerGenerator::EmitClassMethodDeclarationForShadowLayer(gxsys_ios::ostream &os, const cable::Class *, const cable::Method *m, const unsigned int, int indent, const char *implClassName)
{
  cable::FunctionType *ft = m->GetFunctionType();
  unsigned int cArgs = ft->GetNumberOfArguments();
  unsigned int i = 0;
  cable::Type *argType = 0;
  cable::Type *retType = ft->GetReturns();

  EmitIndent(os, indent);

  // Context:
//  Emit(os, "/* ");
//  Emit(os, m->GetContext()->GetName());
//  Emit(os, " */ ");

  // Abstract?
//  if (m->GetPureVirtual())
//    {
//    Emit(os, "/* pure virtual */ ");
//    }

  // Definitely virtual:
  if (m->GetVirtual() && !implClassName)
    {
    Emit(os, "virtual ");
    }

  // Return type:
  Emit(os, retType->GetCxxType().GetName().c_str());
  Emit(os, " ");

  // Class name colon colon:
  if (implClassName)
    {
    Emit(os, implClassName);
    Emit(os, "::");
    }

  // Method name:
  Emit(os, m->GetName());

  // Open args:
  Emit(os, "(");

  // The real args:
  for (i= 0; i<cArgs; ++i)
    {
    argType = ft->GetArgument(i);

    // arg "i":
    Emit(os, argType->GetCxxType().GetName().c_str());
    Emit(os, " ");
    Emit(os, GetArgName(ft, i));

    if (i<cArgs-1)
      {
      Emit(os, ", ");
      }
    }

  // Close args:
  Emit(os, ")");
}


//----------------------------------------------------------------------------
void MummyCsharpShadowLayerGenerator::EmitClassDeclarationForShadowLayer(gxsys_ios::ostream &os, const cable::Class *c, const gxsys_stl::vector<cable::Method*>& wrapped_methods, cable::Method*, cable::Method*, cable::Method*, cable::Method*)
{
  unsigned int i = 0;
  gxsys_stl::string s(GetShadowClassName(c));

  if (ClassIsA(c, "vtkObject"))
    {
    Emit(os, "\n");
    Emit(os, "\n");
    Emit(os, "//----------------------------------------------------------------------------\n");
    Emit(os, "#include \"vtkDebugLeaks.h\"\n");
    }

  Emit(os, "\n");
  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");

  Emit(os, "#undef ");
  Emit(os, s.c_str());
  Emit(os, "_USE_IDISPATCH_AS_PRIMARY\n");
  Emit(os, "\n");
  Emit(os, "#ifdef _MSC_VER\n");
  Emit(os, "#define ");
  Emit(os, s.c_str());
  Emit(os, "_USE_IDISPATCH_AS_PRIMARY\n");
  Emit(os, "#endif\n");
  Emit(os, "\n");
  Emit(os, "#ifdef ");
  Emit(os, s.c_str());
  Emit(os, "_USE_IDISPATCH_AS_PRIMARY\n");
  Emit(os, "#include \"windows.h\" // HRESULT, FAILED, E_UNEXPECTED\n");
  Emit(os, "#define ");
  Emit(os, s.c_str());
  Emit(os, "_PrimaryType IDispatch*\n");
  Emit(os, "#else\n");
  Emit(os, "#define ");
  Emit(os, s.c_str());
  Emit(os, "_PrimaryType void*\n");
  Emit(os, "#define HRESULT long\n");
  Emit(os, "#define FAILED(hr) ((HRESULT)(hr) < 0)\n");
  Emit(os, "#define E_UNEXPECTED ((HRESULT)0x8000FFFFL)\n");
  Emit(os, "#endif\n");


  Emit(os, "\n");
  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");

  Emit(os, "class ");
  Emit(os, s.c_str());
  Emit(os, " : public ");
  Emit(os, c->GetQualifiedName().c_str());

  // Abstract?
  //if (c->GetAbstract())
  //  {
  //  Emit(os, " /* abstract */");
  //  }

  Emit(os, "\n");

  Emit(os, "{\n");
  Emit(os, "public:\n");

  // Shadow class always has factory method and DisconnectShadowDelegates:
  //
  // The factory method is "create-and-connect" so that it can be made in
  // one "over-the-boundary" call from the wrapped layer:
  //
  EmitIndent(os);
  Emit(os, "static ");
  Emit(os, s.c_str());
  Emit(os, "* CreateShadow(");
  Emit(os, GetPrimaryDelegateCxxType(c).c_str());
  Emit(os, " primary);");
  Emit(os, "\n");

  EmitIndent(os);
  Emit(os, "void DisconnectShadowDelegates();");
  Emit(os, "\n");

  Emit(os, "\n");

  // And overrides of all the virtual wrapped methods (including the Register
  // and UnRegister methods, if any...)
  //
  i = 0;
  for (gxsys_stl::vector<cable::Method*>::const_iterator mit = wrapped_methods.begin();
    mit != wrapped_methods.end(); ++mit)
    {
    if ((*mit)->GetVirtual())
      {
      EmitClassMethodDeclarationForShadowLayer(os, c, *mit, i+1, 1, false);
      Emit(os, ";\n");
      }
    ++i;
    }


  Emit(os, "\n");
  Emit(os, "protected:\n");
  EmitIndent(os);
  Emit(os, s.c_str());
  Emit(os, "();\n");
  EmitIndent(os);
  Emit(os, "virtual ~");
  Emit(os, s.c_str());
  Emit(os, "();\n");


  Emit(os, "\n");
  Emit(os, "private:\n");
  EmitIndent(os);
  Emit(os, GetPrimaryDelegateCxxType(c).c_str());
  Emit(os, " PrimaryDelegate;");
  Emit(os, "\n");
  EmitIndent(os);
  Emit(os, GetSecondaryDelegateCxxType(c).c_str());
  Emit(os, " SecondaryDelegate;");
  Emit(os, "\n");

  // A bool flag for each virtual override indicating whether the shadow
  // class is currently calling that method on the primary delegate or not:
  //
  i = 0;
  for (gxsys_stl::vector<cable::Method*>::const_iterator mit = wrapped_methods.begin();
    mit != wrapped_methods.end(); ++mit)
    {
    if ((*mit)->GetVirtual())
      {
      EmitIndent(os);
      Emit(os, "bool CallingPrimary_");
      Emit(os, (*mit)->GetName());
      Emit(os, "_");
      EmitUint(os, i+1);
      Emit(os, ";\n");
      }

    ++i;
    }

  Emit(os, "};\n");
}


//----------------------------------------------------------------------------
// The string returned from these Variant functions should be a valid V_ and
// VT_ suffix for use with initializing/accessing a VARIANT variable of the
// given type...
//
// For example, in the use case:
//   VariantInit(&vargs[0]);
//   V_VT(&vargs[0]) = VT_I4;
//   V_I4(&vargs[0]) = (long) callData;
// This function will receive a cable type representing the C++ type "long"
// and return "I4"...
//
gxsys_stl::string /* MummyCsharpShadowLayerGenerator:: */ GetVariantFundamentalTypeString(const cable::Type *t)
{
  gxsys_stl::string s;

  if (cable::Type::FundamentalTypeId == t->GetTypeId())
    {
    s = cable::FundamentalType::SafeDownCast(t)->GetTypeName();

    switch (cxx::FundamentalType::SafeDownCast(t->GetCxxType().GetType())->GetId())
      {
      case cxx::FundamentalType::UnsignedChar:
        s = "UI1";
      break;

      case cxx::FundamentalType::UnsignedShortInt:
        s = "UI2";
      break;

      case cxx::FundamentalType::UnsignedInt:
      case cxx::FundamentalType::UnsignedLongInt:
        s = "UI4";
      break;

      case cxx::FundamentalType::UnsignedLongLongInt:
        s = "UI8";
      break;

      case cxx::FundamentalType::SignedChar:
      case cxx::FundamentalType::Char:
        s = "I1";
      break;

      case cxx::FundamentalType::ShortInt:
        s = "I2";
      break;

      case cxx::FundamentalType::Int:
      case cxx::FundamentalType::LongInt:
        s = "I4";
      break;

      case cxx::FundamentalType::LongLongInt:
        s = "I8";
      break;

      case cxx::FundamentalType::Bool:
        s = "BOOL";
      break;

      case cxx::FundamentalType::Float:
        s = "R4";
      break;

      case cxx::FundamentalType::Double:
        s = "R8";
      break;

      //case cxx::FundamentalType::Void:
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
    LogError(me_InternalError,
      << "Unhandled variable type. GetVariantFundamentalTypeString returning the empty string...");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string /* MummyCsharpShadowLayerGenerator:: */ GetVariantTypeString(const cable::Type *t)
{
  gxsys_stl::string s;
  cable::Type *nested_type = 0;

  switch (t->GetTypeId())
    {
    case cable::Type::EnumerationTypeId:
      s = "UI4";
    break;

    case cable::Type::FundamentalTypeId:
      s = GetVariantFundamentalTypeString(t);
    break;

    case cable::Type::ArrayTypeId:
      s = "ERROR_ArrayTypeId_not_yet_implemented";
      LogError(me_InternalError, << s.c_str());
    break;

    case cable::Type::ClassTypeId:
      s = "ERROR_passing_class__";
      s += cable::ClassType::SafeDownCast(t)->GetClass()->GetName();
      s += "__by_value_not_allowed";
      LogError(me_InternalError, << s.c_str());
    break;

    case cable::Type::PointerTypeId:
      nested_type = cable::PointerType::SafeDownCast(t)->GetTarget();

      // IntPtr maps to I4 on 32-bit windows and I8 on 64-bit windows...
      //
      if (IsObject(nested_type))
        {
		  s = "PTR";
        }
      else if (IsChar(nested_type))
        {
        s = "PTR";
        }
      else if (IsVoid(nested_type))
        {
		  s = "PTR";
        }
      else if (cable::Type::FundamentalTypeId == nested_type->GetTypeId())
        {
        s = "PTR";
        }
      else if (cable::Type::PointerTypeId == nested_type->GetTypeId())
        {
		  s = "PTR";
        }
      else
        {
        s = "ERROR_PointerTypeId_not_yet_implemented_for_nested_type";
        LogError(me_InternalError, << s.c_str());
        }
    break;

    case cable::Type::ReferenceTypeId:
      s = "ERROR_ReferenceTypeId_not_yet_implemented";
      LogError(me_InternalError, << s.c_str());
    break;

    case cable::Type::OffsetTypeId:
    case cable::Type::MethodTypeId:
    case cable::Type::FunctionTypeId:
    default:
      s = "ERROR_No_Variant_type_for_cable_Type_TypeId";
      LogError(me_InternalError, << s.c_str());
    break;
    }

  if (s == "")
    {
    LogError(me_InternalError,
      << "Unhandled variable type. GetVariantTypeString returning the empty string...");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string /* MummyCsharpShadowLayerGenerator:: */ GetVariantTypeCastingString(const cable::Type *t)
{
  gxsys_stl::string s;
  cable::Type *nested_type = 0;

  switch (t->GetTypeId())
    {
    case cable::Type::EnumerationTypeId:
      s = "(unsigned int) ";
    break;

    case cable::Type::FundamentalTypeId:
      // No cast needed...
      s = "";
    break;

    case cable::Type::ArrayTypeId:
      s = "ERROR_ArrayTypeId_not_yet_implemented";
      LogError(me_InternalError, << s.c_str());
    break;

    case cable::Type::ClassTypeId:
      s = "ERROR_passing_class__";
      s += cable::ClassType::SafeDownCast(t)->GetClass()->GetName();
      s += "__by_value_not_allowed";
      LogError(me_InternalError, << s.c_str());
    break;

    case cable::Type::PointerTypeId:
      nested_type = cable::PointerType::SafeDownCast(t)->GetTarget();

      // IntPtr maps to I4 on 32-bit windows and I8 on 64-bit windows...
      //
      if (IsObject(nested_type))
        {
        s = "(uintptr_t) ";
        }
      else if (IsChar(nested_type))
        {
        s = "(uintptr_t) ";
        }
      else if (IsVoid(nested_type))
        {
        s = "(uintptr_t) ";
        }
      else if (cable::Type::FundamentalTypeId == nested_type->GetTypeId())
        {
        s = "(uintptr_t) ";
        }
      else if (cable::Type::PointerTypeId == nested_type->GetTypeId())
        {
        s = "(uintptr_t) /* pointer */";
        }
      else
        {
        s = "ERROR_PointerTypeId_not_yet_implemented_for_nested_type";
        LogError(me_InternalError, << s.c_str());
        }
    break;

    case cable::Type::ReferenceTypeId:
      s = "ERROR_ReferenceTypeId_not_yet_implemented";
      LogError(me_InternalError, << s.c_str());
    break;

    case cable::Type::OffsetTypeId:
    case cable::Type::MethodTypeId:
    case cable::Type::FunctionTypeId:
    default:
      s = "ERROR_No_Variant_type_for_cable_Type_TypeId";
      LogError(me_InternalError, << s.c_str());
    break;
    }

  return s;
}


//----------------------------------------------------------------------------
void MummyCsharpShadowLayerGenerator::EmitClassImplementationForShadowLayer(gxsys_ios::ostream &os, const cable::Class *c, const gxsys_stl::vector<cable::Method*>& wrapped_methods, cable::Method* factoryM, cable::Method* disposalM, cable::Method* registerM, cable::Method* unRegisterM)
{
  unsigned int i = 0;
  gxsys_stl::string s(GetShadowClassName(c));
  gxsys_ios::ostringstream oss;
  gxsys_stl::string vargs;

  // Shadow class always has factory method and DisconnectShadowDelegates:
  //
  Emit(os, "\n");
  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");
  Emit(os, "/* static */\n");
  Emit(os, s.c_str());
  Emit(os, "* ");
  Emit(os, s.c_str());
  Emit(os, "::");
  Emit(os, "CreateShadow(");
  Emit(os, GetPrimaryDelegateCxxType(c).c_str());
  Emit(os, " primary)");
  Emit(os, "\n");
  Emit(os, "{\n");

  EmitIndent(os);
  Emit(os, s.c_str());
  Emit(os, "* rv = new ");
  Emit(os, s.c_str());
  Emit(os, ";\n");

  EmitIndent(os);
  Emit(os, "rv->PrimaryDelegate = primary;\n");

  EmitIndent(os);
  Emit(os, "rv->SecondaryDelegate = ");
  if (factoryM)
    {
    Emit(os, c->GetName());
    Emit(os, "::");
    Emit(os, factoryM->GetName());
    Emit(os, "()");
    }
  else
    {
    Emit(os, "0");
    }
  Emit(os, ";\n");
  Emit(os, "\n");

  EmitIndent(os);
  Emit(os, "if (rv->PrimaryDelegate)\n");
  EmitIndent(os, 2);
  Emit(os, "{\n");
  Emit(os, "#ifdef ");
  Emit(os, s.c_str());
  Emit(os, "_USE_IDISPATCH_AS_PRIMARY\n");
  EmitIndent(os, 2);
  Emit(os, "rv->PrimaryDelegate->AddRef();\n");
  Emit(os, "#else\n");
  Emit(os, "#endif\n");
  EmitIndent(os, 2);
  Emit(os, "}\n");
  Emit(os, "\n");

  EmitIndent(os);
  Emit(os, "return rv;\n");

  Emit(os, "}\n");

  Emit(os, "\n");
  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");
  Emit(os, "void ");
  Emit(os, s.c_str());
  Emit(os, "::");
  Emit(os, "DisconnectShadowDelegates()");
  Emit(os, "\n");
  Emit(os, "{\n");

  EmitIndent(os);
  Emit(os, "if (this->PrimaryDelegate)\n");
  EmitIndent(os, 2);
  Emit(os, "{\n");
  Emit(os, "#ifdef ");
  Emit(os, s.c_str());
  Emit(os, "_USE_IDISPATCH_AS_PRIMARY\n");
  EmitIndent(os, 2);
  Emit(os, "this->PrimaryDelegate->Release();\n");
  Emit(os, "#else\n");
  Emit(os, "#endif\n");
  EmitIndent(os, 2);
  Emit(os, "}\n");
  EmitIndent(os);
  Emit(os, "this->PrimaryDelegate = 0;\n");
  Emit(os, "\n");

  if (disposalM)
    {
    EmitIndent(os);
    Emit(os, "if (this->SecondaryDelegate)\n");
    EmitIndent(os, 2);
    Emit(os, "{\n");
    EmitIndent(os, 2);
    Emit(os, "this->SecondaryDelegate->");
    Emit(os, disposalM->GetName());
    Emit(os, "();\n");
    EmitIndent(os, 2);
    Emit(os, "}\n");
    }

  EmitIndent(os);
  Emit(os, "this->SecondaryDelegate = 0;\n");

  Emit(os, "\n");
  Emit(os, "}\n");

  // And overrides of all the virtual wrapped methods:
  //
  i = 0;

  for (gxsys_stl::vector<cable::Method*>::const_iterator mit = wrapped_methods.begin();
    mit != wrapped_methods.end(); ++mit)
    {
    if ((*mit)->GetVirtual())
      {
      unsigned int argi = 0;
      unsigned int cArgs = (*mit)->GetFunctionType()->GetNumberOfArguments();
      cable::Type *retType = (*mit)->GetFunctionType()->GetReturns();
      bool voidReturn = false;
      if (IsVoid(retType))
        {
        voidReturn = true;
        }

      Emit(os, "\n");
      Emit(os, "\n");
      Emit(os, "//----------------------------------------------------------------------------\n");
      EmitClassMethodDeclarationForShadowLayer(os, c, *mit, i+1, 0, s.c_str());
      Emit(os, "\n");
      Emit(os, "{\n");

      if (!voidReturn)
        {
        EmitIndent(os);
        Emit(os, retType->GetCxxType().GetName().c_str());
        Emit(os, " rv = ");
        if (cable::Type::PointerTypeId == retType->GetTypeId())
          {
          Emit(os, "0");
          }
        else if (cable::Type::FundamentalTypeId == retType->GetTypeId())
          {
          Emit(os, "(");
          Emit(os, retType->GetCxxType().GetName().c_str());
          Emit(os, ") 0");
          }
        else
          {
          Emit(os, retType->GetCxxType().GetName().c_str());
          Emit(os, "()");
          }
        Emit(os, ";\n");
        }

      EmitIndent(os);
      Emit(os, "HRESULT msl_hr = E_UNEXPECTED;\n");
      Emit(os, "\n");

      EmitIndent(os);
      Emit(os, "if (this->PrimaryDelegate && !this->CallingPrimary_");
      Emit(os, (*mit)->GetName());
      Emit(os, "_");
      EmitUint(os, i+1);
      Emit(os, ")\n");

      EmitIndent(os, 2);
      Emit(os, "{\n");

      EmitIndent(os, 2);
      Emit(os, "this->CallingPrimary_");
      Emit(os, (*mit)->GetName());
      Emit(os, "_");
      EmitUint(os, i+1);
      Emit(os, " = true;\n");
      Emit(os, "\n");

      Emit(os, "#ifdef ");
      Emit(os, s.c_str());
      Emit(os, "_USE_IDISPATCH_AS_PRIMARY\n");
      EmitIndent(os, 2);
      Emit(os, "IDispatch *msl_pDisp = (IDispatch *) this->PrimaryDelegate;\n");
      EmitIndent(os, 2);
      Emit(os, "msl_pDisp->AddRef();\n");
      Emit(os, "\n");
      EmitIndent(os, 2);
      Emit(os, "DISPID msl_dispid = (DISPID) -1;\n");

      EmitIndent(os, 2);
      Emit(os, "OLECHAR *msl_name = L\"");
      Emit(os, (*mit)->GetName());
      Emit(os, "\";\n");

      if (cArgs!=0)
        {
        EmitIndent(os, 2);
        Emit(os, "VARIANTARG msl_vargs[");
        oss.rdbuf()->str("");
        oss << cArgs << gxsys_ios::ends;
        vargs = oss.str();
        Emit(os, vargs.c_str());
        Emit(os, "];\n");
        }

      EmitIndent(os, 2);
      Emit(os, "DISPPARAMS msl_params;\n");
      EmitIndent(os, 2);
      Emit(os, "VARIANT msl_result;\n");
      EmitIndent(os, 2);
      Emit(os, "EXCEPINFO msl_excep;\n");
      EmitIndent(os, 2);
      Emit(os, "UINT msl_arg = (UINT) -1;\n");
      Emit(os, "\n");
      EmitIndent(os, 2);
      Emit(os, "msl_hr = msl_pDisp->GetIDsOfNames(\n");
      EmitIndent(os, 3);
      Emit(os, "IID_NULL,\n");
      EmitIndent(os, 3);
      Emit(os, "&msl_name,\n");
      EmitIndent(os, 3);
      Emit(os, "1,\n");
      EmitIndent(os, 3);
      Emit(os, "LOCALE_USER_DEFAULT,\n");
      EmitIndent(os, 3);
      Emit(os, "&msl_dispid);\n");
      Emit(os, "\n");

      EmitIndent(os, 2);
      Emit(os, "if (SUCCEEDED(msl_hr))\n");
      EmitIndent(os, 3);
      Emit(os, "{\n");

      for (argi= 0; argi<cArgs; ++argi)
        {
        oss.rdbuf()->str("");
        oss << "&msl_vargs[" << (cArgs-1-argi) << "]" << gxsys_ios::ends;
        vargs = oss.str();

        EmitIndent(os, 3);
        Emit(os, "VariantInit(");
        Emit(os, vargs.c_str());
        Emit(os, ");\n");

		cable::Type *paramType = (*mit)->GetFunctionType()->GetArgument(argi);
		std::string paramTypeName = GetVariantTypeString(paramType);

        EmitIndent(os, 3);
        Emit(os, "V_VT(");
        Emit(os, vargs.c_str());
        Emit(os, ") = VT_");
        Emit(os, paramTypeName.c_str());
        Emit(os, ";\n");

        EmitIndent(os, 3);
        Emit(os, "V_");
		if (paramTypeName == "PTR")
		{
			paramTypeName = "UINT_" + paramTypeName;
		}
		Emit(os, paramTypeName.c_str());
        Emit(os, "(");
        Emit(os, vargs.c_str());
        Emit(os, ") = ");
        Emit(os, GetVariantTypeCastingString(paramType).c_str());
        Emit(os, GetArgName((*mit)->GetFunctionType(), argi));
        Emit(os, ";\n");

        Emit(os, "\n");
        }

      EmitIndent(os, 3);
      Emit(os, "msl_params.rgvarg = ");
      if (0 == cArgs)
        {
        Emit(os, "0");
        }
      else
        {
        Emit(os, "&msl_vargs[0]");
        }
      Emit(os, ";\n");
      EmitIndent(os, 3);
      Emit(os, "msl_params.rgdispidNamedArgs = 0;\n");
      EmitIndent(os, 3);
      Emit(os, "msl_params.cArgs = ");
      if (0 == cArgs)
        {
        Emit(os, "0");
        }
      else
        {
        Emit(os, "sizeof(msl_vargs)/sizeof(msl_vargs[0])");
        }
      Emit(os, ";\n");
      EmitIndent(os, 3);
      Emit(os, "msl_params.cNamedArgs = 0;\n");
      Emit(os, "\n");
      EmitIndent(os, 3);
      Emit(os, "VariantInit(&msl_result);\n");
      Emit(os, "\n");
      EmitIndent(os, 3);
      Emit(os, "memset(&msl_excep, 0, sizeof(msl_excep));\n");
      Emit(os, "\n");
      EmitIndent(os, 3);
      Emit(os, "msl_hr = msl_pDisp->Invoke(msl_dispid, IID_NULL, LOCALE_USER_DEFAULT,\n");
      EmitIndent(os, 4);
      Emit(os, "DISPATCH_METHOD, &msl_params, &msl_result, &msl_excep, &msl_arg);\n");
      Emit(os, "\n");

      if (!voidReturn)
        {
        EmitIndent(os, 3);
        Emit(os, "if (SUCCEEDED(msl_hr))\n");
        EmitIndent(os, 4);
        Emit(os, "{\n");
        EmitIndent(os, 4);
        Emit(os, "rv = (");
        Emit(os, retType->GetCxxType().GetName().c_str());
        Emit(os, ") V_");
		std::string returnTypeName = GetVariantTypeString(retType);
		if (returnTypeName == "PTR")
		{
			returnTypeName = "UINT_" + returnTypeName;
		}
        Emit(os, returnTypeName.c_str());
        Emit(os, "(&msl_result);\n");
        EmitIndent(os, 4);
        Emit(os, "}\n");
        Emit(os, "\n");
        }

      EmitIndent(os, 3);
      Emit(os, "VariantClear(&msl_result);\n");

      for (argi= 0; argi<cArgs; ++argi)
        {
        oss.rdbuf()->str("");
        oss << "&msl_vargs[" << argi << "]" << gxsys_ios::ends;
        vargs = oss.str();

        EmitIndent(os, 3);
        Emit(os, "VariantClear(");
        Emit(os, vargs.c_str());
        Emit(os, ");\n");
        }

      EmitIndent(os, 3);
      Emit(os, "}\n");
      Emit(os, "\n");

      EmitIndent(os, 2);
      Emit(os, "msl_pDisp->Release();\n");
      Emit(os, "#else\n");
      Emit(os, "#endif\n");
      Emit(os, "\n");

      EmitIndent(os, 2);
      Emit(os, "this->CallingPrimary_");
      Emit(os, (*mit)->GetName());
      Emit(os, "_");
      EmitUint(os, i+1);
      Emit(os, " = false;\n");

      EmitIndent(os, 2);
      Emit(os, "}\n");
      Emit(os, "\n");


      EmitIndent(os);
      Emit(os, "if (FAILED(msl_hr) || this->CallingPrimary_");
      Emit(os, (*mit)->GetName());
      Emit(os, "_");
      EmitUint(os, i+1);
      Emit(os, ")\n");

      EmitIndent(os, 2);
      Emit(os, "{\n");

      // Do NOT delegate Register or UnRegister calls to the
      // SecondaryDelegate... Only pass them up to the parent
      // class implementation on *this* object. The SecondaryDelegate
      // is only held by us for delegating non-ref-counting calls
      // to...
      //
      int secondaryIndent = 3;

      if (*mit != registerM && *mit != unRegisterM)
        {
        secondaryIndent = 3;

        EmitIndent(os, 2);
        Emit(os, "if (this->SecondaryDelegate)\n");

        EmitIndent(os, 3);
        Emit(os, "{\n");

        EmitIndent(os, 3);
        if (!voidReturn)
          {
          Emit(os, "rv = ");
          }
        Emit(os, "this->SecondaryDelegate->");
        Emit(os, (*mit)->GetName());
        Emit(os, "(");

        for (argi= 0; argi<cArgs; ++argi)
          {
          Emit(os, GetArgName((*mit)->GetFunctionType(), argi));

          if (argi<cArgs-1)
            {
            Emit(os, ", ");
            }
          }
        Emit(os, ");\n");

        EmitIndent(os, 3);
        Emit(os, "}\n");

        EmitIndent(os, 2);
        Emit(os, "else\n");

        EmitIndent(os, 3);
        Emit(os, "{\n");
        }
      else
        {
        secondaryIndent = 2;
        }


      // Only call the base class if it's not a pure virtual. If it's a pure
      // virtual, then the method we are in may be the only implementation.
      // If that's the case (as in vtkCommand::Execute) then it's a mistake
      // if the primary delegate has not already handled it.
      //
      if ((*mit)->GetPureVirtual())
        {
        EmitIndent(os, secondaryIndent);
        Emit(os, "// pure virtual method in parent class...\n");

        if (*mit == registerM || *mit == unRegisterM)
          {
          EmitIndent(os, secondaryIndent);
          Emit(os, "// Register and UnRegister methods should *not* be\n");
          EmitIndent(os, secondaryIndent);
          Emit(os, "// pure virtual methods in a shadowed class...\n");

          LogError(me_PureVirtualMethodNotAllowed,
            << "Register and UnRegister methods *cannot* be pure "
            << "virtual methods in a shadowed class..."
            );
          }
        }
      else
        {
        EmitIndent(os, secondaryIndent);
        if (!voidReturn)
          {
          Emit(os, "rv = ");
          }
        Emit(os, "this->");
        Emit(os, c->GetQualifiedName().c_str());
        Emit(os, "::");
        Emit(os, (*mit)->GetName());
        Emit(os, "(");

        for (argi= 0; argi<cArgs; ++argi)
          {
          Emit(os, GetArgName((*mit)->GetFunctionType(), argi));

          if (argi<cArgs-1)
            {
            Emit(os, ", ");
            }
          }
        Emit(os, ");\n");
        }

      if (*mit != registerM && *mit != unRegisterM)
        {
        EmitIndent(os, 3);
        Emit(os, "}\n");
        }


      EmitIndent(os, 2);
      Emit(os, "}\n");


      if (!voidReturn)
        {
        Emit(os, "\n");

        EmitIndent(os);
        Emit(os, "return rv;\n");
        }

      Emit(os, "}\n");
      }

    ++i;
    }


  // Constructor:
  //
  Emit(os, "\n");
  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");
  Emit(os, s.c_str());
  Emit(os, "::");
  Emit(os, s.c_str());
  Emit(os, "()\n");
  Emit(os, "{\n");

  // Initialize pointers to 0 and bool flags to false:
  //
  EmitIndent(os);
  Emit(os, "this->PrimaryDelegate = 0;\n");

  EmitIndent(os);
  Emit(os, "this->SecondaryDelegate = 0;\n");

  i = 0;
  for (gxsys_stl::vector<cable::Method*>::const_iterator mit = wrapped_methods.begin();
    mit != wrapped_methods.end(); ++mit)
    {
    if ((*mit)->GetVirtual())
      {
      EmitIndent(os);
      Emit(os, "this->CallingPrimary_");
      Emit(os, (*mit)->GetName());
      Emit(os, "_");
      EmitUint(os, i+1);
      Emit(os, " = false;\n");
      }

    ++i;
    }

  if (ClassIsA(c, "vtkObject"))
    {
    Emit(os, "\n");
    Emit(os, "#ifdef VTK_DEBUG_LEAKS\n");

    EmitIndent(os);
    Emit(os, "vtkDebugLeaks::ConstructClass(\"");
    Emit(os, c->GetName());
    Emit(os, "\");\n");

    Emit(os, "#endif\n");
    }

  Emit(os, "}\n");


  // Destructor:
  //
  Emit(os, "\n");
  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");
  Emit(os, s.c_str());
  Emit(os, "::~");
  Emit(os, s.c_str());
  Emit(os, "()\n");
  Emit(os, "{\n");
  EmitIndent(os);
  Emit(os, "this->DisconnectShadowDelegates();\n");
  Emit(os, "}\n");


  // And one extra export layer style function to create an instance of the
  // shadow class:
  //
  Emit(os, "\n");
  Emit(os, "\n");
  Emit(os, "//----------------------------------------------------------------------------\n");
  Emit(os, "extern \"C\" MUMMY_DLL_EXPORT\n");

  Emit(os, s.c_str());
  Emit(os, "* ");
  Emit(os, s.c_str());
  Emit(os, "_CreateShadow(");
  Emit(os, GetPrimaryDelegateCxxType(c).c_str());
  Emit(os, " primary)\n");

  Emit(os, "{\n");

  EmitIndent(os);
  Emit(os, s.c_str());
  Emit(os, "* rv = ");
  Emit(os, s.c_str());
  Emit(os, "::CreateShadow(primary);\n");

  EmitIndent(os);
  Emit(os, "return rv;\n");

  Emit(os, "}\n");
}


//----------------------------------------------------------------------------
void MummyCsharpShadowLayerGenerator::EmitClassForShadowLayer(gxsys_ios::ostream &os, const cable::Class *c)
{
  // Gather wrapped elements:
  //
  gxsys_stl::vector<cable::Method*> wrapped_methods;
  cable::Method *factoryM = 0;
  cable::Method *disposalM = 0;
  cable::Method *registerM = 0;
  cable::Method *unRegisterM = 0;

  this->GetCsharpGenerator()->GatherWrappedMethods(c, wrapped_methods, factoryM, disposalM, registerM, unRegisterM, true);

  // For the purposes of the shadow layer, make sure the Register and UnRegister
  // methods are emitted (same as any other wrapped method) in the shadow class:
  //
  wrapped_methods.insert(wrapped_methods.begin(), unRegisterM);
  wrapped_methods.insert(wrapped_methods.begin(), registerM);

  EmitClassDeclarationForShadowLayer(os, c, wrapped_methods, factoryM, disposalM, registerM, unRegisterM);

  EmitClassImplementationForShadowLayer(os, c, wrapped_methods, factoryM, disposalM, registerM, unRegisterM);
}
