//----------------------------------------------------------------------------
//
//  $Id: MummyCsharpUnitTestGenerator.cxx 2 2007-12-17 18:15:56Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2007-12-17 13:15:56 -0500 (Mon, 17 Dec 2007) $
//  $Revision: 2 $
//
//  Copyright (C) 2006-2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MummyCsharpUnitTestGenerator.h"
#include "MummyCsharpGenerator.h"
#include "MummyLog.h"
#include "MummySettings.h"

#include "cableClass.h"
#include "cableClassType.h"
#include "cableFunctionType.h"
#include "cableMethod.h"
#include "cablePointerType.h"
#include "cableReferenceType.h"
#include "cxxCvQualifiedType.h"
#include "cxxFunctionType.h"
#include "cxxPointerType.h"

#include "gxsys/stl/algorithm"
#include "gxsys/SystemTools.hxx"


//----------------------------------------------------------------------------
MummyCsharpUnitTestGenerator::MummyCsharpUnitTestGenerator()
{
  this->CsharpGenerator = 0;
}


//----------------------------------------------------------------------------
MummyCsharpUnitTestGenerator::~MummyCsharpUnitTestGenerator()
{
}


//----------------------------------------------------------------------------
bool MummyCsharpUnitTestGenerator::GenerateWrappers()
{
  this->EmitClass(*GetStream(), GetTargetClass());
  return false;
}


//----------------------------------------------------------------------------
MummyCsharpGenerator* MummyCsharpUnitTestGenerator::GetCsharpGenerator()
{
  return this->CsharpGenerator;
}


//----------------------------------------------------------------------------
void MummyCsharpUnitTestGenerator::SetCsharpGenerator(MummyCsharpGenerator* generator)
{
  this->CsharpGenerator = generator;
}


//----------------------------------------------------------------------------
//bool MummyCsharpUnitTestGenerator::FundamentalTypeIsWrappable(const cable::Type* t)
//{
//  return this->GetCsharpGenerator()->FundamentalTypeIsWrappable(t);
//}


//----------------------------------------------------------------------------
//bool MummyCsharpUnitTestGenerator::TypeIsWrappable(const cable::Type* t)
//{
//  return this->GetCsharpGenerator()->TypeIsWrappable(t);
//}


//----------------------------------------------------------------------------
//bool MummyCsharpUnitTestGenerator::FunctionTypeIsWrappable(const cable::FunctionType* ft)
//{
//  return this->GetCsharpGenerator()->FunctionTypeIsWrappable(ft);
//}


//----------------------------------------------------------------------------
//bool MummyCsharpUnitTestGenerator::MethodIsWrappable(const cable::Method* m, const cable::Context::Access& access)
//{
//  return this->GetCsharpGenerator()->MethodIsWrappable(m, access);
//}


//----------------------------------------------------------------------------
//bool MummyCsharpUnitTestGenerator::ClassIsWrappable(const cable::Class* c)
//{
//  return this->GetCsharpGenerator()->ClassIsWrappable(c);
//}


//----------------------------------------------------------------------------
const char *MummyCsharpUnitTestGenerator::GetArgName(cable::FunctionType *ftype, unsigned int i)
{
  return this->GetCsharpGenerator()->GetArgName(ftype, i);
}


//----------------------------------------------------------------------------
void MummyCsharpUnitTestGenerator::EmitCSharpFactoryMethodUnitTest(gxsys_ios::ostream &os, const cable::Class *)
{
  Emit(os, "// EmitCSharpFactoryMethodUnitTest\n");
}


//----------------------------------------------------------------------------
void MummyCsharpUnitTestGenerator::EmitCSharpMethodUnitTest(gxsys_ios::ostream &os, const cable::Class *, const cable::Method*)
{
  Emit(os, "// EmitCSharpMethodUnitTest\n");
}


//----------------------------------------------------------------------------
void MummyCsharpUnitTestGenerator::EmitCSharpPropertyUnitTest(gxsys_ios::ostream &os, const cable::Class *, const cable::Method*, const cable::Method*)
{
  Emit(os, "// EmitCSharpPropertyUnitTest\n");
}


//----------------------------------------------------------------------------
void MummyCsharpUnitTestGenerator::EmitCSharpStructMemberAccessUnitTest(gxsys_ios::ostream &os, const cable::Class *)
{
  Emit(os, "// EmitCSharpStructMemberAccessUnitTest\n");
}


//----------------------------------------------------------------------------
void MummyCsharpUnitTestGenerator::EmitClass(gxsys_ios::ostream &os, const cable::Class *c)
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

  if (target_namespace != "")
    {
    target_namespace = target_namespace + ".UnitTests";
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
  Emit(os, "/// <summary>\n");
  Emit(os, "/// Automatically generated unit test\n");
  Emit(os, "/// </summary>\n");


  if (IsUtilityClass(c))
    {
    // Utility classes get wrapped as structs:
    //
    EmitCSharpStructMemberAccessUnitTest(os, c);
    }
  else
    {
  if (verbose)
    {
    LogInfo(mi_VerboseInfo, << "Calling GatherWrappedMethods...");
    }

  this->GetCsharpGenerator()->GatherWrappedMethods(c, wrapped_methods, factoryM, disposalM, registerM, unRegisterM, false);


  // Filter out prop gets and sets, putting them in their very own data structure.
  // Key in the map is the name of the property. 1st method in pair is propget,
  // 2nd method is propset.
  //
  gxsys_stl::map<gxsys_stl::string, gxsys_stl::pair<cable::Method*, cable::Method*> > wrapped_properties;
  this->GetCsharpGenerator()->BuildPropGetsAndSetsMap(wrapped_methods, wrapped_properties);

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


  // Class declaration:
  //
  Emit(os, "public ");
//  if (c->GetAbstract())
//    {
//    Emit(os, "abstract ");
//    }
  Emit(os, "class ");
  Emit(os, GetWrappedClassName(c).c_str());
  Emit(os, "UnitTest");

  Emit(os, "\n");


  // Open class:
  //
  Emit(os, "{\n");


  // Factory method test:
  //
  if (factoryM || this->GetSettings()->GetUseShadow(c))
    {
    Emit(os, "\n");
    Emit(os, "\n");
    EmitCSharpFactoryMethodUnitTest(os, c);
    }


  // Properties:
  //
  for (gsit = wrapped_properties.begin(); gsit != wrapped_properties.end(); ++gsit)
    {
    Emit(os, "\n");
    Emit(os, "\n");
    EmitCSharpPropertyUnitTest(os, c, gsit->second.first, gsit->second.second);
    }


  // Plain old methods:
  //
  for (mit = wrapped_methods.begin(); mit != wrapped_methods.end(); ++mit)
    {
    Emit(os, "\n");
    Emit(os, "\n");
    EmitCSharpMethodUnitTest(os, c, *mit);
    }
    }


  // Hand written shtuff:
  //
  // If there is extraCSharpCode, emit it *within* the class definition.
  // If it's there, it's the name of a file that we are to include in
  // its entirety...
  //
  gxsys_stl::string extraCode = this->GetSettings()->GetExtraCsharpUnitTestCode(c);
  if (extraCode != "")
    {
    Emit(os, "\n");
    Emit(os, "\n");
    Emit(os, "// Begin extraCSharpCode\n");
    Emit(os, "\n");
    EmitFile(os, extraCode.c_str());
    Emit(os, "\n");
    Emit(os, "// End extraCSharpCode\n");
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
