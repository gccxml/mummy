//----------------------------------------------------------------------------
//
//  $Id: MummyTypeEntry.cxx 2 2007-12-17 18:15:56Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2007-12-17 13:15:56 -0500 (Mon, 17 Dec 2007) $
//  $Revision: 2 $
//
//  Copyright (C) 2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MummyTypeEntry.h"

#include <string>

//----------------------------------------------------------------------------
namespace Kitware
{
namespace mummy
{

//----------------------------------------------------------------------------
class TypeEntryInternals
{
public:
  std::string ClassNameKey;
  Runtime::TypeEntryIndex Index;
  Runtime::ObjectRefCountChangedCallback ObjectRefCountChangedCallback;
};

//----------------------------------------------------------------------------
TypeEntry::TypeEntry(const char* classNameKey, const Runtime::TypeEntryIndex& index)
{
  this->Internals = new TypeEntryInternals;

  if (classNameKey)
    {
    this->Internals->ClassNameKey = classNameKey;
    }
  else
    {
    //this->Internals->ClassNameKey = ""; // as initialized...
    }

  this->Internals->Index = index;

  this->Internals->ObjectRefCountChangedCallback = Runtime::ObjectRefCountChanged;
}

//----------------------------------------------------------------------------
TypeEntry::~TypeEntry()
{
  delete this->Internals;
  this->Internals = 0;
}

//----------------------------------------------------------------------------
const char* TypeEntry::GetClassNameKey() const
{
  return this->Internals->ClassNameKey.c_str();
}

//----------------------------------------------------------------------------
Runtime::TypeEntryIndex TypeEntry::GetIndex() const
{
  return this->Internals->Index;
}

//----------------------------------------------------------------------------
Runtime::ObjectRefCountChangedCallback TypeEntry::GetObjectRefCountChangedCallback() const
{
  return this->Internals->ObjectRefCountChangedCallback;
}

//----------------------------------------------------------------------------
const char* TypeEntry::Print() const
{
  // david.cole::fix -- todo -- implement this...
  return "";
}

//----------------------------------------------------------------------------
}
}
