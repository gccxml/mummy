//----------------------------------------------------------------------------
//
//  $Id: MummySettings.cxx 64 2008-03-08 18:10:51Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2008-03-08 13:10:51 -0500 (Sat, 08 Mar 2008) $
//  $Revision: 64 $
//
//  Copyright (C) 2006-2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MummySettings.h"
#include "MummyDummyGenerator.h"
#include "MummyLineOrientedTextFileReader.h"
#include "MummyLog.h"
#include "MummyVersion.h"

#include "cableClass.h"
#include "cableNamespace.h"
#include "cableSourceRepresentation.h"
#include "cableVariable.h"
#include "cableXMLParser.h"

#include "gxsys/CommandLineArguments.hxx"
#include "gxsys/RegularExpression.hxx"
#include "gxsys/ios/fstream"
#include "gxsys/ios/iostream"
#include "gxsys/stl/algorithm"
#include "gxsys/stl/map"
#include "gxsys/stl/set"
#include "gxsys/stl/string"
#include "gxsys/stl/vector"


//----------------------------------------------------------------------------
static gxsys_stl::string Mummy_SVN_STATUS(Mummy_SVN_STATUS_ENCODED);

//----------------------------------------------------------------------------
class MummySettingsInternals
{
public:
  gxsys_stl::string ArgsHelpString;
  gxsys_stl::string CsharpFile;
  gxsys_stl::string CsharpUnitTestFile;
  gxsys_stl::string ExportLayerFile;
  gxsys_stl::string GccxmlFile;
  gxsys_stl::string Group;
  gxsys_stl::string Header;
  bool Help;
  gxsys_stl::string Package;
  gxsys_stl::string PackageVersion;
  gxsys_stl::vector<gxsys_stl::string> References;
  bool Run;
  gxsys_stl::string SettingsFile;
  gxsys_stl::vector<int> SuppressedWarnings;
  gxsys_stl::map<gxsys_stl::string, ClassWrappingSettings> unwrappable_classes;
  gxsys_stl::vector<ClassWrappingSettings> unwrappable_patterns;
  bool Verbose;
  bool Version;
  gxsys_stl::map<gxsys_stl::string, ClassWrappingSettings> wrappable_classes;
  gxsys_stl::vector<ClassWrappingSettings> wrappable_patterns;
};


//----------------------------------------------------------------------------
MummySettings::MummySettings()
{
  this->Internals = new MummySettingsInternals;
  this->Reset();
}


//----------------------------------------------------------------------------
MummySettings::~MummySettings()
{
  delete this->Internals;
}


//----------------------------------------------------------------------------
void MummySettings::Reset()
{
  this->Internals->ArgsHelpString = "";
  this->Internals->CsharpFile = "";
  this->Internals->CsharpUnitTestFile = "";
  this->Internals->ExportLayerFile = "";
  this->Internals->GccxmlFile = "";
  this->Internals->Group = "";
  this->Internals->Header = "";
  this->Internals->Help = false;
  this->Internals->Package = "";
  this->Internals->PackageVersion = "";
  this->Internals->References.clear();
  this->Internals->Run = true;
  this->Internals->SettingsFile = "";
  this->Internals->SuppressedWarnings.clear();
  this->Internals->unwrappable_classes.clear();
  this->Internals->unwrappable_patterns.clear();
  this->Internals->Verbose = false;
  this->Internals->Version = false;
  this->Internals->wrappable_classes.clear();
  this->Internals->wrappable_patterns.clear();
}


