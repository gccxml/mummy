//----------------------------------------------------------------------------
// MummyGenerator.h
//
// Author(s) : David Cole
//
// Copyright (C) 2006-2007 Kitware, Inc.
//----------------------------------------------------------------------------

#ifndef _MummyGenerator_h_
#define _MummyGenerator_h_

#include "MummyUtilities.h" // first mummy include for root classes

#include "cableGenerator.h" // parent class

#include "cableContext.h" // enum Context::Access

#include "gxsys/ios/iosfwd"

namespace cable
{
  class Class;
  class Context;
  class FunctionType;
  class Method;
  class Type;
}

class MummyLineOrientedTextFileReader;
class MummySettings;

//----------------------------------------------------------------------------
/// <summary>
/// Base class for all Mummy generators.
/// </summary>
class MummyGenerator : public cable::Generator
{
public:
  MummyGenerator();
  virtual ~MummyGenerator();

  //--------------------------------------------------------------------------
  /// <summary>
  /// Get the associated settings object.
  /// </summary>
  /// <returns>Associated MummySettings object.</returns>
  virtual MummySettings* GetSettings();

  //--------------------------------------------------------------------------
  /// <summary>
  /// Set the associated settings object.
  /// </summary>
  /// <param name='settings'>MummySettings object</param>
  virtual void SetSettings(MummySettings* settings);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Get the current target class.
  /// </summary>
  /// <returns>Associated Class object.</returns>
  virtual const cable::Class* GetTargetClass();

  //--------------------------------------------------------------------------
  /// <summary>
  /// Set the current target class.
  /// </summary>
  /// <param name='c'>Class object</param>
  virtual void SetTargetClass(const cable::Class *c);

  virtual bool FundamentalTypeIsWrappable(const cable::Type* t);
  virtual bool TypeIsWrappable(const cable::Type* t);
  virtual bool FunctionTypeIsWrappable(const cable::FunctionType* ft);
  virtual bool MethodIsWrappable(const cable::Method* m, const cable::Context::Access& access);
  virtual bool ClassIsWrappable(const cable::Class* c);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Get or create a header file reader. Only uses class 'c' on the first
  /// call. Subsequent calls retrieve the same reader instance.
  /// </summary>
  /// <param name='c'>Class object</param>
  /// <returns>Header file reader</returns>
  virtual MummyLineOrientedTextFileReader* GetHeaderFileReader(const cable::Class* c);

  virtual void EmitMummyVersionComments(gxsys_ios::ostream &os, const char *lineCommentString);

private:
  MummySettings* Settings;
  const cable::Class* TargetClass;
  MummyLineOrientedTextFileReader* HeaderFileReader;

  MummyGenerator(const MummyGenerator&); //Not implemented
  MummyGenerator& operator=(const MummyGenerator&); //Not implemented
};

#endif
