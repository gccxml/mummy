//----------------------------------------------------------------------------
// MummySettings.h
//
// Author(s) : David Cole
//
// Copyright (C) 2006-2007 Kitware, Inc.
//----------------------------------------------------------------------------

#include "MummyUtilities.h" // first mummy include for root classes

#include "gxsys/stl/string"
#include "gxsys/stl/vector"

class MummySettingsInternals;

namespace cable
{
  class Class;
  class SourceRepresentation;
}

namespace gxsys
{
  class CommandLineArguments;
}


//----------------------------------------------------------------------------
class ClassWrappingSettings
{
public:
  gxsys_stl::string name;
  gxsys_stl::string pattern;
  gxsys_stl::string sortkey;
  gxsys_stl::string defaultFactoryMethod;
  bool emitDefaultFactoryMethod;
  gxsys_stl::string countedMethodsRegex;
  gxsys_stl::string defaultDisposalMethod;
  bool shadow;
  gxsys_stl::string externalHints;
  gxsys_stl::string extraExportLayerCode;
  gxsys_stl::string extraCsharpCode;
  gxsys_stl::string extraCsharpUnitTestCode;
  gxsys_stl::string csharpConstructorModifier;
  gxsys_stl::string registerMethod;
  gxsys_stl::string unRegisterMethod;
  gxsys_stl::string getRefCountMethod;
  gxsys_stl::string registerBaseClass;
  gxsys_stl::string registerInclude;
  gxsys_stl::string getMummyTypeEntryMethod;
  gxsys_stl::string setMummyTypeEntryMethod;
  bool excludeMarkedLines;
  gxsys_stl::string beginExcludeRegex;
  gxsys_stl::string endExcludeRegex;
  bool partialClass;
  gxsys_stl::string exceptionBaseClass;
  gxsys_stl::string exceptionCloneMethod;
  gxsys_stl::string exceptionInclude;
  gxsys_stl::string wrappedObjectBase;

  ClassWrappingSettings(
    gxsys_stl::string nm = gxsys_stl::string(""),
    gxsys_stl::string pat = gxsys_stl::string(""),
    gxsys_stl::string sk = gxsys_stl::string(""),
    gxsys_stl::string fm = gxsys_stl::string(""),
    bool edfm = false,
    gxsys_stl::string cmr = gxsys_stl::string(""),
    gxsys_stl::string dm = gxsys_stl::string(""),
    bool sh = false,
    gxsys_stl::string exH = gxsys_stl::string(""),
    gxsys_stl::string exEL = gxsys_stl::string(""),
    gxsys_stl::string exCS = gxsys_stl::string(""),
    gxsys_stl::string exCSUT = gxsys_stl::string(""),
    gxsys_stl::string csCM = gxsys_stl::string(""),
    gxsys_stl::string rm = gxsys_stl::string(""),
    gxsys_stl::string um = gxsys_stl::string(""),
    gxsys_stl::string grcm = gxsys_stl::string(""),
    gxsys_stl::string rbc = gxsys_stl::string(""),
    gxsys_stl::string ri = gxsys_stl::string(""),
    gxsys_stl::string gmtem = gxsys_stl::string(""),
    gxsys_stl::string smtem = gxsys_stl::string(""),
    bool eml = false,
    gxsys_stl::string ber = gxsys_stl::string(""),
    gxsys_stl::string eer = gxsys_stl::string(""),
    bool partial = false,
    gxsys_stl::string ebc = gxsys_stl::string(""),
    gxsys_stl::string ecm = gxsys_stl::string(""),
    gxsys_stl::string ei = gxsys_stl::string(""),
    gxsys_stl::string wob = gxsys_stl::string("")
    ) :
    name(nm),
    pattern(pat),
    sortkey(sk),
    defaultFactoryMethod(fm),
    emitDefaultFactoryMethod(edfm),
    countedMethodsRegex(cmr),
    defaultDisposalMethod(dm),
    shadow(sh),
    externalHints(exH),
    extraExportLayerCode(exEL),
    extraCsharpCode(exCS),
    extraCsharpUnitTestCode(exCSUT),
    csharpConstructorModifier(csCM),
    registerMethod(rm),
    unRegisterMethod(um),
    getRefCountMethod(grcm),
    registerBaseClass(rbc),
    registerInclude(ri),
    getMummyTypeEntryMethod(gmtem),
    setMummyTypeEntryMethod(smtem),
    excludeMarkedLines(eml),
    beginExcludeRegex(ber),
    endExcludeRegex(eer),
    partialClass(partial),
    exceptionBaseClass(ebc),
    exceptionCloneMethod(ecm),
    exceptionInclude(ei),
    wrappedObjectBase(wob)
  {
  }
};