//----------------------------------------------------------------------------
void MummySettings::AddArgumentHandlers(gxsys::CommandLineArguments& arguments)
{
  arguments.AddArgument(
    "--settings-file",
    gxsys::CommandLineArguments::SPACE_ARGUMENT,
    &this->Internals->SettingsFile,
    "Input file describing mummy configuration settings, including the set of wrapped classes. Required."
    );

  arguments.AddArgument(
    "--csharp-file",
    gxsys::CommandLineArguments::SPACE_ARGUMENT,
    &this->Internals->CsharpFile,
    "C# output file. Default value is 'ClassName.cs' in the current directory."
    );

  arguments.AddArgument(
    "--export-layer-file",
    gxsys::CommandLineArguments::SPACE_ARGUMENT,
    &this->Internals->ExportLayerFile,
    "C++ output file. Default value is 'ClassNameEL.cxx' in the current directory."
    );

  arguments.AddArgument(
    "--csharp-unit-test-file",
    gxsys::CommandLineArguments::SPACE_ARGUMENT,
    &this->Internals->CsharpUnitTestFile,
    "C# output file. Default value is 'ClassNameUnitTest.cs' in the current directory."
    );

  arguments.AddArgument(
    "--gccxml-file",
    gxsys::CommandLineArguments::SPACE_ARGUMENT,
    &this->Internals->GccxmlFile,
    "Input file (output of gccxml) describing class to be wrapped. Required."
    );

  arguments.AddCallback(
    "--help",
    gxsys::CommandLineArguments::NO_ARGUMENT,
    MummySettings::DisplayHelp,
    this,
    "Display (this) detailed help information."
    );

  arguments.AddArgument(
    "--verbose",
    gxsys::CommandLineArguments::NO_ARGUMENT,
    &this->Internals->Verbose,
    "Overwhelm me with output, I don't have enough reading material... ;)"
    );

  arguments.AddCallback(
    "--version",
    gxsys::CommandLineArguments::NO_ARGUMENT,
    MummySettings::DisplayVersion,
    this,
    "Display the program version."
    );

  arguments.AddArgument(
    "--suppress-warnings",
    gxsys::CommandLineArguments::MULTI_ARGUMENT,
    &this->Internals->SuppressedWarnings,
    "Space separated list of warning numbers to suppress."
    );
}


//----------------------------------------------------------------------------
int MummySettings::Initialize(int argc, char *argv[])
{
  int err = 0;

  gxsys::CommandLineArguments arguments;

  this->Reset();

  arguments.Initialize(argc, argv);

  this->AddArgumentHandlers(arguments);

  // Stash big help string in case we process "--help" during the Parse:
  this->Internals->ArgsHelpString = arguments.GetHelp();

  if (!arguments.Parse())
    {
    err = me_CouldNotParse;
    LogError(err, << "Could not parse all arguments in MummySettings::Initialize. Use --help for detailed help information.");
    }

  if (!err && this->ShouldRun())
    {
    gxsys_stl::vector<int>::iterator wit;
    for (wit = this->Internals->SuppressedWarnings.begin();
      wit != this->Internals->SuppressedWarnings.end();
      ++wit)
      {
      SuppressMsg(*wit);
      }

    if (this->Internals->SettingsFile == "")
      {
      err = me_MissingRequiredCommandLineArg;
      LogError(err, << "Required setting --settings-file not given. Use --help for detailed help information.");
      }

    if (!err && this->Internals->GccxmlFile == "")
      {
      err = me_MissingRequiredCommandLineArg;
      LogError(err, << "Required setting --gccxml-file not given. Use --help for detailed help information.");
      }

    if (!err && this->Internals->Verbose)
      {
      LogInfo(mi_VerboseInfo, << "--verbose mode turned on.");
      }

    if (!err)
      {
      err = this->ParseSettingsFile(this->GetSettingsFileName());
      }
    }

  return err;
}


//----------------------------------------------------------------------------
void MummySettings::AddUnwrappableClass(const ClassWrappingSettings &cws)
{
  this->Internals->unwrappable_classes.insert(gxsys_stl::make_pair(cws.name, cws));
}


//----------------------------------------------------------------------------
void MummySettings::AddWrappableClass(const ClassWrappingSettings &cws)
{
  this->Internals->wrappable_classes.insert(gxsys_stl::make_pair(cws.name, cws));
}


//----------------------------------------------------------------------------
void MummySettings::AddUnwrappablePattern(const ClassWrappingSettings &cws)
{
  this->Internals->unwrappable_patterns.push_back(cws);
}


//----------------------------------------------------------------------------
struct SortBySortKey
{
  bool operator()(const ClassWrappingSettings& c1, const ClassWrappingSettings& c2)
  {
    return c1.sortkey < c2.sortkey;
  }
};


//----------------------------------------------------------------------------
void MummySettings::AddWrappablePattern(const ClassWrappingSettings &cws)
{
  this->Internals->wrappable_patterns.push_back(cws);

  // Sort the vector by sortkey now that a new pattern has been added:
  //
  gxsys_stl::sort(this->Internals->wrappable_patterns.begin(),
    this->Internals->wrappable_patterns.end(), SortBySortKey());
}


//----------------------------------------------------------------------------
void MummySettings::AddReference(const char *name)
{
  if (name)
    {
    this->Internals->References.push_back(gxsys_stl::string(name));
    }
}


//----------------------------------------------------------------------------
void MummySettings::GetReferences(gxsys_stl::vector<gxsys_stl::string>& references)
{
  references.clear();
  references.insert(references.begin(),
    this->Internals->References.begin(), this->Internals->References.end());
}


