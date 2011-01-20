//----------------------------------------------------------------------------
// MummyDummyGenerator.h
//
// Author(s) : David Cole
//
// Copyright (C) 2006-2008 Kitware, Inc.
//----------------------------------------------------------------------------

#ifndef _MummyDummyGenerator_h_
#define _MummyDummyGenerator_h_

#include "MummyGenerator.h" // parent class

//----------------------------------------------------------------------------
/// <summary>
/// "Dummy" concrete derived class - its only purpose is to increase the
/// coverage results.
/// </summary>
class MummyDummyGenerator : public MummyGenerator
{
public:
  MummyDummyGenerator();
  virtual ~MummyDummyGenerator();
  virtual bool GenerateWrappers();
};

#endif
