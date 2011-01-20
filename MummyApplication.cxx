//----------------------------------------------------------------------------
//
//  $Id: MummyApplication.cxx 506 2010-07-27 15:30:53Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2010-07-27 11:30:53 -0400 (Tue, 27 Jul 2010) $
//  $Revision: 506 $
//
//  Copyright (C) 2006-2009 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MummyApplication.h"
#include "MummyCsharpGenerator.h"
#include "MummyCsharpExportLayerGenerator.h"
#include "MummyCsharpShadowLayerGenerator.h"
#include "MummyCsharpUnitTestGenerator.h"
#include "MummyLog.h"
#include "MummySettings.h"

#include "cableClass.h"
#include "cableClassType.h"
#include "cableContext.h"
#include "cableMethod.h"
#include "cableNamespace.h"
#include "cableSourceRepresentation.h"
#include "cableTypedef.h"
#include "cableVariable.h"
#include "cableXMLSourceParser.h"
#include "cxxFundamentalType.h"

#include "gxsys/ios/fstream"
#include "gxsys/ios/iostream"
#include "gxsys/ios/sstream"
#include "gxsys/stl/map"
#include "gxsys/stl/set"
#include "gxsys/stl/string"

#include "string.h" // strlen


//----------------------------------------------------------------------------
MummyApplication::MummyApplication()
{
  this->Settings = 0;
}


//----------------------------------------------------------------------------
MummyApplication::~MummyApplication()
{
}


//----------------------------------------------------------------------------
int MummyApplication::Main(int argc, char *argv[])
{
  int err = 0;

  MummySettings settings;
  this->SetSettings(&settings);

  err = settings.Initialize(argc, argv);

  if (!err && settings.ShouldRun())
    {
    cable::SourceRepresentation::Pointer sr = BuildSourceRepresentation();

    err = ProcessSource(sr);
    }

  this->SetSettings(0);

  return GetFirstErrorValue();
}


//----------------------------------------------------------------------------
MummySettings* MummyApplication::GetSettings()
{
  return this->Settings;
}


//----------------------------------------------------------------------------
void MummyApplication::SetSettings(MummySettings* settings)
{
  this->Settings = settings;
}


//----------------------------------------------------------------------------
cable::SourceRepresentation* MummyApplication::BuildSourceRepresentation()
{
  const char* inFileName = this->GetSettings()->GetGccxmlFileName();

  if (!inFileName || (0 == strlen(inFileName)))
    {
    LogError(me_InvalidArg, << "Must specify valid --gccxml-file (use --help for options).");
    return 0;
    }

  LogVerboseInfo(<< "Using --gccxml-file '" << inFileName << "'");

  // Open the XML input file produced by GCC-XML.
  gxsys_ios::ifstream inFile(inFileName, std::ios_base::in|std::ios_base::binary);
  if (!inFile)
    {
    LogError(me_CouldNotOpen, << "Could not open GCC-XML output: " << inFileName);
    return 0;
    }

  // Parse the XML input file.
  cable::XMLSourceParser::Pointer parser = cable::XMLSourceParser::New();
  parser->SetStream(&inFile);
  if(!parser->Parse())
    {
    LogError(me_CouldNotParse, << "Could not parse GCC-XML output: " << inFileName);
    return 0;
    }
  parser->SetStream(0);
  inFile.close();

  // Get the parsed source representation.
  return parser->GetSourceRepresentation();
}


