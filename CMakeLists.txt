CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3 FATAL_ERROR)
PROJECT(Mummy)

# Determine if we're building Mummy as a "top level" project.
#
SET(Mummy_BUILD_IS_TOP_LEVEL 0)
IF("${Mummy_BINARY_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  SET(Mummy_BUILD_IS_TOP_LEVEL 1)
ENDIF("${Mummy_BINARY_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")

# Version number and default installation location:
#
SET(Mummy_VERSION_MAJOR 1)
SET(Mummy_VERSION_MINOR 0)
SET(Mummy_VERSION_PATCH 3)
SET(Mummy_VERSION_STRING "${Mummy_VERSION_MAJOR}.${Mummy_VERSION_MINOR}.${Mummy_VERSION_PATCH}")
IF(Mummy_BUILD_IS_TOP_LEVEL)
  SET(Mummy_INSTALL_PREFIX "${CMAKE_BINARY_DIR} Install" CACHE STRING "")
  SET(CMAKE_INSTALL_PREFIX "${Mummy_INSTALL_PREFIX}" CACHE INTERNAL "" FORCE)
ENDIF(Mummy_BUILD_IS_TOP_LEVEL)

# Source control system keyword substitutions (from the last
# time *this* file was committed):
#
SET(Mummy_SCS_Author "$Author: david.cole $")
SET(Mummy_SCS_Date "$Date: 2011-01-27 $")
SET(Mummy_SCS_Id "$Id: CMakeLists.txt $")
SET(Mummy_SCS_Revision "$Revision: 599 $")
SET(Mummy_SCS_URL "$URL: git://public.kitware.com/mummy.git $")

# Use these settings when building with the Msvc compiler:
#
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/MsvcMacros.cmake")
MSVC_LINK_TO_STATIC_CRT()
MSVC80_FORCE_MANIFEST_LINKER_FLAG()
MSVC_SUPPRESS_CRT_DEPRECATED_WARNINGS()

# All exes and libs should end up in the "bin" directory:
#
IF(NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin" CACHE INTERNAL
    "Single output directory for building all executables.")
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)

IF(NOT LIBRARY_OUTPUT_PATH)
  SET(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin" CACHE INTERNAL
    "Single output directory for building all libraries.")
ENDIF(NOT LIBRARY_OUTPUT_PATH)

# Name of the library that clients need to link to for building "export layer"
# code that links to the mummy runtime library. This values gets configured
# into the MummyConfig variable Mummy_RUNTIME_LINK_LIBRARIES. Clients should
# link to it with:
#
#   target_link_libraries(ClientExportLayerLib ${Mummy_RUNTIME_LINK_LIBRARIES})
#
IF(WIN32)
  SET(Mummy_RUNTIME_UNMANAGED_LINK_LIBRARY
    "Kitware.mummy.Runtime.Unmanaged.lib")
ELSE()
  SET(Mummy_RUNTIME_UNMANAGED_LINK_LIBRARY
    "libKitware.mummy.Runtime.Unmanaged.so")
ENDIF()

# Sort configuration types according to the order we'd like to find them in
# if multiple ones exist in the build tree and we're looking at mummy from
# an external project that is trying to use it.
#
IF(CMAKE_CONFIGURATION_TYPES)
  SET(
    Mummy_CMAKE_CONFIGURATION_TYPES
    "Release"
    "MinSizeRel"
    "RelWithDebInfo"
    "Debug"
    )
ELSE(CMAKE_CONFIGURATION_TYPES)
  SET(Mummy_CMAKE_CONFIGURATION_TYPES)
ENDIF(CMAKE_CONFIGURATION_TYPES)

# 32-bit build? (CableSwig, gccxml and mummy.exe only get built for 32-bit builds.
# The mummy Runtime components get built for both 32-bit and 64-bit builds.)
#
IF(NOT DEFINED Mummy_BUILD_COMPONENT_CABLESWIG)
  SET(Mummy_BUILD_COMPONENT_CABLESWIG 1)
