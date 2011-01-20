FILE(TO_CMAKE_PATH "$ENV{WINDIR}" windir)
SET(framework_version "$ENV{FrameworkVersion}")

# Prefer v2 of the .NET Framework when both v2 and v3.5 are installed...:
# (older, wiser, more stable...)
#
SET(csc_search_paths
  "${windir}/Microsoft.NET/Framework/v2.0.50727"
  "${windir}/Microsoft.NET/Framework/v3.5"
  "${windir}/Microsoft.NET/Framework/v3.0"
  )

# ...if using the VS2008 generator, prefer v3.5:
#
IF(CMAKE_GENERATOR MATCHES "Visual Studio 9 2008")
  SET(csc_search_paths
    "${windir}/Microsoft.NET/Framework/v3.5"
    "${windir}/Microsoft.NET/Framework/v3.0"
    "${windir}/Microsoft.NET/Framework/v2.0.50727"
    )
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio 9 2008")

# ...if using the VS2010 generator, prefer v4.0:
#
IF(CMAKE_GENERATOR MATCHES "Visual Studio 10")
  SET(csc_search_paths
    "${windir}/Microsoft.NET/Framework/v4.0.30128"
    "${windir}/Microsoft.NET/Framework/v3.5"
    "${windir}/Microsoft.NET/Framework/v3.0"
    "${windir}/Microsoft.NET/Framework/v2.0.50727"
    )
ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio 10")

FIND_PROGRAM(csc_EXECUTABLE "csc"
  ${csc_search_paths}
  "${windir}/Microsoft.NET/Framework/${framework_version}"
  "${windir}/Microsoft.NET/Framework/v1.1.4322"
  "${windir}/Microsoft.NET/Framework/v1.0.3705"
  NO_DEFAULT_PATH
  )

IF(NOT csc_EXECUTABLE)
  FIND_PROGRAM(csc_EXECUTABLE NAMES "csc" "gmcs")
ENDIF(NOT csc_EXECUTABLE)

FIND_PROGRAM(mono_EXECUTABLE "mono")

GET_FILENAME_COMPONENT(csc_DIR "${csc_EXECUTABLE}" PATH)

SET(CSC_ACCEPTS_PLATFORM_FLAG 0)
SET(CSC_COMPILES_PARTIAL_CLASSES 0)

IF(csc_EXECUTABLE)
  EXECUTE_PROCESS(COMMAND "${csc_EXECUTABLE}" "/?" OUTPUT_VARIABLE CSC_HELP)

  IF("${CSC_HELP}" MATCHES "/platform:<string>")
    SET(CSC_ACCEPTS_PLATFORM_FLAG 1)
    SET(CSC_COMPILES_PARTIAL_CLASSES 1)
  ENDIF("${CSC_HELP}" MATCHES "/platform:<string>")
ENDIF(csc_EXECUTABLE)

IF(NOT DEFINED CSC_PLATFORM_FLAG)
  SET(CSC_PLATFORM_FLAG "")
  IF(CSC_ACCEPTS_PLATFORM_FLAG)
    SET(CSC_PLATFORM_FLAG "/platform:x86")
    IF("${CMAKE_SIZEOF_VOID_P}" GREATER 4)
      SET(CSC_PLATFORM_FLAG "/platform:x64")
    ENDIF("${CMAKE_SIZEOF_VOID_P}" GREATER 4)
  ENDIF(CSC_ACCEPTS_PLATFORM_FLAG)
ENDIF(NOT DEFINED CSC_PLATFORM_FLAG)

IF(NOT DEFINED CSC_DEBUG_FLAG)
  SET(CSC_DEBUG_FLAG "")
  IF("${CMAKE_BUILD_TYPE}" MATCHES "Debug|RelWithDebInfo")
    SET(CSC_DEBUG_FLAG "/debug")
  ENDIF("${CMAKE_BUILD_TYPE}" MATCHES "Debug|RelWithDebInfo")
  IF("${BUILDNAME}" MATCHES "Debug|RelWithDebInfo")
    SET(CSC_DEBUG_FLAG "/debug")
  ENDIF("${BUILDNAME}" MATCHES "Debug|RelWithDebInfo")
  IF("${CMAKE_BINARY_DIR}" MATCHES "Debug|RelWithDebInfo")
    SET(CSC_DEBUG_FLAG "/debug")
  ENDIF("${CMAKE_BINARY_DIR}" MATCHES "Debug|RelWithDebInfo")
ENDIF(NOT DEFINED CSC_DEBUG_FLAG)