//----------------------------------------------------------------------------
class MummySettings
{
public:
  MummySettings();
  virtual ~MummySettings();

  virtual int Initialize(int argc, char *argv[]);

  virtual int ParseSettingsFile(const char *filename);
  virtual int ProcessSource(cable::SourceRepresentation* sr);

  static int DisplayHelp(const char* argument, const char* value, void* call_data);
  static int DisplayVersion(const char* argument, const char* value, void* call_data);

  virtual const char *GetSettingsFileName();
  virtual const char *GetCsharpFileName();
  virtual const char *GetExportLayerFileName();
  virtual const char *GetCsharpUnitTestFileName();
  virtual const char *GetGccxmlFileName();
  virtual const char *GetGroup();
  virtual const char *GetHeader();
  virtual const char *GetPackage();
  virtual const char *GetPackageVersion();
  virtual bool ShouldRun();
  virtual bool GetVerbose();
  virtual gxsys_stl::string GetMummyVersion();

  virtual void AddUnwrappableClass(const ClassWrappingSettings &cws);
  virtual void AddWrappableClass(const ClassWrappingSettings &cws);
  virtual void AddUnwrappablePattern(const ClassWrappingSettings &cws);
  virtual void AddWrappablePattern(const ClassWrappingSettings &cws);
  virtual bool FindClassWrappingSettings(const char *name, ClassWrappingSettings* cws);

  virtual void AddReference(const char *name);
  virtual void GetReferences(gxsys_stl::vector<gxsys_stl::string>& references);

  virtual bool ClassIsWrappable(const cable::Class *c);

  virtual gxsys_stl::string GetCsharpFileName(const cable::Class* c);
  virtual gxsys_stl::string GetExportLayerFileName(const cable::Class* c);
  virtual gxsys_stl::string GetCsharpUnitTestFileName(const cable::Class* c);
  virtual gxsys_stl::string GetCsharpConstructorModifier(const cable::Class* c);
  virtual gxsys_stl::string GetFactoryMethod(const cable::Class* c);
  virtual bool GetEmitDefaultFactoryMethod(const cable::Class* c);
  virtual gxsys_stl::string GetCountedMethodsRegex(const cable::Class* c);
  virtual gxsys_stl::string GetDisposalMethod(const cable::Class* c);
  virtual bool GetUseShadow(const cable::Class* c);
  virtual gxsys_stl::string GetExternalHints(const cable::Class* c);
  virtual gxsys_stl::string GetExtraCsharpCode(const cable::Class* c);
  virtual gxsys_stl::string GetExtraExportLayerCode(const cable::Class* c);
  virtual gxsys_stl::string GetExtraCsharpUnitTestCode(const cable::Class* c);
  virtual gxsys_stl::string GetRegisterMethod(const cable::Class* c);
  virtual gxsys_stl::string GetUnRegisterMethod(const cable::Class* c);
  virtual gxsys_stl::string GetRegisterBaseClass(const cable::Class* c);
  virtual gxsys_stl::string GetRegisterInclude(const cable::Class* c);
  virtual bool GetPartialClass(const cable::Class* c);

private:
  void AddArgumentHandlers(gxsys::CommandLineArguments& arguments);
  void Reset();

  MummySettingsInternals *Internals;
};