ENDIF(NOT DEFINED Mummy_BUILD_COMPONENT_CABLESWIG)

IF(NOT DEFINED Mummy_BUILD_COMPONENT_MUMMY)
  SET(Mummy_BUILD_COMPONENT_MUMMY 1)
ENDIF(NOT DEFINED Mummy_BUILD_COMPONENT_MUMMY)

IF(NOT DEFINED Mummy_BUILD_COMPONENT_MUMMY_RUNTIME)
  SET(Mummy_BUILD_COMPONENT_MUMMY_RUNTIME 1)
ENDIF(NOT DEFINED Mummy_BUILD_COMPONENT_MUMMY_RUNTIME)

SET(Mummy_32_BIT 1)

IF("${CMAKE_GENERATOR}" MATCHES " Win64$")
  SET(Mummy_32_BIT 0)
ENDIF("${CMAKE_GENERATOR}" MATCHES " Win64$")

IF("${CMAKE_SIZEOF_VOID_P}" GREATER 4)
  SET(Mummy_32_BIT 0)
ENDIF("${CMAKE_SIZEOF_VOID_P}" GREATER 4)

IF(NOT Mummy_32_BIT)
  IF(Mummy_64_JUST_BUILD_RUNTIME)
    SET(Mummy_BUILD_COMPONENT_CABLESWIG 0)
    SET(Mummy_BUILD_COMPONENT_MUMMY 0)
  ENDIF(Mummy_64_JUST_BUILD_RUNTIME)
ENDIF(NOT Mummy_32_BIT)

# Provide mummy and C# settings at the top level so runtime, test and
# example sub-projects can use them:
#
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/CvsMacros.cmake")
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/FindCsharp.cmake")
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/MummyCMakeMacros.cmake")
INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/SvnMacros.cmake")

SET(mummy_EXECUTABLE "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/mummy${CMAKE_EXECUTABLE_SUFFIX}")

