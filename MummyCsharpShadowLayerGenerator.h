//----------------------------------------------------------------------------
// MummyCsharpShadowLayerGenerator.h
//
// Author(s) : David Cole
//
// Copyright (C) 2006-2007 Kitware, Inc.
//----------------------------------------------------------------------------

#include "MummyGenerator.h" // parent class

#include "gxsys/stl/string"
#include "gxsys/stl/vector"

class MummyCsharpGenerator;

class MummyCsharpShadowLayerGenerator : public MummyGenerator
{
public:
  MummyCsharpShadowLayerGenerator();
  virtual ~MummyCsharpShadowLayerGenerator();

  virtual bool GenerateWrappers();

  virtual MummyCsharpGenerator* GetCsharpGenerator();
  virtual void SetCsharpGenerator(MummyCsharpGenerator* generator);

  virtual const char *GetArgName(cable::FunctionType *ftype, unsigned int i);

  virtual void EmitClassMethodDeclarationForShadowLayer(gxsys_ios::ostream &os, const cable::Class *c, const cable::Method *m, const unsigned int index, int indent, const char* implClassName);
  virtual void EmitClassDeclarationForShadowLayer(gxsys_ios::ostream &os, const cable::Class *c, const gxsys_stl::vector<cable::Method*>& wrapped_methods, cable::Method* factoryM, cable::Method* disposalM, cable::Method* registerM, cable::Method* unRegisterM);
  virtual void EmitClassImplementationForShadowLayer(gxsys_ios::ostream &os, const cable::Class *c, const gxsys_stl::vector<cable::Method*>& wrapped_methods, cable::Method* factoryM, cable::Method* disposalM, cable::Method* registerM, cable::Method* unRegisterM);
  virtual void EmitClassForShadowLayer(gxsys_ios::ostream &os, const cable::Class *c);

private:
  MummyCsharpGenerator* CsharpGenerator;

  MummyCsharpShadowLayerGenerator(const MummyCsharpShadowLayerGenerator&); //Not implemented
  MummyCsharpShadowLayerGenerator& operator=(const MummyCsharpShadowLayerGenerator&); //Not implemented
};
