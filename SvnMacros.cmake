MACRO(SVN_IGNORE si_workingdir si_ignore)
  FIND_PROGRAM(svn_EXECUTABLE svn
    "C:/Program Files/Subversion/bin"
    "C:/Program Files (x86)/Subversion/bin"
    "C:/cygwin/bin"
    "/usr/bin"
    "/usr/local/bin"
    )
  IF(NOT svn_EXECUTABLE)
    MESSAGE(FATAL_ERROR "ERROR: svn is required to use the SVN_IGNORE CMake macro.")
  ENDIF(NOT svn_EXECUTABLE)
  MARK_AS_ADVANCED(svn_EXECUTABLE)

  EXECUTE_PROCESS(
    COMMAND "${svn_EXECUTABLE}"
      "propset" "svn:ignore" "${si_ignore}" "."
    WORKING_DIRECTORY "${si_workingdir}"
    RESULT_VARIABLE si_svn_rv
    )
  IF(NOT "${si_svn_rv}" STREQUAL "0")
    MESSAGE(SEND_ERROR "ERROR: svn propset command returned '${si_svn_rv}'")
  ENDIF(NOT "${si_svn_rv}" STREQUAL "0")
ENDMACRO(SVN_IGNORE)


MACRO(SVN_INFO si_workingdir si_svn_info_var)
  FIND_PROGRAM(svn_EXECUTABLE svn
    "C:/Program Files/Subversion/bin"
    "C:/Program Files (x86)/Subversion/bin"
    "C:/cygwin/bin"
    "/usr/bin"
    "/usr/local/bin"
    )
  IF(NOT svn_EXECUTABLE)
    MESSAGE(FATAL_ERROR "ERROR: svn is required to use the SVN_INFO CMake macro.")
  ENDIF(NOT svn_EXECUTABLE)
  MARK_AS_ADVANCED(svn_EXECUTABLE)

  EXECUTE_PROCESS(
    COMMAND "${svn_EXECUTABLE}"
      "info"
    WORKING_DIRECTORY "${si_workingdir}"
    OUTPUT_VARIABLE ${si_svn_info_var}
    RESULT_VARIABLE si_svn_rv
    )
  IF(NOT "${si_svn_rv}" STREQUAL "0")
    MESSAGE(SEND_ERROR "ERROR: svn info command returned '${si_svn_rv}'")
  ENDIF(NOT "${si_svn_rv}" STREQUAL "0")
ENDMACRO(SVN_INFO)


MACRO(SVN_STATUS si_workingdir si_svn_status_var)
  FIND_PROGRAM(svn_EXECUTABLE svn
    "C:/Program Files/Subversion/bin"
    "C:/Program Files (x86)/Subversion/bin"
    "C:/cygwin/bin"
    "/usr/bin"
    "/usr/local/bin"
    )
  IF(NOT svn_EXECUTABLE)
    MESSAGE(FATAL_ERROR "ERROR: svn is required to use the SVN_STATUS CMake macro.")
  ENDIF(NOT svn_EXECUTABLE)
  MARK_AS_ADVANCED(svn_EXECUTABLE)

  EXECUTE_PROCESS(
    COMMAND "${svn_EXECUTABLE}"
      "status"
    WORKING_DIRECTORY "${si_workingdir}"
    OUTPUT_VARIABLE ${si_svn_status_var}
    RESULT_VARIABLE si_svn_rv
    )
  IF(NOT "${si_svn_rv}" STREQUAL "0")
    MESSAGE(SEND_ERROR "ERROR: svn status command returned '${si_svn_rv}'")
  ENDIF(NOT "${si_svn_rv}" STREQUAL "0")
ENDMACRO(SVN_STATUS)
