//----------------------------------------------------------------------------
//
//  $Id: MummyRuntime.cxx 2 2007-12-17 18:15:56Z david.cole $
//
//  $Author: david.cole $
//  $Date: 2007-12-17 13:15:56 -0500 (Mon, 17 Dec 2007) $
//  $Revision: 2 $
//
//  Copyright (C) 2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MummyRuntime.h"

#include <map>
#include <string>
#include <vector>

//----------------------------------------------------------------------------
namespace Kitware
{
namespace mummy
{

//----------------------------------------------------------------------------
Runtime Runtime::Instance;

//----------------------------------------------------------------------------
class RuntimeInternals
{
public:
  std::map<std::string, TypeEntry*> TypeEntriesMap;
  std::vector<TypeEntry*> TypeEntriesVector;
  Runtime::TypeEntryIndex NextTypeEntryIndex;
  std::map<unsigned int, Runtime::ObjectRefCountChangedCallback> Callbacks;
  unsigned int NextCallbackId;
};

//----------------------------------------------------------------------------
Runtime::Runtime()
{
  this->Internals = new RuntimeInternals;

  //this->Internals->TypeEntriesMap.clear(); // as initialized...
  //this->Internals->TypeEntriesVector.clear(); // as initialized...
  this->Internals->NextTypeEntryIndex = 0;
  //this->Internals->Callbacks.clear(); // as initialized...
  this->Internals->NextCallbackId = 0;
}

//----------------------------------------------------------------------------
Runtime::~Runtime()
{
  std::vector<TypeEntry*>::iterator it;
  for (
    it = this->Internals->TypeEntriesVector.begin();
    it != this->Internals->TypeEntriesVector.end();
    ++it)
    {
    delete *it;
    }
  this->Internals->TypeEntriesMap.clear();
  this->Internals->TypeEntriesVector.clear();

  delete this->Internals;
  this->Internals = 0;
}

//----------------------------------------------------------------------------
TypeEntry* Runtime::GetTypeEntry(void*, const char* classNameKey)
{
  TypeEntry* entry = 0;

  //if (obj)
  //  {
  //  entry = LookupInstanceTypeEntry(obj);
  //  }

  //if (0 == entry)
  //  {
    entry = LookupClassTypeEntry(classNameKey);
  //  }

  return entry;
}

//----------------------------------------------------------------------------
TypeEntry* Runtime::LookupInstanceTypeEntry(void*)
{
  // If this ever gets implemented to do an "external associative"
  // instance based lookup... uncomment the code in GetTypeEntry.
  // Only do it if instance based lookup might be faster than the
  // classNameKey lookup we already have...
  //
  return 0;
}

//----------------------------------------------------------------------------
TypeEntry* Runtime::LookupClassTypeEntry(const char* classNameKey)
{
  TypeEntry* entry = Instance.Internals->TypeEntriesMap[classNameKey];
  if (0 == entry)
    {
    entry = new TypeEntry(classNameKey, Instance.Internals->NextTypeEntryIndex);
    Instance.Internals->TypeEntriesMap[classNameKey] = entry;
    Instance.Internals->TypeEntriesVector.push_back(entry);
    ++Instance.Internals->NextTypeEntryIndex;
    }
  return entry;
}

//----------------------------------------------------------------------------
TypeEntry* Runtime::GetTypeEntryByIndex(TypeEntryIndex index)
{
  if (index<Instance.Internals->TypeEntriesVector.size())
    {
    return Instance.Internals->TypeEntriesVector[index];
    }
  // david.cole::fix -- todo -- throw exception instead of returning non-data...?
  return 0;
}

//----------------------------------------------------------------------------
Runtime::TypeEntryIndex Runtime::GetTypeEntryCount()
{
  return static_cast<Runtime::TypeEntryIndex>(
    Instance.Internals->TypeEntriesVector.size());
}

//----------------------------------------------------------------------------
const char* Runtime::GetTypeEntryClassNameKey(TypeEntryIndex index)
{
  if (index<Instance.Internals->TypeEntriesVector.size())
    {
    return Instance.Internals->TypeEntriesVector[index]->GetClassNameKey();
    }
  // david.cole::fix -- todo -- throw exception instead of returning non-data...?
  return "";
}

//----------------------------------------------------------------------------
unsigned int Runtime::AddObjectRefCountChangedCallback(ObjectRefCountChangedCallback cb)
{
  unsigned int cbid = Instance.Internals->NextCallbackId;
  Instance.Internals->Callbacks[cbid] = cb;
  ++Instance.Internals->NextCallbackId;
  return cbid;
}

//----------------------------------------------------------------------------
void Runtime::RemoveObjectRefCountChangedCallback(unsigned int cbid)
{
  Instance.Internals->Callbacks.erase(cbid);
}

//----------------------------------------------------------------------------
void Runtime::ObjectRefCountChanged(void *obj, unsigned int oldCount, unsigned int newCount)
{
  std::map<unsigned int, Runtime::ObjectRefCountChangedCallback>::iterator cbit;
  cbit = Instance.Internals->Callbacks.begin();
  while (cbit != Instance.Internals->Callbacks.end())
    {
    (*cbit->second)(obj, oldCount, newCount);
    ++cbit;
    }
}

//----------------------------------------------------------------------------
const char* Runtime::Print()
{
  // david.cole::fix -- todo -- implement this...
  return "";
}

//----------------------------------------------------------------------------
bool Runtime::Verify()
{
  // david.cole::fix -- todo -- implement this fully...

  if (Instance.Internals->TypeEntriesMap.size() !=
    Instance.Internals->TypeEntriesVector.size())
    {
    return false;
    }

  return true;
}

//----------------------------------------------------------------------------
}
}

//----------------------------------------------------------------------------
// ...and the built-in export layer:

//----------------------------------------------------------------------------

extern "C" MUMMY_RUNTIME_EXPORT
Kitware::mummy::Runtime::TypeEntryIndex Kitware_mummy_Runtime_GetTypeEntryCount()
{
  return Kitware::mummy::Runtime::GetTypeEntryCount();
}

extern "C" MUMMY_RUNTIME_EXPORT
const char* Kitware_mummy_Runtime_GetTypeEntryClassNameKey(Kitware::mummy::Runtime::TypeEntryIndex index)
{
  return Kitware::mummy::Runtime::GetTypeEntryClassNameKey(index);
}

extern "C" MUMMY_RUNTIME_EXPORT
void Kitware_mummy_Runtime_RemoveObjectRefCountChangedCallback(unsigned int cbid)
{
  return Kitware::mummy::Runtime::RemoveObjectRefCountChangedCallback(cbid);
}

extern "C" MUMMY_RUNTIME_EXPORT
unsigned int Kitware_mummy_Runtime_AddObjectRefCountChangedCallback(Kitware::mummy::Runtime::ObjectRefCountChangedCallback cb)
{
  return Kitware::mummy::Runtime::AddObjectRefCountChangedCallback(cb);
}