//----------------------------------------------------------------------------
bool MummySettings::FindClassWrappingSettings(const char *name, ClassWrappingSettings* cws)
{
  bool found = false;

  if (name)
    {
    gxsys_stl::map<gxsys_stl::string, ClassWrappingSettings>::iterator mit;
    gxsys_stl::vector<ClassWrappingSettings>::iterator vit;
    gxsys::RegularExpression re;
    gxsys_stl::string s(name);

    mit = this->Internals->wrappable_classes.find(s);
    if (mit != this->Internals->wrappable_classes.end())
      {
      found = true;
      if (cws)
        {
        (*cws) = mit->second;
        }
      }

    for (vit = this->Internals->wrappable_patterns.begin();
      !found && vit != this->Internals->wrappable_patterns.end(); ++vit)
      {
      re.compile(vit->pattern.c_str());
      if (re.find(s.c_str()))
        {
        found = true;
        if (cws)
          {
          (*cws) = *vit;
          }
        }
      }
    }

  return found;
}


//----------------------------------------------------------------------------
bool MummySettings::ClassIsWrappable(const cable::Class *c)
{
  bool wrappable = false;

  // A class is wrappable if:
  //  - it is not excluded by name or pattern match *and*
  //  - it is included by name or pattern match

  if (c)
    {
    gxsys_stl::map<gxsys_stl::string, ClassWrappingSettings>::iterator mit;
    gxsys_stl::vector<ClassWrappingSettings>::iterator vit;
    gxsys::RegularExpression re;
    gxsys_stl::string s(GetFullyQualifiedNameForCPlusPlus(c));

    mit = this->Internals->unwrappable_classes.find(s);
    if (mit != this->Internals->unwrappable_classes.end())
      {
      return false;
      }

    for (vit = this->Internals->unwrappable_patterns.begin();
      vit != this->Internals->unwrappable_patterns.end(); ++vit)
      {
      re.compile(vit->pattern.c_str());
      if (re.find(s.c_str()))
        {
        return false;
        }
      }

    wrappable = FindClassWrappingSettings(s.c_str(), 0);
    }

  return wrappable;
}


//----------------------------------------------------------------------------
#define READ_MummySettings_ATTS() \
{ \
  if (s=="beginExcludeRegex"){attBeginExcludeRegex = atts[i+1];knownAttribute = true;} \
  if (s=="countedMethodsRegex"){attCountedMethodsRegex = atts[i+1];knownAttribute = true;} \
  if (s=="csharpConstructorModifier"){attCsharpConstructorModifier = atts[i+1];knownAttribute = true;} \
  if (s=="defaultDisposalMethod"){attDefaultDisposalMethod = atts[i+1];knownAttribute = true;} \
  if (s=="defaultFactoryMethod"){attDefaultFactoryMethod = atts[i+1];knownAttribute = true;} \
  if (s=="emitDefaultFactoryMethod"){attEmitDefaultFactoryMethod = atts[i+1];knownAttribute = true;} \
  if (s=="endExcludeRegex"){attEndExcludeRegex = atts[i+1];knownAttribute = true;} \
  if (s=="exceptionBaseClass"){attExceptionBaseClass = atts[i+1];knownAttribute = true;} \
  if (s=="exceptionCloneMethod"){attExceptionCloneMethod = atts[i+1];knownAttribute = true;} \
  if (s=="exceptionInclude"){attExceptionInclude = atts[i+1];knownAttribute = true;} \
  if (s=="excludeMarkedLines"){attExcludeMarkedLines = atts[i+1];knownAttribute = true;} \
  if (s=="externalHints"){attExternalHints = atts[i+1];knownAttribute = true;} \
  if (s=="extraCsharpCode"){attExtraCsharpCode = atts[i+1];knownAttribute = true;} \
  if (s=="extraCsharpUnitTestCode"){attExtraCsharpUnitTestCode = atts[i+1];knownAttribute = true;} \
  if (s=="extraExportLayerCode"){attExtraExportLayerCode = atts[i+1];knownAttribute = true;} \
  if (s=="getMummyTypeEntryMethod"){attGetMummyTypeEntryMethod = atts[i+1];knownAttribute = true;} \
  if (s=="getRefCountMethod"){attGetRefCountMethod = atts[i+1];knownAttribute = true;} \
  if (s=="partial"){attPartial = atts[i+1];knownAttribute = true;} \
  if (s=="registerBaseClass"){attRegisterBaseClass = atts[i+1];knownAttribute = true;} \
  if (s=="registerMethod"){attRegisterMethod = atts[i+1];knownAttribute = true;} \
  if (s=="registerInclude"){attRegisterInclude = atts[i+1];knownAttribute = true;} \
  if (s=="setMummyTypeEntryMethod"){attSetMummyTypeEntryMethod = atts[i+1];knownAttribute = true;} \
  if (s=="shadow"){attShadow = atts[i+1];knownAttribute = true;} \
  if (s=="unRegisterMethod"){attUnRegisterMethod = atts[i+1];knownAttribute = true;} \
  if (s=="wrappedObjectBase"){attWrappedObjectBase = atts[i+1];knownAttribute = true;} \
}