//----------------------------------------------------------------------------
int MummyApplication::ProcessSource(cable::SourceRepresentation* sr)
{
  int err = 0;

  if (!sr)
    {
    LogError(me_InvalidArg, << "MummyApplication::ProcessSource");
    return me_InvalidArg;
    }

  gxsys_stl::string s;
  const cable::Namespace* gns = sr->GetGlobalNamespace();

  // Look inside the _cable_ namespace for a wrappers namespace
  // and classes within:
  //
  cable::Context::Iterator lower = gns->LowerBound("_cable_");
  cable::Context::Iterator upper = gns->UpperBound("_cable_");
  cable::Namespace* cns = 0;
  if (lower != upper)
    {
    cns = cable::Namespace::SafeDownCast(*lower);
    }
  if (!cns)
    {
    LogError(me_UnexpectedGccxmlInput, << "MummyApplication::ProcessSource");
    return me_UnexpectedGccxmlInput;
    }

  // Give the settings object a chance to read package, packageVersion
  // and group info out of the source rep:
  //
  err = this->GetSettings()->ProcessSource(sr);

  // Collect up all the classes to be wrapped:
  //
  gxsys_stl::map<const cable::Class*, cable::Typedef*> TypedefLookup;
  gxsys_stl::set<const cable::Class*> ClassesToBeIncluded;

  const cable::Namespace* wns = 0;
  cable::Context::Iterator wlower = cns->LowerBound("wrappers");
  cable::Context::Iterator wupper = cns->UpperBound("wrappers");
  if (wlower != wupper)
    {
    wns = cable::Namespace::SafeDownCast(*wlower);
    }
  if (!wns)
    {
    LogError(me_UnexpectedGccxmlInput, << "MummyApplication::ProcessSource");
    return me_UnexpectedGccxmlInput;
    }

  for (cable::Context::Iterator w = wns->Begin(); w != wns->End(); ++w)
    {
    cable::Typedef* td = cable::Typedef::SafeDownCast(*w);
    if(td)
      {
      const cable::ClassType* ct = cable::ClassType::SafeDownCast(td->GetType());
      if(ct)
        {
        const cable::Class* c = ct->GetClass();
        TypedefLookup.insert(gxsys_stl::make_pair(c, td));
        ClassesToBeIncluded.insert(c);
        }
      }
    }

  // Wrap 'em:
  //
  for (gxsys_stl::set<const cable::Class*>::iterator i =
    ClassesToBeIncluded.begin(); !err && i != ClassesToBeIncluded.end(); ++i)
    {
    const cable::Class* c = *i;

    //cable::Typedef* td = 0;
    //gxsys_stl::map<const cable::Class*, cable::Typedef*>::iterator tdi =
    //  TypedefLookup.find(c);
    //if(tdi != TypedefLookup.end())
    //  {
    //  td = tdi->second;
    //  }

    err = this->ProcessClass(sr, c);
    }

  return err;
}


//----------------------------------------------------------------------------
int MummyApplication::ProcessClass(cable::SourceRepresentation* sr, const cable::Class* c)
{
  int err = 0;

  if (!ValidateBaseClasses(c))
    {
    LogError(me_CouldNotValidate, << "ValidateBaseClasses failed for class '" << c->GetName() << "'");
    return me_CouldNotValidate;
    }

  if (!this->GetSettings()->ClassIsWrappable(c))
    {
    LogError(me_CouldNotWrap, << "ClassIsWrappable returned false for class '" << c->GetName() << "'");
    return me_CouldNotWrap;
    }

  MummyCsharpGenerator csg;
  gxsys_ios::ostringstream ossCS;
  csg.SetSettings(this->GetSettings());
  csg.SetSourceRepresentation(sr);
  csg.SetStream(&ossCS);
  csg.SetTargetClass(c);
  csg.Generate();

  MummyCsharpExportLayerGenerator cselg;
  gxsys_ios::ostringstream ossEL;
  cselg.SetCsharpGenerator(&csg);
  cselg.SetSettings(this->GetSettings());
  cselg.SetSourceRepresentation(sr);
  cselg.SetStream(&ossEL);
  cselg.SetTargetClass(c);
  cselg.Generate();

  if (this->GetSettings()->GetUseShadow(c))
    {
    MummyCsharpShadowLayerGenerator csslg;
    csslg.SetCsharpGenerator(&csg);
    csslg.SetSettings(this->GetSettings());
    csslg.SetSourceRepresentation(sr);
    csslg.SetStream(&ossEL);
    csslg.SetTargetClass(c);
    csslg.Generate();
    }

  MummyCsharpUnitTestGenerator csutg;
  gxsys_ios::ostringstream ossUT;
  csutg.SetCsharpGenerator(&csg);
  csutg.SetSettings(this->GetSettings());
  csutg.SetSourceRepresentation(sr);
  csutg.SetStream(&ossUT);
  csutg.SetTargetClass(c);
  csutg.Generate();

  WriteToFile(this->GetSettings()->GetCsharpFileName(c).c_str(), ossCS.str().c_str());
  LogVerboseInfo("Wrote file: " << this->GetSettings()->GetCsharpFileName(c));

  WriteToFile(this->GetSettings()->GetExportLayerFileName(c).c_str(), ossEL.str().c_str());
  LogVerboseInfo("Wrote file: " << this->GetSettings()->GetExportLayerFileName(c));

// Not yet ready for prime time:
//  WriteToFile(this->GetSettings()->GetCsharpUnitTestFileName(c).c_str(), ossUT.str().c_str());
//  LogVerboseInfo("Wrote file: " << this->GetSettings()->GetCsharpUnitTestFileName(c));

  return err;
}


