//----------------------------------------------------------------------------
// MummyCsharpGenerator.h
//
// Author(s) : David Cole
//
// Copyright (C) 2006-2008 Kitware, Inc.
//----------------------------------------------------------------------------

#ifndef _MummyCsharpGenerator_h_
#define _MummyCsharpGenerator_h_

#include "MummyGenerator.h" // parent class

#include "gxsys/stl/map"
#include "gxsys/stl/string"
#include "gxsys/stl/vector"

class MummyCsharpGenerator : public MummyGenerator
{
public:
  MummyCsharpGenerator();
  virtual ~MummyCsharpGenerator();

  virtual void SetTargetClass(const cable::Class *c);
  virtual void CacheExternalHints(const gxsys_stl::string& hintsfile);

  virtual void AddTargetInterface(const gxsys_stl::string& iface);
  virtual bool HasTargetInterface(const char *iface) const;

  virtual bool IsKeyword(const char *p);
  virtual bool IsReservedMethodName(const gxsys_stl::string &name);
  virtual gxsys_stl::string GetFundamentalTypeString(const cable::Type *t);
  virtual gxsys_stl::string GetWrappedMethodName(const cable::Method *m);
  virtual gxsys_stl::string GetMethodArgumentArraySize(const cable::Class *c, const cable::Method *m, const cable::FunctionType *ft, unsigned int i);
  virtual gxsys_stl::string GetMethodSignature(const cable::Class *c, const cable::Method *m);
  virtual const char *GetArgName(cable::FunctionType *ftype, unsigned int i);
  virtual bool GetIsRefArg(const cable::Type *t);
  virtual gxsys_stl::string GetPInvokeTypeString(const cable::Type *t, bool forReturn, bool isArray, bool forDelegate);
  virtual gxsys_stl::string GetCSharpTypeString(const cable::Type *t, bool forReturn, bool isArray);
  virtual void ClearLookupEntries();
  virtual void AddLookupEntries(const cable::Class* c);
  virtual void DumpLookupEntries();
  virtual bool OtherMethodRedefined(const gxsys_stl::string &signature);
  virtual bool StaticMethodRedefined(const gxsys_stl::string &signature);
  virtual bool VirtualMethodOverridden(const gxsys_stl::string &signature);
  virtual bool WrappedMethodExists(const gxsys_stl::string &signature);
  virtual bool WrappedEnumExists(const gxsys_stl::string &name);

  virtual bool FundamentalTypeIsWrappable(const cable::Type* t);
  virtual bool TypeIsWrappable(const cable::Type* t);
  virtual bool FunctionTypeIsWrappable(const cable::FunctionType* ft);
  virtual bool MethodWrappableAsEvent(const cable::Method* m, const cable::Context::Access& access);
  virtual bool MethodIsWrappable(const cable::Method* m, const cable::Context::Access& access);
  virtual bool ClassIsWrappable(const cable::Class* c);
  virtual const cable::Class* GetWrappableParentClass(const cable::Class *c);

  virtual bool IsFactoryMethod(const cable::Class *c, const cable::Method *m);
  virtual bool IsDisposalMethod(const cable::Class *c, const cable::Method *m);
  virtual bool MethodReturnValueIsCounted(const cable::Class *c, const cable::Method *m);
  virtual gxsys_stl::string GetExportLayerFunctionName(const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname);
  virtual void EmitCSharpMethodDeclaration(gxsys_ios::ostream &os, const cable::Class *c, const cable::Method *m, bool asProperty, bool useArg0AsReturn, const gxsys_stl::string& accessLevel);
  virtual void EmitCSharpDllImportDeclaration(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, const char *f, bool emitExceptionParams);
  virtual void EmitCSharpProperty(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *propGetMethod, const cable::Method *propSetMethod, bool emitExceptionParams);
  virtual void EmitCSharpMethodBody(gxsys_ios::ostream &os, unsigned int indent, const cable::Class *c, const cable::Method *m, gxsys_stl::string& f, const char *impliedArg0, bool emitExceptionParams);
  virtual void EmitCSharpMethod(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, const gxsys_stl::string& accessLevel, bool emitExceptionParams);
  virtual void EmitCSharpEvent(gxsys_ios::ostream& os, const char *dllname, const cable::Class* c, const cable::Method* m, const gxsys_stl::string& eventName);
  virtual void EmitCSharpEnums(gxsys_ios::ostream &os, const cable::Class *c);
  virtual void EmitCSharpConstructor(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, bool emitExceptionParams);
  virtual void EmitCSharpRegister(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, bool emitExceptionParams);
  virtual void EmitCSharpDispose(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, const unsigned int eventCount, bool emitExceptionParams);
  virtual void EmitCSharpWrapperClassAsStruct(gxsys_ios::ostream &os, const cable::Class *c);
  virtual void GatherWrappedMethods(
    const cable::Class *c,
    gxsys_stl::vector<cable::Method*>& wrapped_methods,
    cable::Method*& factoryM,
    cable::Method*& disposalM,
    cable::Method*& registerM,
    cable::Method*& unRegisterM,
    bool includeParentMethods
    );
  virtual bool ValidateWrappedMethods(
    const cable::Class *c,
    gxsys_stl::vector<cable::Method*>& wrapped_methods,
    cable::Method*& factoryM,
    cable::Method*& disposalM,
    cable::Method*& registerM,
    cable::Method*& unRegisterM
    );
  virtual void BuildPropGetsAndSetsMap(
    gxsys_stl::vector<cable::Method*>& wrapped_methods,
    gxsys_stl::map<gxsys_stl::string, gxsys_stl::pair<cable::Method*, cable::Method*> >& wrapped_properties
    );
  virtual void EmitCSharpWrapperClass(gxsys_ios::ostream &os, const char *dllname, const cable::Class *c);

protected:
  // Provide implementation for inherited pure virtuals:
  virtual bool GenerateWrappers();

private:
  unsigned int CurrentMethodId;
  unsigned int ClassLineNumber;
  gxsys_stl::map<const cable::Method*, unsigned int> MethodIdMap;
  gxsys_stl::string TargetInterface;
  gxsys_stl::map<const gxsys_stl::string, gxsys_stl::string> HintsMap;

  MummyCsharpGenerator(const MummyCsharpGenerator&); //Not implemented
  MummyCsharpGenerator& operator=(const MummyCsharpGenerator&); //Not implemented
};

#endif