//----------------------------------------------------------------------------
namespace cable
{
class SettingsParser : public XMLParser
{
public:
  cableTypeMacro(SettingsParser, XMLParser);
  static Pointer New() { return new Self; }

  MummySettings* GetSettings() { return this->Settings; }
  void SetSettings(MummySettings* settings) { this->Settings = settings; }

protected:
  virtual void StartElement(const char* name, const char** atts)
    {
    gxsys_stl::string elementName(name);

    gxsys_stl::string attName;
    gxsys_stl::string attPattern;
    gxsys_stl::string attSortKey;
    gxsys_stl::string attWrappable;

    gxsys_stl::string attCsharpConstructorModifier("");
    gxsys_stl::string attDefaultDisposalMethod("");
    gxsys_stl::string attDefaultFactoryMethod("");
    gxsys_stl::string attEmitDefaultFactoryMethod("false");
    gxsys_stl::string attCountedMethodsRegex("");
    gxsys_stl::string attShadow("false");
    gxsys_stl::string attExceptionBaseClass("");
    gxsys_stl::string attExceptionCloneMethod("");
    gxsys_stl::string attExceptionInclude("");
    gxsys_stl::string attExternalHints("");
    gxsys_stl::string attExtraCsharpCode("");
    gxsys_stl::string attExtraCsharpUnitTestCode("");
    gxsys_stl::string attExtraExportLayerCode("");
    gxsys_stl::string attPartial("false");
    gxsys_stl::string attRegisterMethod("");
    gxsys_stl::string attUnRegisterMethod("");
    gxsys_stl::string attRegisterBaseClass("");
    gxsys_stl::string attRegisterInclude("");
    gxsys_stl::string attGetRefCountMethod("");
    gxsys_stl::string attGetMummyTypeEntryMethod("");
    gxsys_stl::string attSetMummyTypeEntryMethod("");
    gxsys_stl::string attExcludeMarkedLines("false");
    gxsys_stl::string attBeginExcludeRegex("");
    gxsys_stl::string attEndExcludeRegex("");
    gxsys_stl::string attWrappedObjectBase("");

    gxsys_stl::string s;
    bool knownAttribute = false;


    if (elementName == "Class")
      {
      for (unsigned int i=0; atts[i] && atts[i+1]; i+=2)
        {
        s = atts[i];
        knownAttribute = false;

        if (s=="name"){attName = atts[i+1];knownAttribute = true;}
        if (s=="wrappable"){attWrappable = atts[i+1];knownAttribute = true;}

        READ_MummySettings_ATTS();

        if (!knownAttribute)
          {
          LogWarning(mw_UnknownAttribute, "Unknown XML attribute '"
            << elementName << "/@" << s << "'");
          }
        }

      ClassWrappingSettings cws(
        attName,
        attPattern,
        attSortKey,
        attDefaultFactoryMethod,
        attEmitDefaultFactoryMethod == "false" ? false : true,
        attCountedMethodsRegex,
        attDefaultDisposalMethod,
        attShadow == "false" ? false : true,
        attExternalHints,
        attExtraExportLayerCode,
        attExtraCsharpCode,
        attExtraCsharpUnitTestCode,
        attCsharpConstructorModifier,
        attRegisterMethod,
        attUnRegisterMethod,
        attGetRefCountMethod,
        attRegisterBaseClass,
        attRegisterInclude,
        attGetMummyTypeEntryMethod,
        attSetMummyTypeEntryMethod,
        attExcludeMarkedLines == "false" ? false : true,
        attBeginExcludeRegex,
        attEndExcludeRegex,
        attPartial == "false" ? false : true,
        attExceptionBaseClass,
        attExceptionCloneMethod,
        attExceptionInclude,
        attWrappedObjectBase
        );

      if (attWrappable == "false")
        {
        this->GetSettings()->AddUnwrappableClass(cws);
        }
      else
        {
        this->GetSettings()->AddWrappableClass(cws);
        }
      }


    if (elementName == "ClassPattern")
      {
      for (unsigned int i=0; atts[i] && atts[i+1]; i+=2)
        {
        s = atts[i];
        knownAttribute = false;

        if (s=="pattern"){attPattern = atts[i+1];knownAttribute = true;}
        if (s=="wrappable"){attWrappable = atts[i+1];knownAttribute = true;}
        if (s=="sortkey"){attSortKey = atts[i+1];knownAttribute = true;}

        READ_MummySettings_ATTS();

        if (!knownAttribute)
          {
          LogWarning(mw_UnknownAttribute, "Unknown XML attribute '"
            << elementName << "/@" << s << "'");
          }
        }  

      ClassWrappingSettings cws(
        attName,
        attPattern,
        attSortKey,
        attDefaultFactoryMethod,
        attEmitDefaultFactoryMethod == "false" ? false : true,
        attCountedMethodsRegex,
        attDefaultDisposalMethod,
        attShadow == "false" ? false : true,
        attExternalHints,
        attExtraExportLayerCode,
        attExtraCsharpCode,
        attExtraCsharpUnitTestCode,
        attCsharpConstructorModifier,
        attRegisterMethod,
        attUnRegisterMethod,
        attGetRefCountMethod,
        attRegisterBaseClass,
        attRegisterInclude,
        attGetMummyTypeEntryMethod,
        attSetMummyTypeEntryMethod,
        attExcludeMarkedLines == "false" ? false : true,
        attBeginExcludeRegex,
        attEndExcludeRegex,
        attPartial == "false" ? false : true,
        attExceptionBaseClass,
        attExceptionCloneMethod,
        attExceptionInclude,
        attWrappedObjectBase
        );

      if (attWrappable == "false")
        {
        this->GetSettings()->AddUnwrappablePattern(cws);
        }
      else
        {
        this->GetSettings()->AddWrappablePattern(cws);
        }
      }


    if (elementName == "Reference")
      {
      for (unsigned int i=0; atts[i] && atts[i+1]; i+=2)
        {
        s = atts[i];
        knownAttribute = false;

        if (s=="name"){attName = atts[i+1];knownAttribute = true;}

        if (!knownAttribute)
          {
          LogWarning(mw_UnknownAttribute, "Unknown XML attribute '"
            << elementName << "/@" << s << "'");
          }
        }

      this->GetSettings()->AddReference(attName.c_str());
      }
    }

private:
  MummySettings *Settings;
};
}


