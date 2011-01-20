//----------------------------------------------------------------------------
//
//  $Id: MummyLineOrientedTextFileReader.cxx 470 2009-06-12 17:43:02Z hoffman $
//
//  $Author: hoffman $
//  $Date: 2009-06-12 13:43:02 -0400 (Fri, 12 Jun 2009) $
//  $Revision: 470 $
//
//  Copyright (C) 2007 Kitware, Inc.
//
//----------------------------------------------------------------------------

#include "MummyLineOrientedTextFileReader.h"
#include "MummyLog.h"
#include "MummySettings.h"

#include "gxsys/RegularExpression.hxx"
#include "gxsys/ios/fstream"
#include "gxsys/ios/sstream"


//----------------------------------------------------------------------------
MummyLineOrientedTextFileReader::MummyLineOrientedTextFileReader()
{
  //this->FileName; // correctly constructed empty
  //this->Lines; // correctly constructed empty
  this->ExcludeMarkedLines = false;
  //this->BeginExcludeRegex; // correctly constructed empty
  //this->EndExcludeRegex; // correctly constructed empty
}


//----------------------------------------------------------------------------
MummyLineOrientedTextFileReader::~MummyLineOrientedTextFileReader()
{
}


//----------------------------------------------------------------------------
gxsys_stl::string MummyLineOrientedTextFileReader::GetFileName()
{
  return this->FileName;
}


//----------------------------------------------------------------------------
void MummyLineOrientedTextFileReader::SetFileName(const char *filename)
{
  if (filename)
    {
    this->FileName = filename;
    }
  else
    {
    this->FileName = "";
    }

  this->Update();
}


//----------------------------------------------------------------------------
bool MummyLineOrientedTextFileReader::GetExcludeMarkedLines()
{
  return this->ExcludeMarkedLines;
}


