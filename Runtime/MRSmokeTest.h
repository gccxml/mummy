/* -----------------------------------------------------------------------------
 * MRSmokeTest.h
 *
 * Author(s) : David Cole
 *
 * Copyright (C) 2007 Kitware, Inc.
 * ----------------------------------------------------------------------------- */

#ifndef mummy_MRSmokeTest_h
#define mummy_MRSmokeTest_h

#ifdef _WIN32
#ifdef MRSmokeTest_EXPORTS
#define MRSMOKETEST_EXPORT __declspec(dllexport)
#else
#define MRSMOKETEST_EXPORT __declspec(dllimport)
#endif
#endif

#ifndef MRSMOKETEST_EXPORT
#define MRSMOKETEST_EXPORT
#endif

class MRSMOKETEST_EXPORT MRSmokeTest
{
public:
  MRSmokeTest();
  virtual ~MRSmokeTest();

  MRSmokeTest* Clone();

private:
  MRSmokeTest(const MRSmokeTest&);
  const MRSmokeTest& operator=(const MRSmokeTest&);
};

#endif