//----------------------------------------------------------------------------
int MummySettings::ParseSettingsFile(const char *filename)
{
  int err = 0;

  // The settings file shall be an XML file that lists classes explicitly
  // declared as wrapped or not, something like this:
  //
  // <MummySettings>
  //   <Class name="vtkCommand" wrappable="true" defaultFactoryMethod="New" defaultDisposalMethod="Delete" shadow="true"/>
  //   <ClassPattern pattern="^vtk" wrappable="true" defaultFactoryMethod="New" defaultDisposalMethod="Delete" shadow="false"/>
  // </MummySettings>
  //
  // Where the name of the class is the fully qualified C++ class name and
  // the only valid values for the wrapped attribute are the lower case
  // boolean constants "true" and "false".

  if (this->GetVerbose())
    {
    LogInfo(mi_VerboseInfo, << "Using --settings-file '" << filename << "'");
    }

  gxsys_ios::ifstream inFile(filename, std::ios_base::in|std::ios_base::binary);
  if (!inFile)
    {
    err = me_CouldNotOpen;
    LogFileLineErrorMsg(filename, 1,
      err, << "Could not open file: '" << filename << "'");
    }

  if (!err)
    {
    cable::SettingsParser::Pointer parser = cable::SettingsParser::New();
    parser->SetStream(&inFile);
    parser->SetSettings(this);
    if(!parser->Parse())
      {
      err = me_CouldNotParse;
      LogFileLineErrorMsg(filename, 1,
        err, << "Could not parse file: '" << filename << "'");
      }
    parser->SetStream(0);
    parser->SetSettings(0);
    inFile.close();
    }

  return err;
}


//----------------------------------------------------------------------------
bool ParseName(const char* name, gxsys_stl::string& result)
{
  gxsys_stl::string group = name;
  gxsys_stl::string::size_type l = group.find('"');
  gxsys_stl::string::size_type r = group.rfind('"');
  if((l != gxsys_stl::string::npos) && (r != gxsys_stl::string::npos) && (r > l))
    {
    result = group.substr(l+1, r-l-1);
    return true;
    }
  return false;
}


