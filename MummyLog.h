//----------------------------------------------------------------------------
// MummyLog.h
//
// Author(s) : David Cole
//
// Copyright (C) 2006-2009 Kitware, Inc.
//----------------------------------------------------------------------------

#ifndef MummyLog_h
#define MummyLog_h

#include "MummyUtilities.h" // first mummy include for root classes

#include "gxsys/ios/iostream"

/// <summary>
/// Log the message value via the global utility function LogMsg and emit it
/// to the given stream.
/// </summary>
#define LogFileLineLabelledMsg(os, file, line, label, n, m) \
  if (ShouldLogMsg(n)) \
    { \
    LogMsg(file, line, label, n); \
    os << file << "(" << line << "): " << label << ": m" << n << ": " m << gxsys_ios::endl; \
    }


#define LogFileLineErrorMsg(file, line, n, m) \
  LogFileLineLabelledMsg(gxsys_ios::cerr, file, line, "error", n, m)

#define LogFileLineWarningMsg(file, line, n, m) \
  LogFileLineLabelledMsg(gxsys_ios::cerr, file, line, "warning", n, m)

#define LogFileLineInfoMsg(file, line, n, m) \
  LogFileLineLabelledMsg(gxsys_ios::cout, file, line, "info", n, m)


// n == number
// m == (C++ stream-able) message or string literal
//
#define LogError(n, m) \
  LogFileLineErrorMsg("# ", 0, n, m)

#define LogWarning(n, m) \
  LogFileLineWarningMsg("# ", 0, n, m)

#define LogInfo(n, m) \
  LogFileLineInfoMsg("# ", 0, n, m)


#define LogVerboseInfo(m) \
  if (this->GetSettings()->GetVerbose()) \
    { \
    LogInfo(mi_VerboseInfo, m); \
    }


// Error (me_), warning (mw_) and info (mi_) values.
//
// Add new error values to the end of the me_ segment, new warnings to the
// end of the mw_ segment and new info values to the end of the mi_ segment.
// That way, new builds of mummy will always have the same error and warning
// values as previous builds...
//
// DO NOT be tempted to alphabetize this list. Or, if you are tempted, resist
// it with all that is in you... Keep this list ordered the same within each
// segment - always!
//
enum MummyLogMsgValues
{
  // Error values segment:
  me_ErrorValueBase = 5000,
  me_InternalError,
  me_CouldNotOpen,
  me_CouldNotParse,
  me_CouldNotValidate,
  me_CouldNotWrap,
  me_MissingRequiredCommandLineArg,
  me_UnexpectedGccxmlInput,
  me_InvalidArg,
  me_MoreThanOneBaseClass,
  me_UnknownFundamentalType,
  me_NoClassWrappingSettings,
  me_EventMethodIncorrectReturnType,
  me_NoVirtualMethodsAllowed,
  me_UnknownMapToType,
  me_PureVirtualMethodNotAllowed,

  // Warning values segment:
  mw_WarningValueBase = 6000,
  mw_InternalWarning,
  mw_NoSuchLineNumber,
  mw_UndocumentedEntity,
  mw_DuplicateGeneratedName,
  mw_ReservedMethodName,
  mw_UnnamedEnum,
  mw_MultipleTargetInterfaces,
  mw_CouldNotWrap,
  mw_PropGetReturnsVoid,
  mw_PropGetNotConst,
  mw_PropSetReturnsNonVoid,
  mw_PropSetUnexpectedArgCount,
  mw_DefaultArgumentValuesIgnored,
  mw_WriteOnlyProperty,
  mw_MissingPropGetHint,
  mw_MissingPropSetHint,
  mw_PropGetHasArgs,
  mw_SeriousMissingPropGetHint,
  mw_SeriousMissingPropSetHint,
  mw_UnknownAttribute,
  mw_MultipleHints,
  mw_UnknownHintDataType,

  // Info values segment:
  mi_InfoValueBase = 7000,
  mi_Info,
  mi_VerboseInfo,
  mi_InfoRefArgEncountered,

  // Always keep this one last:
  //
  me_LastMummyLogMsgValue
};

#endif
