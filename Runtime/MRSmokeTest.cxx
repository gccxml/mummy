//----------------------------------------------------------------------------
//
//  $Id: MRSmokeTest.cxx 2 2007-12-17 18:15:56Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2007-12-17 13:15:56 -0500 (Mon, 17 Dec 2007) $
//  $Revision: 2 $
//
//  Copyright (C) 2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MRSmokeTest.h"
#include "MummyRuntime.h"
#include <typeinfo>

//----------------------------------------------------------------------------
MRSmokeTest::MRSmokeTest()
{
}

//----------------------------------------------------------------------------
MRSmokeTest::~MRSmokeTest()
{
}

//----------------------------------------------------------------------------
MRSmokeTest* MRSmokeTest::Clone()
{
  MRSmokeTest* rv = new MRSmokeTest;
  Kitware::mummy::Runtime::GetTypeEntry(rv, typeid(*rv).name());
  return rv;
}


//----------------------------------------------------------------------------
extern "C" MRSMOKETEST_EXPORT
MRSmokeTest* MRSmokeTest_new()
{
   MRSmokeTest* rv = new MRSmokeTest;
   return rv;
}


//----------------------------------------------------------------------------
extern "C" MRSMOKETEST_EXPORT
void MRSmokeTest_delete(MRSmokeTest* pThis)
{
   delete pThis;
}


//----------------------------------------------------------------------------
extern "C" MRSMOKETEST_EXPORT
MRSmokeTest* MRSmokeTest_Clone_01(MRSmokeTest* pThis)
{
   MRSmokeTest* rv = pThis->Clone();
   return rv;
}
