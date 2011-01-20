//----------------------------------------------------------------------------
// MummyLineOrientedTextFileReader.h
//
// Author(s) : David Cole
//
// Copyright (C) 2007 Kitware, Inc.
//----------------------------------------------------------------------------

#ifndef _MummyLineOrientedTextFileReader_h_
#define _MummyLineOrientedTextFileReader_h_

#include "MummyUtilities.h" // first mummy include for root classes

#include "gxsys/stl/string"
#include "gxsys/stl/vector"


//----------------------------------------------------------------------------
/// <summary>
/// Internal, implementation detail class that caches information associated
/// with a line in a line oriented text file. Used only internally by
/// MummyLineOrientedTextFileReader.
/// </summary>
class LineData
{
public:
  LineData(const gxsys_stl::string &line, const bool isLineComment, const int btxEtxLevel) :
    Line(line), IsLineComment(isLineComment), BtxEtxLevel(btxEtxLevel)
  {
  }

  gxsys_stl::string Line;
  bool IsLineComment;
  int BtxEtxLevel;
};


//----------------------------------------------------------------------------
/// <summary>
/// Class that reads a text file and caches its lines and information about
/// those lines for quick queries based on line numbers. Primarily used to
/// look up code comment documentation blocks just prior to class and method
/// declarations in C++ header files. Initialize by calling SetFileName.
/// </summary>
class MummyLineOrientedTextFileReader
{
public:
  MummyLineOrientedTextFileReader();
  virtual ~MummyLineOrientedTextFileReader();

  //--------------------------------------------------------------------------
  /// <summary>
  /// Get the filename.
  /// </summary>
  /// <returns>Filename as a string.</returns>
  virtual gxsys_stl::string GetFileName();

  //--------------------------------------------------------------------------
  /// <summary>
  /// Set the filename.
  /// </summary>
  /// <param name='filename'>Filename</param>
  virtual void SetFileName(const char *filename);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Get whether to exclude lines between beginExcludeRegex and
  /// endExcludeRegex matching lines when considering lines for documentation.
  /// </summary>
  /// <returns>Exclude flag.</returns>
  virtual bool GetExcludeMarkedLines();

  //--------------------------------------------------------------------------
  /// <summary>
  /// Set whether to exclude lines between beginExcludeRegex and
  /// endExcludeRegex matching lines when considering lines for documentation.
  /// </summary>
  /// <param name='excludeMarkedLines'>Exclude flag.</param>
  virtual void SetExcludeMarkedLines(bool excludeMarkedLines);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Get the regular expression that delineates a line as the beginning of
  /// a "marked as excluded" block.
  /// </summary>
  /// <returns>"Begin excluding" regular expression</returns>
  virtual gxsys_stl::string GetBeginExcludeRegex();

  //--------------------------------------------------------------------------
  /// <summary>
  /// Set the regular expression that delineates a line as the beginning of
  /// a "marked as excluded" block.
  /// </summary>
  /// <param name='beginExcludeRegex'>"Begin excluding" regular
  /// expression
  /// </param>
  virtual void SetBeginExcludeRegex(const gxsys_stl::string& beginExcludeRegex);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Get the regular expression that delineates a line as the end of
  /// a "marked as excluded" block.
  /// </summary>
  /// <returns>"End excluding" regular expression</returns>
  virtual gxsys_stl::string GetEndExcludeRegex();

  //--------------------------------------------------------------------------
  /// <summary>
  /// Set the regular expression that delineates a line as the end of
  /// a "marked as excluded" block.
  /// </summary>
  /// <param name='endExcludeRegex'>"End excluding" regular
  /// expression
  /// </param>
  virtual void SetEndExcludeRegex(const gxsys_stl::string& endExcludeRegex);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Read the current text file and cache its lines for subsequent quick
  /// lookup by line number.
  /// This allows us to retrieve the
  /// comment blocks just prior to a given line number and allows us to parse
  /// and/or transfer the documentation from the source code to the generated
  /// code.
  /// </summary>
  virtual void Update();

  //--------------------------------------------------------------------------
  /// <summary>
  /// Retrieve the total number of lines currently cached.
  /// </summary>
  /// <returns>The total number of lines.</returns>
  virtual unsigned int GetNumberOfLines();

  //--------------------------------------------------------------------------
  /// <summary>
  /// Retrieve line number 'lineNumber' as a string from the currently cached
  /// text file.
  /// Valid 'lineNumber' values are 1 through GetNumberOfLines inclusive.
  /// </summary>
  /// <param name='lineNumber'>1-based line number</param>
  /// <returns>The line as a string. Empty string if lineNumber is invalid.</returns>
  virtual gxsys_stl::string GetLine(unsigned int lineNumber);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Query if line number 'lineNumber' is a "line comment."
  /// Valid 'lineNumber' values are 1 through GetNumberOfLines inclusive.
  /// </summary>
  /// <param name='lineNumber'>1-based line number</param>
  /// <returns>Whether the line is a comment. false if lineNumber is invalid.</returns>
  virtual bool GetIsLineComment(unsigned int lineNumber);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Get the current nesting level at line number 'lineNumber' of "//BTX -
  /// //ETX" style wrapper exclusion comments. This should be exactly 0 or 1
  /// because the old VTK wrappers start ignoring at //BTX and do not resume
  /// wrapping until the closing //ETX occurs.
  /// Valid 'lineNumber' values are 1 through GetNumberOfLines inclusive.
  /// </summary>
  /// <param name='lineNumber'>1-based line number</param>
  /// <returns>Whether the line is in between BTX and ETX markers. 0 if lineNumber is invalid.</returns>
  virtual int GetBtxEtxLevel(unsigned int lineNumber);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Query whether the given line number should be excluded based on the
  /// exclude flag and the line number's BtxEtx level.
  /// Valid 'lineNumber' values are 1 through GetNumberOfLines inclusive.
  /// </summary>
  /// <param name='lineNumber'>1-based line number</param>
  /// <returns>Whether the line should be excluded.</returns>
  virtual bool IsLineExcluded(unsigned int lineNumber);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Retrieve the nearest preceding block of comment lines relative to line
  /// number 'lineNumber'.
  /// Valid 'lineNumber' values are 2 through GetNumberOfLines inclusive.
  /// </summary>
  /// <param name='lineNumber'>1-based line number</param>
  /// <param name='block'>vector of strings, one per line</param>
  /// <param name='smallestAcceptableLineNumber'>if the comment block happens before this line an Undocumented Block is returned</param>
 
  virtual void GetCommentBlockBefore(unsigned int lineNumber, gxsys_stl::vector<gxsys_stl::string>& block, unsigned int smallestAcceptableLineNumber);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Retrieve the first block of comment lines, if any, in the file.
  /// </summary>
  /// <param name='block'>vector of strings, one per line</param>
  virtual void GetFirstCommentBlock(gxsys_stl::vector<gxsys_stl::string>& block);

private:
  gxsys_stl::string FileName;
  gxsys_stl::vector<LineData> Lines;
  bool ExcludeMarkedLines;
  gxsys_stl::string BeginExcludeRegex;
  gxsys_stl::string EndExcludeRegex;
};

#endif // _MummyLineOrientedTextFileReader_h_
