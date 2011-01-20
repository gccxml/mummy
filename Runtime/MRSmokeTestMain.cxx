//----------------------------------------------------------------------------
//
//  $Id: MRSmokeTestMain.cxx 418 2009-03-19 19:41:27Z hoffman $
//
//  $Author: hoffman $
//  $Date: 2009-03-19 15:41:27 -0400 (Thu, 19 Mar 2009) $
//  $Revision: 418 $
//
//  Copyright (C) 2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MRSmokeTest.h"
#include "MummyRuntime.h"
#include <iostream>
#include "stdlib.h" // atoi

//----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  int err = 0;
  int i = 0;
  int n = 10000;

  if (argc>1)
  {
    n = atoi(argv[1]);
  }

  // Call ObjectRefCountChanged with bogus parameters (NULL
  // pointer, silly ref counts) to make sure it's robust...
  //
  Kitware::mummy::Runtime::ObjectRefCountChanged(0, 333, 444);

  // Call Verify *before* the loop:
  //
  std::cout << "Calling Kitware::mummy::Runtime::Verify" << std::endl;
  Kitware::mummy::Runtime::Verify();

  std::cout << "n: " << n << std::endl;
  for (i= 0; i<n; ++i)
  {
    MRSmokeTest* test = new MRSmokeTest;
    MRSmokeTest* clone = test->Clone();
    Kitware::mummy::Runtime::LookupInstanceTypeEntry(test);
    Kitware::mummy::Runtime::LookupInstanceTypeEntry(clone);
    delete clone;
    delete test;
  }

  // Valid calls (as long as there is at least one entry by now...):
  //
  std::cout << "Testing mummy::Runtime methods with valid calls..." << std::endl;
  MRSmokeTest* test2 = new MRSmokeTest;
  MRSmokeTest* clone2 = test2->Clone();

  Kitware::mummy::Runtime::ObjectRefCountChanged(test2, 1, 2);
  Kitware::mummy::Runtime::LookupInstanceTypeEntry(test2);
  Kitware::mummy::Runtime::LookupInstanceTypeEntry(clone2);
  Kitware::mummy::Runtime::GetTypeEntryByIndex(0);
  Kitware::mummy::Runtime::GetTypeEntryClassNameKey(0);
  Kitware::mummy::Runtime::ObjectRefCountChanged(test2, 2, 1);

  delete clone2;
  delete test2;

  std::cout << "Calling Kitware::mummy::Runtime::Print" << std::endl;
  std::cout << Kitware::mummy::Runtime::Print() << std::endl;

  std::cout << "Calling Kitware::mummy::Runtime::Verify" << std::endl;
  Kitware::mummy::Runtime::Verify();

  std::cout << "Calling Kitware::mummy::Runtime::GetTypeEntryByIndex(0)" << std::endl;
  Kitware::mummy::TypeEntry* entry = Kitware::mummy::Runtime::GetTypeEntryByIndex(0);
  if (entry)
    {
    std::cout << "entry:" << std::endl;
    std::cout << entry->Print() << std::endl;
    entry->GetObjectRefCountChangedCallback();
    }
  else
    {
    std::cout << "error: no entry found at index 0" << std::endl;
    err = 1;
    }

  // Invalid calls (unless there are somehow 4 billion++ entries...):
  //
  std::cout << "Testing mummy::Runtime methods with invalid calls..." << std::endl;
  Kitware::mummy::Runtime::ObjectRefCountChanged(0, 0, 0);
  Kitware::mummy::Runtime::LookupInstanceTypeEntry(0);
  Kitware::mummy::Runtime::GetTypeEntryByIndex((Kitware::mummy::Runtime::TypeEntryIndex) -1);
  Kitware::mummy::Runtime::GetTypeEntryClassNameKey((Kitware::mummy::Runtime::TypeEntryIndex) -1);

  std::cout << "Test done" << std::endl;

  return err;
}