# If the parent dir is named exactly "ActiViz" then create external source
# trees as siblings of "ActiViz" - otherwise create them as siblings of
# "${CMAKE_CURRENT_SOURCE_DIR}"
#
GET_FILENAME_COMPONENT(Mummy_PARENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" PATH)
SET(Mummy_REL_EXPR "..")
IF("${Mummy_PARENT_DIR}" MATCHES "/ActiViz$")
  SET(Mummy_REL_EXPR "../..")
ENDIF("${Mummy_PARENT_DIR}" MATCHES "/ActiViz$")
GET_FILENAME_COMPONENT(Mummy_EXTERNAL_SOURCE_TREES_DIR
  "${CMAKE_CURRENT_SOURCE_DIR}/${Mummy_REL_EXPR}/mummyExternalSourceTrees" ABSOLUTE)

SET(Mummy_CableSwig_CVS_REVISION "-D;2009-06-12 20:00:00 UTC")
SET(Mummy_CableSwig_PARENT_DIR "${Mummy_EXTERNAL_SOURCE_TREES_DIR}")
SET(Mummy_CableSwig_SOURCE_DIR "${Mummy_CableSwig_PARENT_DIR}/CableSwig")
SET(Mummy_CableSwig_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/CableSwigBuild")

# Echo selected CMake/configuration settings:
#
FOREACH(v
  CMAKE_SIZEOF_VOID_P
  Mummy_32_BIT
  Mummy_BUILD_COMPONENT_CABLESWIG
  Mummy_BUILD_COMPONENT_MUMMY
  Mummy_BUILD_COMPONENT_MUMMY_RUNTIME
  Mummy_CableSwig_CVS_REVISION
  Mummy_EXTERNAL_SOURCE_TREES_DIR
  Mummy_TESTING_CONFIG_PREFIX
  )
  MESSAGE(STATUS "${v}='${${v}}'")
ENDFOREACH(v)

SET(Mummy_SOURCE_TREE_IS_SVN_CHECKOUT 0)
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.svn")
  SET(Mummy_SOURCE_TREE_IS_SVN_CHECKOUT 1)
ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.svn")

# Retrieve current state of the source tree according to svn,
# if this source tree is an svn checkout. Otherwise, status
# is just empty...
#
SET(Mummy_SVN_STATUS "")
IF(Mummy_SOURCE_TREE_IS_SVN_CHECKOUT)
  SVN_STATUS("${CMAKE_CURRENT_SOURCE_DIR}" Mummy_SVN_STATUS)
ENDIF(Mummy_SOURCE_TREE_IS_SVN_CHECKOUT)

IF("${Mummy_SVN_STATUS}" STREQUAL "")
  SET(Mummy_SVN_STATUS_BRIEF "")
  SET(Mummy_SVN_STATUS_ENCODED "")
ELSE("${Mummy_SVN_STATUS}" STREQUAL "")
  SET(Mummy_SVN_STATUS_BRIEF " [warning: experimental development build!]")
  STRING(REGEX REPLACE "\\\\" "/" Mummy_SVN_STATUS_ENCODED "${Mummy_SVN_STATUS}")
  STRING(REGEX REPLACE "\n" "\\\\n" Mummy_SVN_STATUS_ENCODED "${Mummy_SVN_STATUS_ENCODED}")
  STRING(REGEX REPLACE "\"" "\\\\\"" Mummy_SVN_STATUS_ENCODED "${Mummy_SVN_STATUS_ENCODED}")
ENDIF("${Mummy_SVN_STATUS}" STREQUAL "")

# Pull in FindSubversion.cmake to get the Subversion_WC_INFO macro:
#

SET(Mummy_VERSION_SVN_REVISION "599")
  #
  # This value is an artificial number that is simply incremented manually
  # now after switching to git from svn... Whenever we do "real packaging
  # releases" this number should be incremented manually until this code
  # is updated to reflect our shiny, new git repo-ness.

IF(Mummy_SOURCE_TREE_IS_SVN_CHECKOUT)
  SET(Subversion_SVN_EXECUTABLE "${svn_EXECUTABLE}")
  FIND_PACKAGE(Subversion)
  IF(Subversion_FOUND)
    Subversion_WC_INFO("${Mummy_SOURCE_DIR}" Mummy)
    SET(Mummy_VERSION_SVN_REVISION "${Mummy_WC_LAST_CHANGED_REV}")

    # Now the following variables are available for use:
  #  MESSAGE("Subversion_VERSION_SVN='${Subversion_VERSION_SVN}'")
  #  MESSAGE("Subversion_LAST_CHANGED_LOG='${Subversion_LAST_CHANGED_LOG}'")
  #  MESSAGE("Mummy_WC_INFO='${Mummy_WC_INFO}'")
  #  MESSAGE("Mummy_WC_URL='${Mummy_WC_URL}'")
  #  MESSAGE("Mummy_WC_REVISION='${Mummy_WC_REVISION}'")
  #  MESSAGE("Mummy_WC_LAST_CHANGED_AUTHOR='${Mummy_WC_LAST_CHANGED_AUTHOR}'")
  #  MESSAGE("Mummy_WC_LAST_CHANGED_REV='${Mummy_WC_LAST_CHANGED_REV}'")
  #  MESSAGE("Mummy_WC_LAST_CHANGED_DATE='${Mummy_WC_LAST_CHANGED_DATE}'")
  ENDIF(Subversion_FOUND)
ENDIF(Mummy_SOURCE_TREE_IS_SVN_CHECKOUT)

# After getting svn info and status we can set Mummy_FULL_VERSION_STRING:
#
SET(Mummy_FULL_VERSION_STRING "mummy version ${Mummy_VERSION_STRING} (revision ${Mummy_VERSION_SVN_REVISION})${Mummy_SVN_STATUS_BRIEF}")


# Settings required to use the Cable and gxsys libs:
#
SET(Mummy_include_dirs
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${Mummy_CableSwig_SOURCE_DIR}"
  "${Mummy_CableSwig_SOURCE_DIR}/Cable"
  "${Mummy_CableSwig_BINARY_DIR}/Cable"
  "${Mummy_CableSwig_SOURCE_DIR}/Cable/CxxTypes"
  "${Mummy_CableSwig_SOURCE_DIR}/Cable/Generators"
  "${Mummy_CableSwig_SOURCE_DIR}/Cable/Parsers"
  "${Mummy_CableSwig_BINARY_DIR}/GCC_XML"
  )

INCLUDE_DIRECTORIES(${Mummy_include_dirs})

# Always true - see Cable/CxxTypes/CMakeLists.txt
#
ADD_DEFINITIONS(-D_cxx_STATIC)

# Also, do not install any Cable headers with mummy installation
# (also in Cable/CxxTypes/CMakeLists.txt)
#
SET(CABLE_NO_INSTALL_FACILITIES 1)

# "Near copy" of kwsys related settings from CableSwig/GCC_XML/CMakeLists.txt:
#
SET(Mummy_save_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
SET(BUILD_SHARED_LIBS 0)
#SET(LIBRARY_OUTPUT_PATH ${GCCXML_BINARY_DIR}/KWSys)
SET(KWSYS_NAMESPACE gxsys)
SET(KWSYS_USE_Process 1)
SET(KWSYS_USE_RegularExpression 1)
SET(KWSYS_USE_SystemTools 1)
SET(KWSYS_HEADER_ROOT "${Mummy_CableSwig_BINARY_DIR}/GCC_XML")
#IF("${CMAKE_SYSTEM}" MATCHES "AIX") # mummy does not build on AIX
#  SET(KWSYS_LFS_DISABLE 1)
#ENDIF("${CMAKE_SYSTEM}" MATCHES "AIX")
#INCLUDE_DIRECTORIES(${GCCXML_BINARY_DIR}) # done later

# Make sure gxsys gets built with CommandLineArguments support:
#
SET(KWSYS_USE_CommandLineArguments 1)

# Always enable and build Testing and Documentation if building as a top
# level project.
#
OPTION(Mummy_RUN_LONG_TESTS "Run mummy tests that take more than a few seconds to run..." ON)
IF(Mummy_BUILD_IS_TOP_LEVEL)
  SET(Mummy_BUILD_EXAMPLES 1)

  # Avoid the "Continuous, Experimental, Nightly and NightlyMemoryCheck" custom
  # targets for Visual Studio builds:
  #
  IF(CMAKE_GENERATOR MATCHES "Visual Studio")
  IF(COMMAND SET_PROPERTY)
    SET_PROPERTY(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
  ENDIF(COMMAND SET_PROPERTY)
  ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio")

  INCLUDE(CTest)

  ENABLE_TESTING()
  SET(Mummy_BUILD_TESTING 1)

  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/CTestCustom.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CTestCustom.cmake
    @ONLY
    )
ENDIF(Mummy_BUILD_IS_TOP_LEVEL)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/MummyVersion.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/MummyVersion.h"
  @ONLY
  )

SET(MummyRC_RC_FILE_NAME "MummyExe.rc")
SET(MummyRC_FILE_DESCRIPTION "${Mummy_FULL_VERSION_STRING} - command line executable")
SET(MummyRC_ORIGINAL_FILENAME "mummy${CMAKE_EXECUTABLE_SUFFIX}")
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/MummyVersion.rc.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${MummyRC_RC_FILE_NAME}"
  @ONLY
  )

# Enable other projects to use mummy via CMake's FIND_PACKAGE(mummy)
#
CONFIGURE_FILE(
  ${CMAKE_CURRENT_SOURCE_DIR}/MummyConfig.cmake.in
  ${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/MummyConfig.cmake
  @ONLY
  )

# Configure these files (in addition to having custom build/install rules for
# them later) so that doing a find_package(Mummy) down in the "Examples" will
# be able to read these files during this CMake configure...
#
foreach(f FindCsharp.cmake MummyCMakeMacros.cmake)
  CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/${f}
    ${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/${f}
    COPYONLY
    )
endforeach()


# Get a "known to be good" CableSwig snapshot:
#
IF(Mummy_BUILD_COMPONENT_CABLESWIG OR Mummy_BUILD_COMPONENT_MUMMY)
  SET(Mummy_UPDATE_CABLESWIG 1)

  IF(NOT Mummy_CableSwig_CVS_REVISION STREQUAL "")
    IF("${Mummy_CableSwig_PARENT_DIR}/CableSwig/CMakeFiles/cvs-up-args.txt" IS_NEWER_THAN "${CMAKE_CURRENT_BINARY_DIR}/MummyVersion.h")
      SET(Mummy_UPDATE_CABLESWIG 0)
    ENDIF("${Mummy_CableSwig_PARENT_DIR}/CableSwig/CMakeFiles/cvs-up-args.txt" IS_NEWER_THAN "${CMAKE_CURRENT_BINARY_DIR}/MummyVersion.h")

    IF(NOT EXISTS "${Mummy_CableSwig_PARENT_DIR}")
      SET(Mummy_UPDATE_CABLESWIG 1)
    ENDIF(NOT EXISTS "${Mummy_CableSwig_PARENT_DIR}")
  ENDIF(NOT Mummy_CableSwig_CVS_REVISION STREQUAL "")

  IF(Mummy_UPDATE_CABLESWIG)
    GET_CVS_SNAPSHOT(
      "${Mummy_CableSwig_PARENT_DIR}"
      ":pserver:anonymous:@public.kitware.com:/cvsroot/CableSwig"
      "CableSwig"
      "${Mummy_CableSwig_CVS_REVISION}"
      )
  ELSE(Mummy_UPDATE_CABLESWIG)
    MESSAGE(STATUS "CableSwig snapshot already up to date. Skipping cvs update.")
  ENDIF(Mummy_UPDATE_CABLESWIG)

  IF(NOT EXISTS "${Mummy_CableSwig_SOURCE_DIR}/CMakeLists.txt")
    MESSAGE(FATAL_ERROR "error: CableSwig could not be checked out or updated. Check it out or copy it manually to '${Mummy_CableSwig_SOURCE_DIR}'...")
  ENDIF(NOT EXISTS "${Mummy_CableSwig_SOURCE_DIR}/CMakeLists.txt")
ENDIF(Mummy_BUILD_COMPONENT_CABLESWIG OR Mummy_BUILD_COMPONENT_MUMMY)

IF(Mummy_BUILD_COMPONENT_CABLESWIG)
  ADD_SUBDIRECTORY(
    "${Mummy_CableSwig_SOURCE_DIR}/Cable"
    "${Mummy_CableSwig_BINARY_DIR}/Cable"
    )

  ADD_SUBDIRECTORY(
    "${Mummy_CableSwig_SOURCE_DIR}/GCC_XML/KWSys"
    "${Mummy_CableSwig_BINARY_DIR}/GCC_XML/KWSys"
    )

  # Remove these CableSwig settings from the CMake GUI when configuring Mummy:
  #
  MARK_AS_ADVANCED(BISON_YACC)
  MARK_AS_ADVANCED(CSWIG_USE_SYSTEM_GCCXML)
  MARK_AS_ADVANCED(SWIG_BUILD_EXAMPLES)
ENDIF(Mummy_BUILD_COMPONENT_CABLESWIG)

SET(BUILD_SHARED_LIBS ${Mummy_save_BUILD_SHARED_LIBS})

# Don't do these until after including CableSwig because CableSwig presently
# has some level 4 warnings when it builds:
#
MSVC_FORCE_WARNING_LEVEL("4")
#MSVC_TREAT_WARNINGS_AS_ERRORS()

MACRO(ADD_CUSTOM_COMMAND_COPYFILE srcFile dstFile)
  ADD_CUSTOM_COMMAND(
    OUTPUT "${dstFile}"
    DEPENDS "${srcFile}"
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy "${srcFile}" "${dstFile}"
    )
ENDMACRO(ADD_CUSTOM_COMMAND_COPYFILE)

IF(Mummy_BUILD_COMPONENT_MUMMY)
  ADD_LIBRARY(mummyLib STATIC
    MummyApplication.cxx
    MummyApplication.h
    MummyCsharpExportLayerGenerator.cxx
    MummyCsharpExportLayerGenerator.h
    MummyCsharpGenerator.cxx
    MummyCsharpGenerator.h
    MummyCsharpShadowLayerGenerator.cxx
    MummyCsharpShadowLayerGenerator.h
    MummyCsharpUnitTestGenerator.cxx
    MummyCsharpUnitTestGenerator.h
    MummyDummyGenerator.cxx
    MummyDummyGenerator.h
    MummyGenerator.cxx
    MummyGenerator.h
    MummyLineOrientedTextFileReader.cxx
    MummyLineOrientedTextFileReader.h
    MummyLog.h
    MummySettings.cxx
    MummySettings.h
    MummyUtilities.cxx
    MummyUtilities.h
    "${CMAKE_CURRENT_BINARY_DIR}/MummyVersion.h"
    )
  TARGET_LINK_LIBRARIES(mummyLib
    "CableGenerators"
    "CableParsers"
    "CableExpat"
    "CxxTypes"
    "gxsys"
    )

  ADD_EXECUTABLE(mummy
    MummyMain.cxx
    "${CMAKE_CURRENT_BINARY_DIR}/MummyExe.rc"
    )
  TARGET_LINK_LIBRARIES(mummy
    "mummyLib"
    )

  # Add custom build steps to copy these include/share files...
  # Copy them into the build tree in a location that is mirror
  # to the install tree location -- this will make our build
  # tree an exact mirror of our install tree and simplify
  # the code needed in MummyConfig.cmake (if build tree has
  # exactly the same structure as install tree, then
  # MummyConfig.cmake does not need two ways of doing things)
  #
  # This will be an OK starategy for mummy because the number
  # of these files is small and build time will be negligible
  # for these steps. Do not copy this strategy for a project
  # with hundreds or more installed files...
  #
  ADD_CUSTOM_COMMAND_COPYFILE(
    "${Mummy_SOURCE_DIR}/Runtime/MummyRuntime.h"
    "${Mummy_BINARY_DIR}/include/mummy-${Mummy_VERSION_STRING}/MummyRuntime.h"
    )
  ADD_CUSTOM_COMMAND_COPYFILE(
    "${Mummy_SOURCE_DIR}/CSharpTestDriver.cs.in"
    "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/CSharpTestDriver.cs.in"
    )
  ADD_CUSTOM_COMMAND_COPYFILE(
    "${Mummy_SOURCE_DIR}/FindCsharp.cmake"
    "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/FindCsharp.cmake"
    )
  ADD_CUSTOM_COMMAND_COPYFILE(
    "${Mummy_SOURCE_DIR}/MummyCMakeMacros.cmake"
    "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/MummyCMakeMacros.cmake"
    )
  ADD_CUSTOM_COMMAND_COPYFILE(
    "${Mummy_SOURCE_DIR}/SvnMacros.cmake"
    "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/SvnMacros.cmake"
    )
  ADD_CUSTOM_TARGET(
    Kitware.mummy.CopyFiles ALL
    DEPENDS
      "${Mummy_BINARY_DIR}/include/mummy-${Mummy_VERSION_STRING}/MummyRuntime.h"
      "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/CSharpTestDriver.cs.in"
      "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/FindCsharp.cmake"
      "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/MummyCMakeMacros.cmake"
      "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/SvnMacros.cmake"
    )
  ADD_DEPENDENCIES(mummyLib Kitware.mummy.CopyFiles)

  # If building 32-bit with Visual Studio, then also build the 64-bit
  # mummy Runtime underneath this binary tree:
  #
  IF(Mummy_32_BIT)
  IF("${CMAKE_GENERATOR}" MATCHES "Visual Studio" AND NOT "${CMAKE_GENERATOR}" MATCHES "Win64")
    # MCAB variables are "Mummy Configure and Build" variables...
    #
    SET(MCAB_BINARY_DIR "${Mummy_BINARY_DIR}/x64Build")
    SET(MCAB_SCRIPT "${Mummy_BINARY_DIR}/Kitware.mummy.Build.x64.dir/${CMAKE_CFG_INTDIR}/ConfigureAndBuild.cmake")

    ADD_CUSTOM_COMMAND(
      OUTPUT ${MCAB_SCRIPT}
      COMMAND ${CMAKE_COMMAND}
      ARGS
        "-DMCAB_BINARY_DIR:PATH=${MCAB_BINARY_DIR}"
        "-DMCAB_CONFIG:STRING=${CMAKE_CFG_INTDIR}"
        "-DMCAB_EXECUTABLE_OUTPUT_PATH:PATH=${EXECUTABLE_OUTPUT_PATH}/x64"
        "-DMCAB_GENERATOR:STRING=${CMAKE_GENERATOR} Win64"
        "-DMCAB_LIBRARY_OUTPUT_PATH:PATH=${LIBRARY_OUTPUT_PATH}/x64"
        "-DMCAB_Mummy_64_JUST_BUILD_RUNTIME:BOOL=ON"
        "-DMCAB_Mummy_SNKEYFILE:FILEPATH=${Mummy_SNKEYFILE}"
        "-DMCAB_PROJECT:STRING=Mummy"
        "-DMCAB_SCRIPT:STRING=${MCAB_SCRIPT}"
        "-DMCAB_SOURCE_DIR:PATH=${Mummy_SOURCE_DIR}"
        -P "${Mummy_SOURCE_DIR}/DoConfigureAndBuild.cmake"
      )

    ADD_CUSTOM_TARGET(
      Kitware.mummy.Build.x64 ALL
      DEPENDS
        "${MCAB_SCRIPT}"
      )

    ADD_DEPENDENCIES(mummyLib Kitware.mummy.Build.x64)
  ENDIF("${CMAKE_GENERATOR}" MATCHES "Visual Studio" AND NOT "${CMAKE_GENERATOR}" MATCHES "Win64")
  ENDIF(Mummy_32_BIT)
ENDIF(Mummy_BUILD_COMPONENT_MUMMY)

# Always build the mummy runtime helper dll. Generated code uses mummy::Runtime
# and mummy::TypeEntry, so clients must link to this runtime helper dll.
#
IF(Mummy_BUILD_COMPONENT_MUMMY_RUNTIME)
  ADD_SUBDIRECTORY(Runtime)
ENDIF(Mummy_BUILD_COMPONENT_MUMMY_RUNTIME)

IF(Mummy_BUILD_COMPONENT_MUMMY)
  # Add a dependency on the runtime dll so that the runtime is always
  # up to date even for targets later in the food chain.
  #
  # (That way the Examples can just depend on mummy and they will be
  # guaranteed to be built *after* the runtime dlls are built...)
  #
  # mummy itself does not contain a build-time dependency on the runtime
  # dlls, but these rules are for the convenience of those who come later.
  #
  ADD_DEPENDENCIES(mummy Kitware.mummy.Runtime.Unmanaged)
ENDIF(Mummy_BUILD_COMPONENT_MUMMY)

# Projects building Mummy that want Mummy's Examples should
# SET(Mummy_BUILD_EXAMPLES 1). Also, Examples can only be built
# if the mummy component itself is built. (The examples use
# the built mummy component...)
#
IF(Mummy_BUILD_EXAMPLES)
IF(Mummy_BUILD_COMPONENT_MUMMY)
  # When building the Examples as part of the mummy build, enable them to find
  # MummyConfig.cmake in this build tree:
  #
  SET(Mummy_DIR "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}")
  ADD_SUBDIRECTORY(Examples)
ENDIF(Mummy_BUILD_COMPONENT_MUMMY)
ENDIF(Mummy_BUILD_EXAMPLES)

# Projects building Mummy that want Mummy's Testing should ENABLE_TESTING and
# SET(Mummy_BUILD_TESTING 1). Can only build if the mummy component itself
# was built.
#
IF(Mummy_BUILD_TESTING)
IF(Mummy_BUILD_COMPONENT_MUMMY)
  #
  # Mummy_TESTING_CONFIG_PREFIX may be defined in the CMake cache...
  #
  # If so, it will either be "" or something like "Debug/" such that
  # it can be used in an expression like this:
  #
  #   ${EXECUTABLE_OUTPUT_PATH}/${Mummy_TESTING_CONFIG_PREFIX}blah.exe
  #
  ADD_SUBDIRECTORY(MummyTesting)
ENDIF(Mummy_BUILD_COMPONENT_MUMMY)
ENDIF(Mummy_BUILD_TESTING)

# Build mummy documentation, too. (Only actually builds if doxygen can be
# found.)
#
IF(Mummy_BUILD_COMPONENT_MUMMY)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/MummyDoxygen.cmake")
ENDIF(Mummy_BUILD_COMPONENT_MUMMY)

# "make install" rules
#
IF(Mummy_BUILD_COMPONENT_MUMMY)
  INSTALL(TARGETS
    mummy
    RUNTIME DESTINATION "bin"
    )

  IF(WIN32)
    # These dll files only get built on Windows, and they are only
    # in the \${BUILD_TYPE} subdirectory for build systems that use
    # CMAKE_CONFIGURATION_TYPES:
    #
    IF(CMAKE_CONFIGURATION_TYPES)
      SET(build_type_subdir "/\${BUILD_TYPE}")
    ELSE(CMAKE_CONFIGURATION_TYPES)
      SET(build_type_subdir "")
    ENDIF(CMAKE_CONFIGURATION_TYPES)

    INSTALL(FILES
      "${Mummy_BINARY_DIR}/bin${build_type_subdir}/Kitware.mummy.Runtime.dll"
      DESTINATION "bin"
      )

    IF(MCAB_BINARY_DIR)
      INSTALL(FILES
        "${Mummy_BINARY_DIR}/bin/x64${build_type_subdir}/Kitware.mummy.Runtime.dll"
        "${Mummy_BINARY_DIR}/bin/x64${build_type_subdir}/Kitware.mummy.Runtime.Unmanaged.dll"
        "${Mummy_BINARY_DIR}/bin/x64${build_type_subdir}/Kitware.mummy.Runtime.Unmanaged.lib"
        DESTINATION "bin/x64"
        )
    ENDIF(MCAB_BINARY_DIR)
  ENDIF(WIN32)

  INSTALL(FILES
    "${Mummy_BINARY_DIR}/include/mummy-${Mummy_VERSION_STRING}/MummyRuntime.h"
    DESTINATION "include/mummy-${Mummy_VERSION_STRING}"
    )

  INSTALL(FILES
    "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/CSharpTestDriver.cs.in"
    "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/FindCsharp.cmake"
    "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/MummyCMakeMacros.cmake"
    "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/MummyConfig.cmake"
    "${Mummy_BINARY_DIR}/share/mummy-${Mummy_VERSION_STRING}/SvnMacros.cmake"
    DESTINATION "share/mummy-${Mummy_VERSION_STRING}"
    )
ENDIF(Mummy_BUILD_COMPONENT_MUMMY)

# Build a mummy installer. Only useful if the mummy component itself was built.
#
IF(Mummy_BUILD_COMPONENT_MUMMY)
  INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/MummyCPack.cmake")
ENDIF(Mummy_BUILD_COMPONENT_MUMMY)

MARK_AS_ADVANCED(BUILD_SHARED_LIBS)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