//----------------------------------------------------------------------------
bool GetGroupAndPackageInformation(cable::Namespace* cns,
  gxsys_stl::string& groupName,
  gxsys_stl::vector<gxsys_stl::string>& groupsNames,
  gxsys_stl::string& packageName,
  gxsys_stl::string& packageVersion,
  gxsys_stl::string& headerName,
  gxsys_stl::string& configFile)
{
  // Find the group, if any.
  const cable::Variable* group = 0;
  cable::Context::Iterator lower = cns->LowerBound("group");
  cable::Context::Iterator upper = cns->UpperBound("group");
  if (lower != upper)
    {
    group = cable::Variable::SafeDownCast(*lower);
    if (!group)
      {
      LogError(me_UnexpectedGccxmlInput, << "Identifier _cable_::group is not a variable.");
      return false;
      }
    }

  // Find the groups list, if any.
  const cable::Variable* groups = 0;
  lower = cns->LowerBound("groups");
  upper = cns->UpperBound("groups");
  if (lower != upper)
    {
    groups = cable::Variable::SafeDownCast(*lower);
    if (!groups)
      {
      LogError(me_UnexpectedGccxmlInput, << "Identifier _cable_::groups is not a variable.");
      return false;
      }
    }

  // Find the package name, if any.
  const cable::Variable* package = 0;
  lower = cns->LowerBound("package");
  upper = cns->UpperBound("package");
  if (lower != upper)
    {
    package = cable::Variable::SafeDownCast(*lower);
    if (!package)
      {
      LogError(me_UnexpectedGccxmlInput, << "Identifier _cable_::package is not a variable.");
      return false;
      }
    }

  // Find the package version, if any.
  const cable::Variable* package_version = 0;
  lower = cns->LowerBound("package_version");
  upper = cns->UpperBound("package_version");
  if (lower != upper)
    {
    package_version = cable::Variable::SafeDownCast(*lower);
    if (!package_version)
      {
      LogError(me_UnexpectedGccxmlInput, << "Identifier _cable_::package_version is not a variable.");
      return false;
      }
    }

  // Find the header, if any.
  const cable::Variable* header = 0;
  lower = cns->LowerBound("header");
  upper = cns->UpperBound("header");
  if (lower != upper)
    {
    header = cable::Variable::SafeDownCast(*lower);
    if (!header)
      {
      LogError(me_UnexpectedGccxmlInput, << "Identifier _cable_::header is not a variable.");
      return false;
      }
    }

  // Parse the strings out of the found entities:
  //
  if (group)
    {
    if (!ParseName(group->GetInitializer(), groupName))
      {
      LogError(me_CouldNotParse, << "Error parsing group name.");
      return false;
      }

    // Hold on to the name of the configuration file.
    configFile = group->GetFile();
    }

  if (package && !ParseName(package->GetInitializer(), packageName))
    {
    LogError(me_CouldNotParse, << "Error parsing package name.");
    return false;
    }

  if (package_version &&
     !ParseName(package_version->GetInitializer(), packageVersion))
    {
    LogError(me_CouldNotParse, << "Error parsing package_version string.");
    return false;
    }

  if (groups)
    {
    gxsys_stl::string gl = groups->GetInitializer();
    gxsys_stl::string::size_type lpos = gl.find('"');
    while((lpos != gxsys_stl::string::npos) && (lpos < gl.length()))
      {
      gxsys_stl::string::size_type rpos = gl.find_first_of(",}", lpos);
      gxsys_stl::string result;
      if((rpos == gxsys_stl::string::npos) ||
         !ParseName(gl.substr(lpos, rpos-lpos).c_str(), result))
        {
        LogError(me_CouldNotParse, << "Error parsing groups list.");
        return false;
        }
      groupsNames.push_back(result);
      lpos = rpos+1;
      }
    }

  if (header)
    {
    if (!ParseName(header->GetInitializer(), headerName))
      {
      LogError(me_CouldNotParse, << "Error parsing header name.");
      return false;
      }
    }

  return true;
}


//----------------------------------------------------------------------------
int MummySettings::ProcessSource(cable::SourceRepresentation* sr)
{
  int err = 0;

  if (!sr)
    {
    LogError(me_InvalidArg, << "NULL pointer!");
    return me_InvalidArg;
    }

  gxsys_stl::string s;
  const cable::Namespace* gns = sr->GetGlobalNamespace();

  // Look inside the _cable_ namespace for package, packageversion and
  // group settings:
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
    LogError(me_UnexpectedGccxmlInput, << "No _cable_ namespace.");
    return me_UnexpectedGccxmlInput;
    }

  gxsys_stl::string group;
  gxsys_stl::vector<gxsys_stl::string> groups;
  gxsys_stl::string package;
  gxsys_stl::string packageVersion;
  gxsys_stl::string header;
  gxsys_stl::string configFile;
  GetGroupAndPackageInformation(cns, group, groups, package, packageVersion, header, configFile);
  if (group.size() == 0)
    {
    if (package.size() == 0)
      {
      LogError(me_UnexpectedGccxmlInput, << "No group or package name specified.");
      return me_UnexpectedGccxmlInput;
      }
    }

  // Save for when clients query us later:
  //
  this->Internals->Group = group;
  this->Internals->Header = header;
  this->Internals->Package = package;
  this->Internals->PackageVersion = packageVersion;

  return err;
}


