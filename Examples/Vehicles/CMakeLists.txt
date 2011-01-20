CMAKE_MINIMUM_REQUIRED(VERSION 2.4 FATAL_ERROR)
PROJECT(Vehicles)


# Determine if we're building Vehicles as a "top level" project.
#
SET(Vehicles_BUILD_IS_TOP_LEVEL 0)
IF("${Vehicles_BINARY_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  SET(Vehicles_BUILD_IS_TOP_LEVEL 1)
ENDIF("${Vehicles_BINARY_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")


INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/MsvcMacros.cmake")
MSVC_FORCE_WARNING_LEVEL("4")
#MSVC_TREAT_WARNINGS_AS_ERRORS()
MSVC_LINK_TO_STATIC_CRT()
MSVC80_FORCE_MANIFEST_LINKER_FLAG()


# 32 or 64 bit build?
#
SET(Vehicles_TARGET_CPU_SIZE "32")
IF(CMAKE_CL_64)
  SET(Vehicles_TARGET_CPU_SIZE "64")
ENDIF(CMAKE_CL_64)


# Debug or not for the C# layers?
#
SET(Vehicles_BUILD_CSHARP_DEBUG_DEFAULT OFF)
IF("${BUILDNAME}" MATCHES "Debug")
  SET(Vehicles_BUILD_CSHARP_DEBUG_DEFAULT ON)
ENDIF("${BUILDNAME}" MATCHES "Debug")
IF("${BUILDNAME}" MATCHES "RelWithDebInfo")
  SET(Vehicles_BUILD_CSHARP_DEBUG_DEFAULT ON)
ENDIF("${BUILDNAME}" MATCHES "RelWithDebInfo")


OPTION(BUILD_SHARED_LIBS "Build shared libraries" ON)
OPTION(BUILD_TESTING "Build/add tests" ON)
SET(Vehicles_INSTALL_TARGETS 0)
IF(Vehicles_BUILD_IS_TOP_LEVEL)
  SET(VEHICLES_INSTALL_PREFIX "${CMAKE_BINARY_DIR} VIP Install" CACHE STRING "")
  SET(CMAKE_INSTALL_PREFIX "${VEHICLES_INSTALL_PREFIX}" CACHE INTERNAL "" FORCE)
  SET(Vehicles_INSTALL_TARGETS 1)
ENDIF(Vehicles_BUILD_IS_TOP_LEVEL)
OPTION(Vehicles_BUILD_CSHARP_DEBUG "Build C# components with '/debug' command line flag" ${Vehicles_BUILD_CSHARP_DEBUG_DEFAULT})


IF(NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE STRING "")
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)
IF(NOT LIBRARY_OUTPUT_PATH)
  SET(LIBRARY_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}" CACHE STRING "")
ENDIF(NOT LIBRARY_OUTPUT_PATH)


MACRO(DEFINE_KIT kit kit_deps_var)
  SET(${kit}_classes ${ARGN} CACHE STRING "")

  SET(${kit}_deps ${${kit_deps_var}} CACHE STRING "")

  SET(${kit}_source_files "")
  FOREACH(class ${${kit}_classes})
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${class}.cxx")
      SET(${kit}_source_files ${${kit}_source_files}
        "${CMAKE_CURRENT_SOURCE_DIR}/${class}.cxx"
      )
    ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${class}.cxx")
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${class}.h")
      SET(${kit}_source_files ${${kit}_source_files}
        "${CMAKE_CURRENT_SOURCE_DIR}/${class}.h"
      )
    ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${class}.h")
  ENDFOREACH(class)
  SET(deffile "${CMAKE_CURRENT_SOURCE_DIR}/${kit}${Vehicles_TARGET_CPU_SIZE}.def")
  IF(EXISTS "${deffile}")
    SET(${kit}_source_files ${${kit}_source_files} "${deffile}")
  ENDIF(EXISTS "${deffile}")
  SET(${kit}_source_files ${${kit}_source_files} CACHE STRING "")

  SET(${kit}_include_dirs "${Vehicles_SOURCE_DIR}/${kit}" CACHE STRING "")

  SET(${kit}_libs "${kit}" CACHE STRING "")

  MARK_AS_ADVANCED(${kit}_classes)
  MARK_AS_ADVANCED(${kit}_deps)
  MARK_AS_ADVANCED(${kit}_source_files)
  MARK_AS_ADVANCED(${kit}_include_dirs)
  MARK_AS_ADVANCED(${kit}_libs)
ENDMACRO(DEFINE_KIT)


MACRO(ECHO_KIT kit)
  MESSAGE(STATUS "${kit}_classes='${${kit}_classes}'")
  MESSAGE(STATUS "${kit}_deps='${${kit}_deps}'")
  MESSAGE(STATUS "${kit}_include_dirs='${${kit}_include_dirs}'")
  MESSAGE(STATUS "${kit}_libs='${${kit}_libs}'")
  MESSAGE(STATUS "${kit}_source_files='${${kit}_source_files}'")
ENDMACRO(ECHO_KIT)


CONFIGURE_FILE(
  "${Vehicles_SOURCE_DIR}/VehiclesConfig.h.in"
  "${Vehicles_BINARY_DIR}/VehiclesConfig.h"
)


INCLUDE_DIRECTORIES(${Vehicles_SOURCE_DIR})
INCLUDE_DIRECTORIES(${Vehicles_BINARY_DIR})
LINK_DIRECTORIES(${EXECUTABLE_OUTPUT_PATH})
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})


MACRO(ADD_KIT kit)
  INCLUDE_DIRECTORIES("${Vehicles_SOURCE_DIR}/${kit}")
  SET(Vehicles_kits ${Vehicles_kits} "${kit}")
  ADD_SUBDIRECTORY("${kit}")
  #ECHO_KIT("${kit}")
ENDMACRO(ADD_KIT)


SET(Vehicles_kits "")

ADD_KIT(base)
ADD_KIT(cars)
ADD_KIT(motorcycles)
ADD_KIT(suvs)

# Always enable and build Testing if building as a top level project.
#
IF(Vehicles_BUILD_IS_TOP_LEVEL)
  INCLUDE(CTest)
  ENABLE_TESTING()
  SET(Vehicles_BUILD_TESTING 1)
ENDIF(Vehicles_BUILD_IS_TOP_LEVEL)

IF(Vehicles_BUILD_TESTING)
  ADD_KIT(tests)
ENDIF(Vehicles_BUILD_TESTING)

SET(Vehicles_kits ${Vehicles_kits} CACHE STRING "")
MARK_AS_ADVANCED(Vehicles_kits)


ADD_SUBDIRECTORY(wrappers)
