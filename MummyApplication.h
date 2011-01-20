//----------------------------------------------------------------------------
// MummyApplication.h
//
// Author(s) : David Cole
//
// Copyright (C) 2006-2007 Kitware, Inc.
//----------------------------------------------------------------------------

#include "MummyUtilities.h" // first mummy include for root classes

namespace cable
{
  class Class;
  class SourceRepresentation;
}

class MummySettings;

//----------------------------------------------------------------------------
/// <summary>
/// Mummy's main entry point object.
/// </summary>
class MummyApplication
{
public:
  MummyApplication();
  virtual ~MummyApplication();


  //--------------------------------------------------------------------------
  /// <summary>
  /// "Command line style" entry point.
  /// Run "mummy --help" from a command prompt to see command line options.
  /// The console application simply instantiates a MummyApplication object
  /// and delegates to this method.
  /// </summary>
  /// <param name='argc'>The number of command line arguments in argv.</param>
  /// <param name='argv'>Array of command line arguments, indexed from 0 to argc-1.</param>
  /// <returns>Exit status. 0 means success.</returns>
  virtual int Main(int argc, char *argv[]);


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
  /// Builds an object model "source representation" from the input file
  /// produced by gccxml.
  /// </summary>
  /// <returns>The source representation.</returns>
  virtual cable::SourceRepresentation* BuildSourceRepresentation();


  //--------------------------------------------------------------------------
  /// <summary>
  /// Processes the source representation to initialize some settings and
  /// determine what class to wrap.
  /// </summary>
  /// <param name='sr'>A valid "source representation" object model.</param>
  /// <returns>Return status. 0 means success.</returns>
  virtual int ProcessSource(cable::SourceRepresentation* sr);


  //--------------------------------------------------------------------------
  /// <summary>
  /// Processes the class to wrap and generates appropriate output files based
  /// on the generators used.
  /// </summary>
  /// <param name='sr'>A valid "source representation" object model.</param>
  /// <param name='c'>The class to wrap.</param>
  /// <returns>Return status. 0 means success.</returns>
  virtual int ProcessClass(cable::SourceRepresentation* sr, const cable::Class* c);

private:
  MummySettings* Settings;
};
