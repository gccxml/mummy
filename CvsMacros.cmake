MACRO(GET_CVS_SNAPSHOT gcs_workingdir gcs_cvsroot gcs_cvsmodule gcs_cvsrevisionargs)
  FIND_PROGRAM(gcs_cvs_EXECUTABLE cvs
    "C:/Program Files/CVSNT"
    "C:/Program Files (x86)/CVSNT"
    "C:/Program Files/TortoiseCVS"
    "C:/Program Files (x86)/TortoiseCVS"
    "C:/cygwin/bin"
    "/usr/bin"
    "/usr/local/bin"
    )
  IF(NOT gcs_cvs_EXECUTABLE)
    MESSAGE(FATAL_ERROR "error: cvs is required to retrieve a ${gcs_cvsmodule} snapshot. Please set gcs_cvs_EXECUTABLE to a working copy of cvs.")
  ENDIF(NOT gcs_cvs_EXECUTABLE)
  MARK_AS_ADVANCED(gcs_cvs_EXECUTABLE)

  IF(EXISTS "${gcs_workingdir}/${gcs_cvsmodule}/CVS/Root")

    MESSAGE(STATUS "Updating ${gcs_cvsmodule} snapshot with '${gcs_cvs_EXECUTABLE}'...")
    SET(gcs_cvsargs "-d" "${gcs_cvsroot}" "up" "-dAP" ${gcs_cvsrevisionargs})
    FILE(APPEND "${gcs_workingdir}/${gcs_cvsmodule}/CMakeFiles/cvs-up-args.txt" "gcs_cvsargs='${gcs_cvsargs}'\n")
    EXECUTE_PROCESS(
      COMMAND "${gcs_cvs_EXECUTABLE}" ${gcs_cvsargs}
      WORKING_DIRECTORY "${gcs_workingdir}/${gcs_cvsmodule}"
      RESULT_VARIABLE gcs_cvs_rv
      )
    MESSAGE(STATUS "Updating ${gcs_cvsmodule} snapshot with '${gcs_cvs_EXECUTABLE}'... -- done")

  ELSE(EXISTS "${gcs_workingdir}/${gcs_cvsmodule}/CVS/Root")

    MESSAGE(STATUS "Checking out ${gcs_cvsmodule} snapshot with '${gcs_cvs_EXECUTABLE}'...          (This may take a few minutes...)")
    SET(gcs_cvsargs "-d" "${gcs_cvsroot}" "co" ${gcs_cvsrevisionargs} "${gcs_cvsmodule}")
    FILE(WRITE "${gcs_workingdir}/${gcs_cvsmodule}/CMakeFiles/cvs-co-args.txt" "gcs_cvsargs='${gcs_cvsargs}'\n")
    EXECUTE_PROCESS(
      COMMAND "${gcs_cvs_EXECUTABLE}" ${gcs_cvsargs}
      WORKING_DIRECTORY "${gcs_workingdir}"
      RESULT_VARIABLE gcs_cvs_rv
      )
    MESSAGE(STATUS "Checking out ${gcs_cvsmodule} snapshot with '${gcs_cvs_EXECUTABLE}'... -- done")

  ENDIF(EXISTS "${gcs_workingdir}/${gcs_cvsmodule}/CVS/Root")

  IF(NOT "${gcs_cvs_rv}" STREQUAL "0")
    MESSAGE(SEND_ERROR "error: cvs command returned '${gcs_cvs_rv}'")
  ENDIF(NOT "${gcs_cvs_rv}" STREQUAL "0")
ENDMACRO(GET_CVS_SNAPSHOT)
