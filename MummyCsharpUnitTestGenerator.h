//----------------------------------------------------------------------------
// MummyCsharpUnitTestGenerator.h
//
// Author(s) : David Cole
//
// Copyright (C) 2006-2007 Kitware, Inc.
//----------------------------------------------------------------------------

#include "MummyGenerator.h" // parent class

#include "gxsys/stl/string"

class MummyCsharpGenerator;

class MummyCsharpUnitTestGenerator : public MummyGenerator
{
public:
  MummyCsharpUnitTestGenerator();
  virtual ~MummyCsharpUnitTestGenerator();

  virtual bool GenerateWrappers();

  virtual MummyCsharpGenerator* GetCsharpGenerator();
  virtual void SetCsharpGenerator(MummyCsharpGenerator* generator);

  //virtual bool FundamentalTypeIsWrappable(const cable::Type* t);
  //virtual bool TypeIsWrappable(const cable::Type* t);
  //virtual bool FunctionTypeIsWrappable(const cable::FunctionType* ft);
  //virtual bool MethodIsWrappable(const cable::Method* m, const cable::Context::Access& access);
  //virtual bool ClassIsWrappable(const cable::Class* c);
  virtual const char *GetArgName(cable::FunctionType *ftype, unsigned int i);

  virtual void EmitCSharpFactoryMethodUnitTest(gxsys_ios::ostream &os, const cable::Class *c);
  virtual void EmitCSharpMethodUnitTest(gxsys_ios::ostream &os, const cable::Class *c, const cable::Method* m);
  virtual void EmitCSharpPropertyUnitTest(gxsys_ios::ostream &os, const cable::Class *c, const cable::Method* mGet, const cable::Method* mSet);
  virtual void EmitCSharpStructMemberAccessUnitTest(gxsys_ios::ostream &os, const cable::Class *c);
  virtual void EmitClass(gxsys_ios::ostream &os, const cable::Class *c);

private:
  MummyCsharpGenerator* CsharpGenerator;

  MummyCsharpUnitTestGenerator(const MummyCsharpUnitTestGenerator&); //Not implemented
  MummyCsharpUnitTestGenerator& operator=(const MummyCsharpUnitTestGenerator&); //Not implemented
};