/// \page roadmap mummy Road Map (Prioritized ToDo List)
///
///
/// // // // //  P1  // // //
///
/// \todo move the MethodInstance data structures inside the C# generator class...
///
/// \todo search for generated "C" style comments : indicator
/// of "wrapped stuff edge cases"...
///
/// \todo make sure that delegates are wrappable function pointer typedefs,
/// not just public function pointer typedefs
///
/// \todo address IntPtr \<--\> I4 issue for 64-bit builds:
/// mummy need to generate different code for 64-bit?
/// generate code that determines IntPtr size at runtime?
///
/// \todo error/warning stack: treat warnings as errors, suppress *all* warnings...
///
/// \todo consolidate GetEnumerationTypeString and GetWrappedEnumName
///
/// \todo IsReservedMethodName need any more work?
///
///
/// // // // //  P2  // // //
///
/// \todo perfect delegate signature generation
///
/// \todo eliminate \<Chunk\> near-copy of code in delegate generation block
///
/// \todo eliminate hard-coded 4100/4099 numbers
///
/// \todo wrap static methods in iwhUtility classes (there's no reason why we
/// shouldn't / couldn't...)
///
/// \todo make enum only classes "root" C# classes -- no need to inherit from
///
/// \todo perfect the BTX ETX exclusion mechanism (multiple HeaderFileReader
/// objects for parent classes too) for "equivalent to the other wrappers"
/// VTK wrapping...
///
/// \todo add hint detection for special method (factory, disposal,
/// register, unRegister) designation
///
/// \todo add KWStyle analysis dashboard of source code
///
/// \todo enable wrapping deprecated methods? would anybody want this?
/// (right now they are always excluded from wrapping)
///
/// \todo how to wrap smart pointers smartly? how does ITK/Java do it?
///
///
/// // // // //  P3  // // //
///
/// \todo more build info cached in executable : resource of text output:
/// cvs status of CableSwig/CMakeLists.txt, svn info (for revision of
/// mummy source directory), build environment, development environment
/// version (VS, gcc/cl version), platform environment
///
/// \todo shadow classes : another way to implement them besides IDispatch??
/// existing implementation should really only provide overrides of the methods
/// directly in the class itself
/// depending on parent classes could introduce wrapper dependencies we don't want
/// if the generation of supporting code is conditional based on whether the class
/// being wrapped is a shadow or not
///
/// \todo get rid of trailing whitespace in documentation comment blocks??
///
/// \todo remove unnecessary comment blocks (saved/old code, now unneeded)
///
/// \todo web page / html documentation
///
/// \todo document all code (at least all classes and methods)
/// point to MummyApplication::ProcessClass as main entry point of interest
/// document all possible MummySettings.xml values
///
/// \todo document assumptions made of hand-crafted base class of generated
/// classes:
///   WrappedObject
///   IDisposable
///   destructor
///   GetCallDisposalMethod
///   GetCppThis
///   ClearCppThis
///   etc.
///
/// \todo document "event assumptions"...
///
/// \todo write a blurb about performance tests for direct C# array access
/// that explains it requires use of the unsafe C# keyword and use of
/// the /unsafe compiler command line flag