//----------------------------------------------------------------------------
static void IncreaseCoverage(MummySettings *settings)
{
  // This function exists only to add test calls to functions that are
  // otherwise not commonly called so that they get some sort of coverage
  // on coverage dashboards...
  //
  Trace("IncreaseCoverage\n");

  MummyDummyGenerator mg;
  mg.FundamentalTypeIsWrappable(0);
  mg.TypeIsWrappable(0);
  mg.FunctionTypeIsWrappable(0);
  mg.MethodIsWrappable(0, cable::Context::Public);
  mg.ClassIsWrappable(0);
  mg.GenerateWrappers();

  GetMostRecentErrorValue();
  GetNthErrorValue(-10);
  GetNthErrorValue(0); // valid if there have been any errors...
  GetNthErrorValue(GetErrorCount()-1); // valid if there have been any errors...
  GetNthErrorValue(GetErrorCount());
  GetNthErrorValue(GetErrorCount()+10);

  // Can't do this... then the test that calls this code fails because there
  // was an "error"......
  //
  //LogError(me_InternalError,
  //  << "This is not really an error." << gxsys_ios::endl
  //  << "  This is IncreaseCoverage increasing the coverage of the" << gxsys_ios::endl
  //  << "  MummyUtilities error functions..."
  //  );
  //GetNthErrorValue(-10);
  //GetNthErrorValue(0); // valid if there have been any errors...
  //GetNthErrorValue(GetErrorCount()-1); // valid if there have been any errors...
  //GetNthErrorValue(GetErrorCount());
  //GetNthErrorValue(GetErrorCount()+10);

  EncodeStringForXml(0);
  EncodeStringForXml("");

  settings->GetHeader();
  settings->GetPackageVersion();

  MummyLineOrientedTextFileReader reader;
  reader.SetFileName(0);
}


//----------------------------------------------------------------------------
int MummySettings::DisplayHelp(const char* argument, const char* value,
  void* call_data)
{
  MummySettings *settings = (MummySettings *) call_data;

  DisplayVersion(argument, value, call_data);

  // If run under a ctest driven dashboard test, emit the string that tells
  // ctest not to truncate the test output:
  //
  if (getenv("DART_TEST_FROM_DART"))
    {
    gxsys_ios::cout << gxsys_ios::endl;
    gxsys_ios::cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << gxsys_ios::endl;
    }

  gxsys_ios::cout << gxsys_ios::endl;
  gxsys_ios::cout << "Command line options:" << gxsys_ios::endl;
  gxsys_ios::cout << settings->Internals->ArgsHelpString << gxsys_ios::endl;

  if (!Mummy_SVN_STATUS.empty())
    {
    gxsys_ios::cout << gxsys_ios::endl;
    gxsys_ios::cout << "svn status (retrieved at CMake configure time):" << gxsys_ios::endl;
    gxsys_ios::cout << "===============================================" << gxsys_ios::endl;
    gxsys_ios::cout << Mummy_SVN_STATUS.c_str() << gxsys_ios::endl;
    gxsys_ios::cout << gxsys_ios::endl;
    }

  IncreaseCoverage(settings);

  settings->Internals->Help = true;
  settings->Internals->Run = false;
  return 1;
}


