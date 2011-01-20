PROJECT(MummyRuntime)


SET(MummyRC_RC_FILE_NAME "MummyRuntime.rc")
SET(MummyRC_FILE_DESCRIPTION "${Mummy_FULL_VERSION_STRING} - mummy runtime dll")
SET(MummyRC_ORIGINAL_FILENAME "${CMAKE_SHARED_LIBRARY_PREFIX}Kitware.mummy.Runtime.Unmanaged${CMAKE_SHARED_LIBRARY_SUFFIX}")
CONFIGURE_FILE(
  "${Mummy_SOURCE_DIR}/MummyVersion.rc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${MummyRC_RC_FILE_NAME}"
  @ONLY
  )


ADD_LIBRARY(
  Kitware.mummy.Runtime.Unmanaged SHARED
  MummyRuntime.cxx
  MummyRuntime.h
  MummyTypeEntry.cxx
  MummyTypeEntry.h
  "${CMAKE_CURRENT_BINARY_DIR}/MummyRuntime.rc"
  )

INSTALL(TARGETS
  Kitware.mummy.Runtime.Unmanaged
  DESTINATION "bin"
  )


# This string should be the same string as the C++ return value from this code:
#   MRSmokeTest *test;
#   return typeid(*test).name();
#
SET(MRSMOKETEST_CLASS_NAME_KEY "ERROR_unknown_MRSmokeTest_class_name_key")
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(MRSMOKETEST_CLASS_NAME_KEY "11MRSmokeTest")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
IF(MSVC)
  SET(MRSMOKETEST_CLASS_NAME_KEY "class MRSmokeTest")
ENDIF(MSVC)


CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/AssemblyInfo.cs.in"
  "${CMAKE_CURRENT_BINARY_DIR}/AssemblyInfo.cs"
  @ONLY
  )

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/MummyRuntime.cs.in"
  "${CMAKE_CURRENT_BINARY_DIR}/MummyRuntime.cs"
  @ONLY
  )

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/MRSmokeTest.cs.in"
  "${CMAKE_CURRENT_BINARY_DIR}/MRSmokeTest.cs"
  @ONLY
  )


# If a real strong name signing keyfile has not been predefined,
# then use the public key file in the source tree to /delaysign
# .NET assemblies...
#
IF(NOT Mummy_SNKEYFILE)
  GET_FILENAME_COMPONENT(Mummy_SNKEYFILE "${CMAKE_CURRENT_SOURCE_DIR}/Kitware.mummy.Runtime.pub.snk" ABSOLUTE)
ENDIF(NOT Mummy_SNKEYFILE)
MESSAGE(STATUS "Mummy_SNKEYFILE='${Mummy_SNKEYFILE}'")


ADD_CSHARP_LIBRARY(
  "Kitware.mummy.Runtime" # name of library
  "Kitware.mummy.Runtime.Unmanaged" # list of CMake targets that need to build first
  "" # list of csc "/reference:" args
  "${CMAKE_SHARED_LIBRARY_PREFIX}Kitware.mummy.Runtime.Unmanaged${CMAKE_SHARED_LIBRARY_SUFFIX}" # list of csc "/linkresource:" args
  "${Mummy_SNKEYFILE}" # strong name signing keyfile
  # Source files:
  "${CMAKE_CURRENT_BINARY_DIR}/AssemblyInfo.cs"
  "${CMAKE_CURRENT_BINARY_DIR}/MummyRuntime.cs"
  "${CMAKE_CURRENT_SOURCE_DIR}/WrappedException.cs"
  "${CMAKE_CURRENT_SOURCE_DIR}/WrappedObject.cs"
  )


ADD_LIBRARY(MRSmokeTest SHARED MRSmokeTest.cxx)
TARGET_LINK_LIBRARIES(MRSmokeTest Kitware.mummy.Runtime.Unmanaged)


ADD_EXECUTABLE(MRSmokeTestMain MRSmokeTestMain.cxx)
TARGET_LINK_LIBRARIES(MRSmokeTestMain MRSmokeTest)


ADD_CSHARP_LIBRARY(
  "Kitware.mummy.Runtime.SmokeTest"
  "Kitware.mummy.Runtime;MRSmokeTest"
  "Kitware.mummy.Runtime"
  ""
  ""
  "${CMAKE_CURRENT_BINARY_DIR}/MRSmokeTest.cs"
  )