// // //  DONE  // // //

// get rid of "ref" args for things like "double*" and "int*" where
// we do not know how much memory the thing points to... Translate to an
// array if size is known, otherwise, translate to IntPtr -- only use
// "ref" for "double&" (actual reference-type args...)

// configure or generate an AssemblyInfo.cs with version number info in it
//   for each managed dll...

// pull in VTK hints file (similar to inline hint iwhArraySize)

// hand-craft(?) VTK events

// "install"

// identify all methods that require the iwhCounted hint because they
//   return objects that have already been registered (anything "like"
//   a factory method)

// flesh out C# keywords table to include *all* C# keywords

// Make getters const -- add warnings to mummy for void/non-const getters...
//   (added mw_PropGetReturnsVoid and mw_PropGetNotConst)

// hint to remove "property get/set or just set" code from *some* generated C#
//   structs?? -- iwhNoFieldAccess -- (add the hint, then write gets/sets
//   manually...)

// other Session Manager ModelCreated/Deleting events (namespace qualification
//   required...)

// handle NULL object pointer return values from methods properly

// test infrastructure work : mummy wrapping of "main" style funcs
// recognize argc/argv param pairs and transform to a single string[] param in
//   the C# method (automatic "main" wrapping...) -- if name is "main" transform
//    to "Main"??

// handle Register/UnRegister properly

// turn warnings back on

// add "iwhPropGet/iwhPropSet needed?" for "^[gGsS]et" methods warning

// warning suppression code, so we can ignore the many, many warnings
//   from wrapping VTK while still keeping them for other code bases...

// make header file header blocks uniform -- update copyright everywhere:
//   header and source file leading comment blocks

// re-structuring (table of all objects, one-to-one map,
//   methodology for removing objects from table, a working "KeepAlive" strategy)

// table stuff and other incidentals while I'm in there...

// consistently log errors when functions that return type strings encounter
// an unknown type

// make sure wrapped reference args work at runtime - done in SignatureSeries
// example class

// search for any generated "ERROR_" chunks


//----------------------------------------------------------------------------
/// \mainpage mummy - Wrapper Generator
/// mummy is a command line executable that generates C# wrappers from gccxml
/// output. A C# class is generated to wrap the wrappable class named in the
/// gccxml output. Settings to control the wrapping are given inline directly
/// in the class header file or in the MummySettings.xml input file.
///
/// mummy transfers comment blocks in the source code as XML documentation
/// comments. If they are already XML documentation comments, as indicated by
/// the presence of \<remarks\> or \<summary\> in the comment block, then they
/// are simply copied over as is. If not, they are surrounded by \<remarks\>
/// and \</remarks\> tags to indicate to doxygen that the comment block is the
/// detailed description block.
///
/// The following table shows XML documentation tags and their doxygen and
/// JavaDoc equivalents:
///
/// \verbatim
/// XML documentation                                   doxygen command equivalent  Javadoc comment tag equivalent
/// ==================================================  ==========================  ===============================
/// <remarks>                                           \detail                     None
/// <summary>                                           \brief                      None
/// <author>name</author>                               \author name                @author name           
/// <obsolete>description</obsolete>                    None                        @deprecated description
/// <exception cref="exception-class">text</exception>  None                        @exception exception-class text
/// <param name="paramname">text</param>                \param paramname text       @param paramname text
/// <returns>text</returns>                             \return text                @return text
/// <seealso cref="typename"/>                          \ref typename               @see typename
/// <seealso cref="typename.member"/>                   \sa typename::member        @see typename#member
/// <value>property-description</value>                 None                        @value property-description
/// <version>text</version>                             None                        @version text
/// \endverbatim
///
/// Sources:
///
/// http://msdn2.microsoft.com/en-us/library/fzdc5d5k%28VS.80%29.aspx
///
/// http://www.stack.nl/~dimitri/doxygen/xmlcmds.html
///
/// Full doxygen manual:
///
/// http://www.stack.nl/~dimitri/doxygen/manual.html
///
/// \author David Cole
