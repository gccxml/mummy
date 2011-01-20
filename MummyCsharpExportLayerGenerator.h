//----------------------------------------------------------------------------
// MummyCsharpExportLayerGenerator.h
//
// Author(s) : David Cole
//
// Copyright (C) 2006-2007 Kitware, Inc.
//----------------------------------------------------------------------------

#include "MummyGenerator.h" // parent class

#include "gxsys/stl/string"

class MummyCsharpGenerator;

class MummyCsharpExportLayerGenerator : public MummyGenerator
{
public:
  MummyCsharpExportLayerGenerator();
  virtual ~MummyCsharpExportLayerGenerator();

  virtual bool GenerateWrappers();

  virtual MummyCsharpGenerator* GetCsharpGenerator();
  virtual void SetCsharpGenerator(MummyCsharpGenerator* generator);

  //virtual bool FundamentalTypeIsWrappable(const cable::Type* t);
  //virtual bool TypeIsWrappable(const cable::Type* t);
  //virtual bool FunctionTypeIsWrappable(const cable::FunctionType* ft);
  //virtual bool MethodIsWrappable(const cable::Method* m, const cable::Context::Access& access);
  //virtual bool ClassIsWrappable(const cable::Class* c);

  virtual const char *GetArgName(cable::FunctionType *ftype, unsigned int i);
  virtual gxsys_stl::string GetArgTypeAndNameString(cable::Type *argType, const char *name, bool stripConst);
  //virtual gxsys_stl::string GetArgTypeAndNameString(cable::FunctionType *ft, const unsigned int i);
  virtual gxsys_stl::string GetExportLayerFunctionName(const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname);

  virtual void EmitSpecialHandlingForObjectPointerReturns(gxsys_ios::ostream &os, const gxsys_stl::string& cname, const cable::Method *m, const gxsys_stl::string& mname, const unsigned int indent);
  virtual void EmitClassMethodDeclarationForExportLayer(gxsys_ios::ostream &os, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname, bool emitExceptionParams);
  virtual void EmitClassMethodForExportLayer(gxsys_ios::ostream &os, const cable::Class *c, const cable::Method *m, const gxsys_stl::string& mname);
  virtual void EmitClassForExportLayer(gxsys_ios::ostream &os, const cable::Class *c);

private:
  MummyCsharpGenerator* CsharpGenerator;

  MummyCsharpExportLayerGenerator(const MummyCsharpExportLayerGenerator&); //Not implemented
  MummyCsharpExportLayerGenerator& operator=(const MummyCsharpExportLayerGenerator&); //Not implemented
};