//----------------------------------------------------------------------------
int MummySettings::DisplayVersion(const char*, const char*, void* call_data)
{
  MummySettings *settings = (MummySettings *) call_data;

  gxsys_ios::cout << settings->GetMummyVersion() << gxsys_ios::endl;

  settings->Internals->Version = true;
  settings->Internals->Run = false;
  return 1;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetMummyVersion()
{
  return Mummy_FULL_VERSION_STRING;
}


//----------------------------------------------------------------------------
const char *MummySettings::GetSettingsFileName()
{
  return this->Internals->SettingsFile.c_str();
}


//----------------------------------------------------------------------------
const char *MummySettings::GetCsharpFileName()
{
  return this->Internals->CsharpFile.c_str();
}


//----------------------------------------------------------------------------
const char *MummySettings::GetExportLayerFileName()
{
  return this->Internals->ExportLayerFile.c_str();
}


//----------------------------------------------------------------------------
const char *MummySettings::GetCsharpUnitTestFileName()
{
  return this->Internals->CsharpUnitTestFile.c_str();
}


//----------------------------------------------------------------------------
const char *MummySettings::GetGccxmlFileName()
{
  return this->Internals->GccxmlFile.c_str();
}


//----------------------------------------------------------------------------
const char *MummySettings::GetGroup()
{
  return this->Internals->Group.c_str();
}


//----------------------------------------------------------------------------
const char *MummySettings::GetHeader()
{
  return this->Internals->Header.c_str();
}


//----------------------------------------------------------------------------
const char *MummySettings::GetPackage()
{
  return this->Internals->Package.c_str();
}


//----------------------------------------------------------------------------
const char *MummySettings::GetPackageVersion()
{
  return this->Internals->PackageVersion.c_str();
}


//----------------------------------------------------------------------------
bool MummySettings::ShouldRun()
{
  return this->Internals->Run;
}


//----------------------------------------------------------------------------
bool MummySettings::GetVerbose()
{
  return this->Internals->Verbose;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetCsharpFileName(const cable::Class* c)
{
  gxsys_stl::string s(this->GetCsharpFileName());

  if (s == "")
    {
    s = c->GetName();
    s += ".cs";
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetExportLayerFileName(const cable::Class* c)
{
  gxsys_stl::string s(this->GetExportLayerFileName());

  if (s == "")
    {
    s = c->GetName();
    s += "EL.cxx";
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetCsharpUnitTestFileName(const cable::Class* c)
{
  gxsys_stl::string s(this->GetCsharpUnitTestFileName());

  if (s == "")
    {
    s = c->GetName();
    s += "UnitTest.cs";
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetCsharpConstructorModifier(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.csharpConstructorModifier;
    }
  else
    {
    s = "ERROR_NoCsharpConstructorModifier";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetFactoryMethod(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.defaultFactoryMethod;
    }
  else
    {
    s = "ERROR_NoFactoryMethod";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
bool MummySettings::GetEmitDefaultFactoryMethod(const cable::Class* c)
{
  bool b = false;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    b = cws.emitDefaultFactoryMethod;
    }
  else
    {
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return b;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetCountedMethodsRegex(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.countedMethodsRegex;
    }
  else
    {
    s = "ERROR_NoCountedMethodsRegex";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetDisposalMethod(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.defaultDisposalMethod;
    }
  else
    {
    s = "ERROR_NoDisposalMethod";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
bool MummySettings::GetUseShadow(const cable::Class* c)
{
  bool shadow = false;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    shadow = cws.shadow;
    }
  else
    {
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return shadow;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetExternalHints(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.externalHints;
    }
  else
    {
    s = "ERROR_NoExternalHints";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetExtraCsharpCode(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.extraCsharpCode;
    }
  else
    {
    s = "ERROR_NoExtraCsharpCode";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetExtraExportLayerCode(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.extraExportLayerCode;
    }
  else
    {
    s = "ERROR_NoExtraExportLayerCode";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetExtraCsharpUnitTestCode(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.extraCsharpUnitTestCode;
    }
  else
    {
    s = "ERROR_NoExtraCsharpUnitTestCode";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetRegisterMethod(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.registerMethod;
    }
  else
    {
    s = "ERROR_NoRegisterMethod";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetUnRegisterMethod(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.unRegisterMethod;
    }
  else
    {
    s = "ERROR_NoUnRegisterMethod";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetRegisterBaseClass(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.registerBaseClass;
    }
  else
    {
    s = "ERROR_NoRegisterBaseClass";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummySettings::GetRegisterInclude(const cable::Class* c)
{
  gxsys_stl::string s;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    s = cws.registerInclude;
    }
  else
    {
    s = "ERROR_NoRegisterInclude";
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return s;
}


//----------------------------------------------------------------------------
bool MummySettings::GetPartialClass(const cable::Class* c)
{
  bool partial = false;
  ClassWrappingSettings cws;

  if (FindClassWrappingSettings(GetFullyQualifiedNameForCPlusPlus(c).c_str(), &cws))
    {
    partial = cws.partialClass;
    }
  else
    {
    LogError(me_NoClassWrappingSettings,
      << "Could not find class wrapping settings for class '" << GetFullyQualifiedNameForCPlusPlus(c).c_str() << "'");
    }

  return partial;
}