//----------------------------------------------------------------------------
void MummyLineOrientedTextFileReader::SetExcludeMarkedLines(bool excludeMarkedLines)
{
  this->ExcludeMarkedLines = excludeMarkedLines;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummyLineOrientedTextFileReader::GetBeginExcludeRegex()
{
  return this->BeginExcludeRegex;
}


//----------------------------------------------------------------------------
void MummyLineOrientedTextFileReader::SetBeginExcludeRegex(const gxsys_stl::string& beginExcludeRegex)
{
  this->BeginExcludeRegex = beginExcludeRegex;
}


//----------------------------------------------------------------------------
gxsys_stl::string MummyLineOrientedTextFileReader::GetEndExcludeRegex()
{
  return this->EndExcludeRegex;
}


//----------------------------------------------------------------------------
void MummyLineOrientedTextFileReader::SetEndExcludeRegex(const gxsys_stl::string& endExcludeRegex)
{
  this->EndExcludeRegex = endExcludeRegex;
}


//----------------------------------------------------------------------------
void MummyLineOrientedTextFileReader::Update()
{
  gxsys_ios::ifstream file(this->GetFileName().c_str());

  this->Lines.clear();

  if (file)
    {
    char line[4100];

    bool trackBtxEtxLevel = !this->GetBeginExcludeRegex().empty() &&
      !this->GetEndExcludeRegex().empty();
    int btxEtxLevel = 0;
    bool isLineComment = false;

    gxsys::RegularExpression reLineComment;
    reLineComment.compile("^[\\t ]*//");

    gxsys::RegularExpression reBTX;
    gxsys::RegularExpression reETX;
    if (trackBtxEtxLevel)
      {
      reBTX.compile(this->GetBeginExcludeRegex().c_str());
      reETX.compile(this->GetEndExcludeRegex().c_str());
      }
    //
    // Old hard-coded values were eerily similar to BTX/ETX regexes found in
    // the VTK source tree in Wrapping/vtkParse.l... ;)
    //
    //reBTX.compile("^[\\t ]*//BTX.*$");
    //reETX.compile("^[\\t ]*//ETX.*$");

    while (!file.eof())
      {
      line[0] = 0;
      file.getline(line, 4099);

      isLineComment = reLineComment.find(line);

      if (trackBtxEtxLevel && reBTX.find(line))
        {
        btxEtxLevel++;
        }

      this->Lines.push_back(LineData(line, isLineComment, btxEtxLevel));

      if (trackBtxEtxLevel && reETX.find(line))
        {
        btxEtxLevel--;
        }
      }
    }


#if 0
  // Dump it out with Trace to see what the settings and the output
  // data look like... This is a Print-method-in-waiting...
  //
  gxsys_stl::ostringstream oss;

  oss << "FileName: " << this->FileName << gxsys_stl::endl;
  oss << "ExcludeMarkedLines: " << this->ExcludeMarkedLines << gxsys_stl::endl;
  oss << "BeginExcludeRegex: " << this->BeginExcludeRegex << gxsys_stl::endl;
  oss << "EndExcludeRegex: " << this->EndExcludeRegex << gxsys_stl::endl;

  oss << "Lines:" << gxsys_stl::endl;

  unsigned int i = 1;
  gxsys_stl::vector<LineData>::iterator itLines;

  for (itLines = this->Lines.begin(); itLines != this->Lines.end();
    ++itLines)
    {
    LineData line = *itLines;

    // Comment?
    //
    if (line.IsLineComment)
      {
      oss << "C | ";
      }
    else
      {
      oss << "  | ";
      }

    // BTX/ETX level:
    //
    if (line.BtxEtxLevel)
      {

      oss << line.BtxEtxLevel;


      oss << " | ";
      }
    else
      {
      oss << "  | ";
      }

    // Line number, i - 4 digits with leading zeroes:
    //
    char f = oss.fill();
    gxsys_stl::streamsize w = oss.width();
    oss.fill('0');
    oss.width(4);
    oss << i;
    oss.fill(f);
    oss.width(w);

    // The line itself:
    //
    oss << ": " << line.Line << gxsys_stl::endl;

    ++i;
    }

  Trace(oss.str().c_str());
#endif
}


//----------------------------------------------------------------------------
unsigned int MummyLineOrientedTextFileReader::GetNumberOfLines()
{
  return static_cast<unsigned int>(this->Lines.size());
}


//----------------------------------------------------------------------------
gxsys_stl::string MummyLineOrientedTextFileReader::GetLine(unsigned int lineNumber)
{
  if (lineNumber<1 || lineNumber>this->GetNumberOfLines())
    {
    LogWarning(mw_NoSuchLineNumber, "lineNumber out of range in GetLine");
    return "";
    }

  return this->Lines.at(lineNumber-1).Line;
}


//----------------------------------------------------------------------------
bool MummyLineOrientedTextFileReader::GetIsLineComment(unsigned int lineNumber)
{
  if (lineNumber<1 || lineNumber>this->GetNumberOfLines())
    {
    LogWarning(mw_NoSuchLineNumber, "lineNumber out of range in GetIsLineComment");
    return false;
    }

  return this->Lines.at(lineNumber-1).IsLineComment;
}


//----------------------------------------------------------------------------
int MummyLineOrientedTextFileReader::GetBtxEtxLevel(unsigned int lineNumber)
{
  if (lineNumber<1 || lineNumber>this->GetNumberOfLines())
    {
    LogWarning(mw_NoSuchLineNumber, "lineNumber out of range in GetBtxEtxLevel");
    return 0;
    }

  return this->Lines.at(lineNumber-1).BtxEtxLevel;
}


//----------------------------------------------------------------------------
bool MummyLineOrientedTextFileReader::IsLineExcluded(unsigned int lineNumber)
{
  return this->GetExcludeMarkedLines() &&
    this->GetBtxEtxLevel(lineNumber) > 0;
}


//----------------------------------------------------------------------------
void MummyLineOrientedTextFileReader::GetCommentBlockBefore(unsigned int lineNumber, gxsys_stl::vector<gxsys_stl::string>& block, unsigned int smallestAcceptableLineNumber)
{
  if (lineNumber<2 || lineNumber>this->GetNumberOfLines())
    {
    LogWarning(mw_NoSuchLineNumber, "lineNumber out of range in GetCommentBlockBefore");
    return;
    }

  // We seek the comment block before a class or method declaration in a
  // header file. Like this line-numbered example from vtkObject.h:
  //
  //    52:
  //    53:  // Description:
  //    54:  // Create an object with Debug turned off, modified time initialized
  //    55:  // to zero, and reference counting on.
  //    56:  static vtkObject *New();
  //    57:
  //
  // If given lineNumber==56 as input, we should compute begin==53 and end==55
  // and return the comment block as a vector of line strings from
  // begin to end inclusive...

  unsigned int begin = 0;
  unsigned int end = 0;
  unsigned int i = lineNumber;

  // Find the first comment before lineNumber, saving its index in 'end'.
  // If no line comments occur before lineNumber, end will be 0.
  //
  while (0 == end && i>1)
    {
    i--;
    if (GetIsLineComment(i) && 0==GetBtxEtxLevel(i))
      {
      end = i;
      }
    }

  // Now find the first non-comment before end, saving the index of the comment
  // line *after* it in 'begin'.
  //
  if (0 != end)
    {
    while (0 == begin && i>1)
      {
      i--;
      if (!GetIsLineComment(i) && 0==GetBtxEtxLevel(i))
        {
        begin = i + 1;
        }
      }

    // Didn't find a non-comment line before 'end'... Comment block must start
    // right at line 1.
    //
    if (0 == begin)
      {
      begin = 1;
      }
    }

  // If end==0 then there are no comment lines prior to lineNumber.
  //
  if (0 == end)
    {
    LogFileLineWarningMsg(this->FileName.c_str(), lineNumber, mw_UndocumentedEntity,
      "No comment lines prior to line " << lineNumber << ". Undocumented class or method?");
    }
  else if (smallestAcceptableLineNumber > begin)
	{
	block.push_back(std::string("//Undocumented Block"));
	}
  else
    {
    gxsys_stl::string s;

    gxsys::RegularExpression reBeginsWithWhiteSpace;
    reBeginsWithWhiteSpace.compile("^([\\t ]*)[^\\t ].*");
    size_t from = 0;
    size_t to = 0;

    for (i= begin; i<=end; ++i)
	  {
      s = GetLine(i);

      if (reBeginsWithWhiteSpace.find(s))
        {
        from = reBeginsWithWhiteSpace.match(1).size();
        to = s.size() - from;
        s = s.substr(from, to);
        }

      block.push_back(s);
      }
    }
}


//----------------------------------------------------------------------------
void MummyLineOrientedTextFileReader::GetFirstCommentBlock(gxsys_stl::vector<gxsys_stl::string>& block)
{
  // Find the first non-comment line after the first comment line and use it
  // to call GetCommentBlockBefore...
  //
  unsigned int i = 1;
  unsigned int n = this->GetNumberOfLines();
  bool foundFirstComment = false;

  for (i= 1; i<=n; ++i)
    {
    if (this->GetIsLineComment(i) && 0==GetBtxEtxLevel(i))
      {
      foundFirstComment = true;
      }
    else if (foundFirstComment)
      {
	    this->GetCommentBlockBefore(i+1, block, 1);
  	  break;
      }
    }
}
