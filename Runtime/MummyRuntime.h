//----------------------------------------------------------------------------
// MummyRuntime.h
//
// Author(s) : David Cole
//
// Copyright (C) 2007 Kitware, Inc.
//----------------------------------------------------------------------------

#ifndef mummy_MummyRuntime_h
#define mummy_MummyRuntime_h


#include <typeinfo>


#ifdef _WIN32
#ifdef Kitware_mummy_Runtime_Unmanaged_EXPORTS
#define MUMMY_RUNTIME_EXPORT __declspec(dllexport)
#else
#define MUMMY_RUNTIME_EXPORT __declspec(dllimport)
#endif
#define MUMMY_RUNTIME_CC __stdcall
#endif

#ifndef MUMMY_RUNTIME_EXPORT
#define MUMMY_RUNTIME_EXPORT
#endif

#ifndef MUMMY_RUNTIME_CC
#define MUMMY_RUNTIME_CC
#endif


namespace Kitware
{
namespace mummy
{


class Runtime;
class RuntimeInternals;
class TypeEntry;
class TypeEntryInternals;


class MUMMY_RUNTIME_EXPORT Runtime
{
public:
  typedef unsigned int TypeEntryIndex;

  static TypeEntry* GetTypeEntry(void* obj, const char* classNameKey);

  static TypeEntry* LookupInstanceTypeEntry(void* obj);

  static TypeEntry* LookupClassTypeEntry(const char* classNameKey);

  static TypeEntry* GetTypeEntryByIndex(TypeEntryIndex index);

  static TypeEntryIndex GetTypeEntryCount();

  static const char* GetTypeEntryClassNameKey(TypeEntryIndex index);

  typedef void (MUMMY_RUNTIME_CC *ObjectRefCountChangedCallback)(void* obj, unsigned int oldCount, unsigned int newCount);

  static unsigned int AddObjectRefCountChangedCallback(ObjectRefCountChangedCallback cb);

  static void RemoveObjectRefCountChangedCallback(unsigned int cbid);

  static void MUMMY_RUNTIME_CC ObjectRefCountChanged(void* obj, unsigned int oldCount, unsigned int newCount);

  static const char* Print();

  static bool Verify();

private:
  static Runtime Instance;

  Runtime();
  virtual ~Runtime();

  RuntimeInternals* Internals;

  Runtime(const Runtime&);
  const Runtime& operator=(const Runtime&);
};


class MUMMY_RUNTIME_EXPORT TypeEntry
{
public:
  TypeEntry(const char* classNameKey, const Runtime::TypeEntryIndex& index);
  virtual ~TypeEntry();

  const char* GetClassNameKey() const;

  Runtime::TypeEntryIndex GetIndex() const;

  Runtime::ObjectRefCountChangedCallback GetObjectRefCountChangedCallback() const;

  const char* Print() const;

private:
  TypeEntryInternals* Internals;

  TypeEntry();
  TypeEntry(const TypeEntry&);
  const TypeEntry& operator=(const TypeEntry&);
};


}
}

#endif