ADD_CSHARP_EXECUTABLE(
  "Kitware.mummy.Runtime.SmokeTestMain"
  "Kitware.mummy.Runtime.SmokeTest"
  "Kitware.mummy.Runtime.SmokeTest;Kitware.mummy.Runtime"
  ""
  ""
  "${CMAKE_CURRENT_SOURCE_DIR}/MRSmokeTestMain.cs"
  )


ADD_CSHARP_EXECUTABLE(
  "Kitware.mummy.Runtime.CreateTestMain"
  "Kitware.mummy.Runtime.SmokeTest"
  "Kitware.mummy.Runtime.SmokeTest;Kitware.mummy.Runtime"
  ""
  ""
  "${CMAKE_CURRENT_SOURCE_DIR}/CreateTestMain.cs"
  )


#MESSAGE(STATUS "Mummy_BUILD_TESTING='${Mummy_BUILD_TESTING}'")
IF(Mummy_BUILD_TESTING)
  SET(testexe MRSmokeTestMain)
  IF(EXECUTABLE_OUTPUT_PATH)
    SET(exefile ${EXECUTABLE_OUTPUT_PATH}/${Mummy_TESTING_CONFIG_PREFIX}${testexe})
  ELSE(EXECUTABLE_OUTPUT_PATH)
    SET(exefile ${testexe})
  ENDIF(EXECUTABLE_OUTPUT_PATH)
  ADD_TEST(mummyRuntimeTest-${testexe}-NoArgs "${exefile}")
  ADD_TEST(mummyRuntimeTest-${testexe} "${exefile}" "20000")
  IF(Mummy_RUN_LONG_TESTS)
    ADD_TEST(mummyRuntimeTest-${testexe}-mega "${exefile}" "1000000")
  ENDIF(Mummy_RUN_LONG_TESTS)

  IF(csc_EXECUTABLE)
    #
    # C# executables are named with ".exe" even on Mono (Linux / Mac)...
    #
    SET(testexe Kitware.mummy.Runtime.SmokeTestMain.exe)
    IF(EXECUTABLE_OUTPUT_PATH)
      SET(exefile ${EXECUTABLE_OUTPUT_PATH}/${Mummy_TESTING_CONFIG_PREFIX}${testexe})
    ELSE(EXECUTABLE_OUTPUT_PATH)
      SET(exefile ${testexe})
    ENDIF(EXECUTABLE_OUTPUT_PATH)
    ADD_CSHARP_TEST(mummyRuntimeTest-${testexe}-NoArgs "${exefile}")
    FOREACH(flag strong weak evensplit pseudorandom)
      ADD_CSHARP_TEST(mummyRuntimeTest-${testexe}-${flag} "${exefile}" "20000" "--${flag}")
      IF(Mummy_RUN_LONG_TESTS)
        ADD_CSHARP_TEST(mummyRuntimeTest-${testexe}-${flag}-mega "${exefile}" "1000000" "--${flag}")
      ENDIF(Mummy_RUN_LONG_TESTS)
    ENDFOREACH(flag)

    SET(testexe Kitware.mummy.Runtime.CreateTestMain.exe)
    IF(EXECUTABLE_OUTPUT_PATH)
      SET(exefile ${EXECUTABLE_OUTPUT_PATH}/${Mummy_TESTING_CONFIG_PREFIX}${testexe})
    ELSE(EXECUTABLE_OUTPUT_PATH)
      SET(exefile ${testexe})
    ENDIF(EXECUTABLE_OUTPUT_PATH)
    ADD_CSHARP_TEST(mummyRuntimeTest-${testexe}-NoArgs "${exefile}")
    ADD_CSHARP_TEST(mummyRuntimeTest-${testexe} "${exefile}" "20000")
    IF(Mummy_RUN_LONG_TESTS)
      ADD_CSHARP_TEST(mummyRuntimeTest-${testexe}-mega "${exefile}" "1000000")
    ENDIF(Mummy_RUN_LONG_TESTS)
  ENDIF(csc_EXECUTABLE)
ENDIF(Mummy_BUILD_TESTING)
