SET(location ${EXECUTABLE_OUTPUT_PATH})
IF(NOT CMAKE_CFG_INTDIR STREQUAL ".")
  SET(location ${location}/${CMAKE_CFG_INTDIR})
ENDIF(NOT CMAKE_CFG_INTDIR STREQUAL ".")


FIND_PACKAGE(Mummy REQUIRED)
IF(NOT Mummy_BASE_DIR)
  MESSAGE(FATAL_ERROR "error: Mummy_BASE_DIR not defined. Please set Mummy_DIR to the directory containing MummyConfig.cmake")
ENDIF(NOT Mummy_BASE_DIR)

INCLUDE("${Mummy_DIR}/MummyCMakeMacros.cmake")


# Tools locations - either already found by parent project, specified on the
# command line, or available in the PATH... If not, you'll have to find
# these things yourself at configure time... :-)
#
INCLUDE("${Mummy_DIR}/FindCsharp.cmake")

IF(NOT csc_EXECUTABLE)
  FIND_PROGRAM(csc_EXECUTABLE csc)
ENDIF(NOT csc_EXECUTABLE)
IF(NOT csc_EXECUTABLE)
  MESSAGE(FATAL_ERROR "error: C# compiler not found. csc_EXECUTABLE='${csc_EXECUTABLE}'")
ENDIF(NOT csc_EXECUTABLE)

IF(NOT mummy_EXECUTABLE)
  FIND_PROGRAM(mummy_EXECUTABLE mummy)
ENDIF(NOT mummy_EXECUTABLE)
IF(NOT mummy_EXECUTABLE)
  MESSAGE(FATAL_ERROR "error: mummy not found. mummy_EXECUTABLE='${mummy_EXECUTABLE}'")
ENDIF(NOT mummy_EXECUTABLE)

IF(NOT gccxml_EXECUTABLE)
  FIND_PROGRAM(gccxml_EXECUTABLE gccxml)
ENDIF(NOT gccxml_EXECUTABLE)
IF(NOT gccxml_EXECUTABLE)
  MESSAGE(FATAL_ERROR "error: gccxml not found. gccxml_EXECUTABLE='${gccxml_EXECUTABLE}'")
ENDIF(NOT gccxml_EXECUTABLE)


INCLUDE_DIRECTORIES(${Mummy_INCLUDE_DIRS})


SET(gccxml_include_args "")
SET(gccxml_include_args ${gccxml_include_args} "-I${Vehicles_SOURCE_DIR}")
SET(gccxml_include_args ${gccxml_include_args} "-I${Vehicles_BINARY_DIR}")
FOREACH(kit ${Vehicles_kits})
  FOREACH(dir ${${kit}_include_dirs})
    #MESSAGE(STATUS "dir='${dir}'")
    SET(gccxml_include_args ${gccxml_include_args} "-I${dir}")
  ENDFOREACH(dir)
ENDFOREACH(kit)
#MESSAGE(STATUS "gccxml_include_args='${gccxml_include_args}'")


MACRO(WRAP_KIT kit lang ext)
  SET(generated_sources "")

  SET(class_list ${${kit}_classes})

  SET(${kit}_${lang}_wrapper_source_files "")
  SET(${kit}_${lang}_wrapper_source_files_cmake "")
  SET(${kit}_EL_SOURCES "")

IF(${gccxml_EXECUTABLE} MATCHES "castxml")
    SET(executable_output -o ${CMAKE_CURRENT_BINARY_DIR})

	IF(MSVC)
		SET(compiler msvc)
	ELSE(MSVC)
		SET(compiler gnu)
	ENDIF(MSVC)

	SET(executable_options --castxml-start _cable_ --castxml-gccxml --castxml-cc-${compiler} ${CMAKE_CXX_COMPILER} -D__CASTXML__)
ELSE(${gccxml_EXECUTABLE} MATCHES "castxml")
    SET(executable_output -fxml=${CMAKE_CURRENT_BINARY_DIR})
	SET(executable_options -fxml-start=_cable_ --gccxml-compiler ${CMAKE_CXX_COMPILER})
ENDIF(${gccxml_EXECUTABLE} MATCHES "castxml")

  FOREACH(cxxclass ${class_list})
    SET(header "${cxxclass}.h")

    CONFIGURE_FILE(
      "${CMAKE_CURRENT_SOURCE_DIR}/gccxml.cxx.in"
      "${CMAKE_CURRENT_BINARY_DIR}/xml/${cxxclass}_gccxml.cxx"
      @ONLY
    )

    # Full path needed for proper dependency checking:
    #
    SET(header_fullpath "${header}")
    IF(EXISTS "${${kit}_include_dirs}/${cxxclass}.h")
      SET(header_fullpath "${${kit}_include_dirs}/${cxxclass}.h")
    ENDIF(EXISTS "${${kit}_include_dirs}/${cxxclass}.h")
    ADD_CUSTOM_COMMAND(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xml/${cxxclass}.xml
      COMMAND ${gccxml_EXECUTABLE}
      ARGS
		${executable_output}/xml/${cxxclass}.xml
		${executable_options}
        ${gccxml_include_args} -DCABLE_CONFIGURATION
        ${CMAKE_CURRENT_BINARY_DIR}/xml/${cxxclass}_gccxml.cxx
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/xml/${cxxclass}_gccxml.cxx
        ${header_fullpath}
        ${gccxml_EXECUTABLE}
      )

    ADD_CUSTOM_COMMAND(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}.${ext}
        ${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}EL.cxx
      COMMAND ${mummy_EXECUTABLE}
      ARGS
        --settings-file ${CMAKE_CURRENT_BINARY_DIR}/xml/MummySettings.xml
        --gccxml-file ${CMAKE_CURRENT_BINARY_DIR}/xml/${cxxclass}.xml
        --csharp-file ${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}.${ext}
        --export-layer-file ${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}EL.cxx
      DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/xml/${cxxclass}.xml
        ${CMAKE_CURRENT_BINARY_DIR}/xml/MummySettings.xml
        ${mummy_EXECUTABLE}
      )

    SET(generated_sources ${generated_sources}
      "${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}.${ext}"
      "${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}EL.cxx"
      )

    IF(WIN32)
     STRING(REGEX REPLACE "/" "\\\\" nf "${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}.${ext}")
    ELSE(WIN32)
     SET(nf "${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}.${ext}")
    ENDIF(WIN32)
    SET(${kit}_${lang}_wrapper_source_files ${${kit}_${lang}_wrapper_source_files} "${nf}")
    SET(${kit}_${lang}_wrapper_source_files_cmake ${${kit}_${lang}_wrapper_source_files_cmake} "${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}.${ext}")

    SET(${kit}_EL_SOURCES ${${kit}_EL_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}EL.cxx")
    SET(ALLKITS_EL_SOURCES ${ALLKITS_EL_SOURCES} "${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}EL.cxx")
  ENDFOREACH(cxxclass)

  # Hand-written/configured WrappedObject and WrappedException base classes:
  #
  IF("${kit}" STREQUAL "base")
    FOREACH(cxxclass "WrappedObject" "WrappedException")
    IF(WIN32)
     STRING(REGEX REPLACE "/" "\\\\" nf "${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}.${ext}")
    ELSE(WIN32)
     SET(nf "${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}.${ext}")
    ENDIF(WIN32)
    SET(${kit}_${lang}_wrapper_source_files ${${kit}_${lang}_wrapper_source_files} "${nf}")
    SET(${kit}_${lang}_wrapper_source_files_cmake ${${kit}_${lang}_wrapper_source_files_cmake} "${CMAKE_CURRENT_BINARY_DIR}/export-layer/${cxxclass}.${ext}")
    ENDFOREACH(cxxclass)
  ENDIF("${kit}" STREQUAL "base")

  SET(${kit}_${lang}_wrapper_source_files ${${kit}_${lang}_wrapper_source_files} CACHE STRING "")
  SET(${kit}_${lang}_wrapper_source_files_cmake ${${kit}_${lang}_wrapper_source_files_cmake} CACHE STRING "")
  MARK_AS_ADVANCED(${kit}_${lang}_wrapper_source_files)
  MARK_AS_ADVANCED(${kit}_${lang}_wrapper_source_files_cmake)

  SET(csc_ttype "/target:library")
  SET(csc_ext "dll")
  GET_TARGET_PROPERTY(ttype ${kit} TYPE)
  IF("${ttype}" STREQUAL "EXECUTABLE")
    SET(csc_ttype "/target:exe") # "exe" for console apps, "winexe" for GUI/Windows apps
    SET(csc_ext "exe")
  ENDIF("${ttype}" STREQUAL "EXECUTABLE")

  SET(deps "")
  SET(refs "")
  FOREACH(d ${${kit}_deps})
    SET(deps ${deps} "${location}/${d}_${lang}.dll")
    IF(WIN32)
     STRING(REGEX REPLACE "/" "\\\\" r "${location}/${d}_${lang}.dll")
    ELSE(WIN32)
     SET(r "${location}/${d}_${lang}.dll")
    ENDIF(WIN32)
    #FILE(TO_NATIVE_PATH "${location}/${d}_${lang}.dll" r)
    SET(refs ${refs} "/reference:${r}")
  ENDFOREACH(d)
  SET(deps ${deps} "${location}/Kitware.mummy.Runtime.dll")
  IF(WIN32)
   STRING(REGEX REPLACE "/" "\\\\" r "${location}/Kitware.mummy.Runtime.dll")
  ELSE(WIN32)
   SET(r "${location}/Kitware.mummy.Runtime.dll")
  ENDIF(WIN32)
  SET(refs ${refs} "/reference:${r}")

  IF(WIN32)
   STRING(REGEX REPLACE "/" "\\\\" csc_out "${location}/${kit}_${lang}.${csc_ext}")
  ELSE(WIN32)
   SET(csc_out "${location}/${kit}_${lang}.${csc_ext}")
  ENDIF(WIN32)
  #FILE(TO_NATIVE_PATH "${location}/${kit}_${lang}.${csc_ext}" csc_out)

  #MESSAGE(STATUS "${kit}_${lang}_wrapper_source_files='${${kit}_${lang}_wrapper_source_files}'")
  #MESSAGE(STATUS "${kit}_${lang}_wrapper_source_files_cmake='${${kit}_${lang}_wrapper_source_files_cmake}'")
  ADD_CUSTOM_COMMAND(
    OUTPUT "${location}/${kit}_${lang}.${csc_ext}"
    DEPENDS ${${kit}_${lang}_wrapper_source_files_cmake} ${deps}
    COMMAND ${csc_EXECUTABLE}
    ARGS ${CSC_DEBUG_FLAG} "/out:${csc_out}" ${csc_ttype} ${CSC_PLATFORM_FLAG}
      ${refs} ${${kit}_${lang}_wrapper_source_files}
    COMMENT "Building csharp wrappers for kit '${kit}'..."
    )

  ADD_CUSTOM_TARGET(
    "${kit}_${lang}" ALL
    DEPENDS "${location}/${kit}_${lang}.${csc_ext}"
    )

  IF(Mummy_BINARY_DIR)
    ADD_DEPENDENCIES("${kit}_${lang}" "mummy")
  ENDIF(Mummy_BINARY_DIR)

  ADD_DEPENDENCIES("${kit}_${lang}" "${kit}")

  FOREACH(d ${${kit}_deps})
    ADD_DEPENDENCIES("${kit}_${lang}" "${d}_${lang}")
  ENDFOREACH(d)
ENDMACRO(WRAP_KIT)


# Organization:
#
# If MV_ONE_EXPORT_LAYER_DLL is 0 then all the generated export-layer/*.cxx
# files get built into multiple export layer DLLs called ${CMAKE_SHARED_LIBRARY_PREFIX}${kit}EL${CMAKE_SHARED_LIBRARY_SUFFIX}.
# If built as dlls, we need to use multiple export layer DLLs.
#
# If MV_ONE_EXPORT_LAYER_DLL is 1 then all the generated export-layer/*.cxx
# files get built into one large export layer DLL called ${CMAKE_SHARED_LIBRARY_PREFIX}VehiclesEL${CMAKE_SHARED_LIBRARY_SUFFIX}.
# If built as static libs, we need to use one large export layer DLL.
#
IF(BUILD_SHARED_LIBS)
  SET(MV_ONE_EXPORT_LAYER_DLL 0)
ELSE(BUILD_SHARED_LIBS)
  SET(MV_ONE_EXPORT_LAYER_DLL 1)
ENDIF(BUILD_SHARED_LIBS)


# Set list of export-layer dlls in C# syntax as CMake variable
# MV_EXPORTLAYER_DLL_VARIABLES. This gets configured into
# WrappedObject.cs and WrappedException.cs below.
#
SET(MV_EXPORTLAYER_DLL_VARIABLES "")
FOREACH(kit ${Vehicles_kits})
  IF(MV_ONE_EXPORT_LAYER_DLL)
    SET(MV_EXPORTLAYER_DLL_VARIABLES "${MV_EXPORTLAYER_DLL_VARIABLES}    public const string ${kit}EL_dll = \"${CMAKE_SHARED_LIBRARY_PREFIX}VehiclesEL${CMAKE_SHARED_LIBRARY_SUFFIX}\";\n")
  ELSE(MV_ONE_EXPORT_LAYER_DLL)
    SET(MV_EXPORTLAYER_DLL_VARIABLES "${MV_EXPORTLAYER_DLL_VARIABLES}    public const string ${kit}EL_dll = \"${CMAKE_SHARED_LIBRARY_PREFIX}${kit}EL${CMAKE_SHARED_LIBRARY_SUFFIX}\";\n")
  ENDIF(MV_ONE_EXPORT_LAYER_DLL)
ENDFOREACH(kit)


CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/WrappedObject.cs.in"
  "${CMAKE_CURRENT_BINARY_DIR}/export-layer/WrappedObject.cs"
  @ONLY
)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/WrappedException.cs.in"
  "${CMAKE_CURRENT_BINARY_DIR}/export-layer/WrappedException.cs"
  @ONLY
)

IF(CSC_COMPILES_PARTIAL_CLASSES)
  SET(PartialValue "true")
ELSE(CSC_COMPILES_PARTIAL_CLASSES)
  SET(PartialValue "false")
ENDIF(CSC_COMPILES_PARTIAL_CLASSES)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/MummySettings.xml.in"
  "${CMAKE_CURRENT_BINARY_DIR}/xml/MummySettings.xml"
  @ONLY
)


IF(csc_EXECUTABLE)
  SET(ALLKITS_EL_SOURCES "")

  FOREACH(test ${tests_classes})
    CONFIGURE_FILE(
      "${CMAKE_CURRENT_SOURCE_DIR}/extra.cs.in"
      "${CMAKE_CURRENT_BINARY_DIR}/${test}Extra.cs"
      @ONLY
      )
  ENDFOREACH(test)


  FOREACH(kit ${Vehicles_kits})
    WRAP_KIT("${kit}" "csharp" "cs")
  ENDFOREACH(kit)


  # The big one or the many little ones:
  #
  IF(MV_ONE_EXPORT_LAYER_DLL)
    ADD_LIBRARY(VehiclesEL SHARED ${ALLKITS_EL_SOURCES})
    TARGET_LINK_LIBRARIES(VehiclesEL ${Mummy_RUNTIME_LINK_LIBRARIES})
    FOREACH(kit ${Vehicles_kits})
      TARGET_LINK_LIBRARIES(VehiclesEL ${kit})
    ENDFOREACH(kit)

    # Optional dependency - see comment below...
    #ADD_DEPENDENCIES(VehiclesEL "VehiclesDotNET")
  ELSE(MV_ONE_EXPORT_LAYER_DLL)
    FOREACH(kit ${Vehicles_kits})
      ADD_LIBRARY(${kit}EL SHARED ${${kit}_EL_SOURCES})
      TARGET_LINK_LIBRARIES(${kit}EL ${Mummy_RUNTIME_LINK_LIBRARIES})
      TARGET_LINK_LIBRARIES(${kit}EL ${kit})
      ADD_DEPENDENCIES(${kit}EL "${kit}_csharp")

      # Optional dependency - see comment below...
      #ADD_DEPENDENCIES(${kit}EL "VehiclesDotNET")
    ENDFOREACH(kit)
  ENDIF(MV_ONE_EXPORT_LAYER_DLL)
  #
  # Optional dependency comment:
  #
  # There is not a real build dependency here, I just want these things built
  # in this order... I want to try to compile the C# code first to see if
  # there are C# compile/link problems. It only takes about 15 seconds to
  # build the C# DLL compared to several minutes for the export-layer cxx
  # compiles.
  #
  # You could as easily swap the dependency or leave it out entirely...
  #


  FOREACH(test ${tests_classes})
  #FOREACH(test)
    CONFIGURE_FILE(
      "${CMAKE_CURRENT_SOURCE_DIR}/TestMain.cs.in"
      "${CMAKE_CURRENT_BINARY_DIR}/${test}Main.cs"
      @ONLY
      )

    SET(deps "")
    SET(refs "")
    FOREACH(d ${Vehicles_kits})
      SET(deps ${deps} "${location}/${d}_csharp.dll")
      IF(WIN32)
       STRING(REGEX REPLACE "/" "\\\\" r "${location}/${d}_csharp.dll")
      ELSE(WIN32)
       SET(r "${location}/${d}_csharp.dll")
      ENDIF(WIN32)
      #FILE(TO_NATIVE_PATH "${location}/${d}_csharp.dll" r)
      SET(refs ${refs} "/reference:${r}")
    ENDFOREACH(d)
    SET(deps ${deps} "${location}/Kitware.mummy.Runtime.dll")
    IF(WIN32)
      STRING(REGEX REPLACE "/" "\\\\" r "${location}/Kitware.mummy.Runtime.dll")
    ELSE(WIN32)
      SET(r "${location}/Kitware.mummy.Runtime.dll")
    ENDIF(WIN32)
    SET(refs ${refs} "/reference:${r}")

    IF(WIN32)
     STRING(REGEX REPLACE "/" "\\\\" csc_out "${location}/${test}_csharp.exe")
    ELSE(WIN32)
     SET(csc_out "${location}/${test}_csharp.exe")
    ENDIF(WIN32)
    #FILE(TO_NATIVE_PATH "${location}/${test}_csharp.exe" csc_out)

    SET(csc_ttype "/target:exe") # "exe" for console apps, "winexe" for GUI/Windows apps

    SET(csc_unsafe "")

    ADD_CUSTOM_COMMAND(
      OUTPUT "${location}/${test}_csharp.exe"
      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${test}Main.cs ${deps}
      COMMAND ${csc_EXECUTABLE}
      ARGS ${CSC_DEBUG_FLAG} ${csc_unsafe} "/out:${csc_out}" ${csc_ttype} ${CSC_PLATFORM_FLAG}
        ${refs} ${CMAKE_CURRENT_BINARY_DIR}/${test}Main.cs
      COMMENT "Building csharp wrapper for test '${test}'..."
      )

    ADD_CUSTOM_TARGET(
      "${test}_csharp" ALL
      DEPENDS "${location}/${test}_csharp.exe"
      )

    ADD_DEPENDENCIES("${test}_csharp" "tests_csharp")

    IF(EXECUTABLE_OUTPUT_PATH)
      SET(exefile ${EXECUTABLE_OUTPUT_PATH}/${Mummy_TESTING_CONFIG_PREFIX}${test}_csharp.exe)
    ELSE(EXECUTABLE_OUTPUT_PATH)
      SET(exefile ${test}_csharp.exe)
    ENDIF(EXECUTABLE_OUTPUT_PATH)

    IF(Mummy_RUN_LONG_TESTS)
      ADD_CSHARP_TEST(${test}_csharp "${exefile}")
    ELSE(Mummy_RUN_LONG_TESTS)
      IF("${test}" STREQUAL "TestAllVehicles")
        ADD_CSHARP_TEST(${test}_csharp "${exefile}")
      ENDIF("${test}" STREQUAL "TestAllVehicles")
    ENDIF(Mummy_RUN_LONG_TESTS)
  ENDFOREACH(test)


  # Quick running manual tests:
  #
  SET(tests
    ManualTestAllVehicles
    ManualTestEvent
    ManualTestRefCountedVehicle
    ManualTestSignatureSeries
    ManualTestSimpleVehicle
    )

  # Tests that run fine on Windows ("real" .NET framework),
  # but not on all supported versions of mono... (tests
  # that exercise the shadow layer / IDispatch stuff...)
  #
  SET(Mummy_RUN_SHADOW_LAYER_TESTS 0)
  IF(WIN32)
    SET(Mummy_RUN_SHADOW_LAYER_TESTS 1)
  ENDIF(WIN32)

  IF(Mummy_RUN_SHADOW_LAYER_TESTS)
    SET(tests ${tests}
      ManualTestBlatMobile
      )
  ENDIF(Mummy_RUN_SHADOW_LAYER_TESTS)

  # Slow running manual tests:
  #
  IF(Mummy_RUN_LONG_TESTS)
    SET(tests ${tests}
      ManualTestCallback
      ManualTestPerformance
      ManualTestPerformanceArrayAccess
      )
  ENDIF(Mummy_RUN_LONG_TESTS)

  FOREACH(test ${tests})
    SET(deps "")
    SET(refs "")
    FOREACH(d ${Vehicles_kits})
      SET(deps ${deps} "${location}/${d}_csharp.dll")
      IF(WIN32)
       STRING(REGEX REPLACE "/" "\\\\" r "${location}/${d}_csharp.dll")
      ELSE(WIN32)
       SET(r "${location}/${d}_csharp.dll")
      ENDIF(WIN32)
      #FILE(TO_NATIVE_PATH "${location}/${d}_csharp.dll" r)
      SET(refs ${refs} "/reference:${r}")
    ENDFOREACH(d)
    SET(deps ${deps} "${location}/Kitware.mummy.Runtime.dll")
    IF(WIN32)
     STRING(REGEX REPLACE "/" "\\\\" r "${location}/Kitware.mummy.Runtime.dll")
    ELSE(WIN32)
     SET(r "${location}/Kitware.mummy.Runtime.dll")
    ENDIF(WIN32)
    SET(refs ${refs} "/reference:${r}")

    IF(WIN32)
     STRING(REGEX REPLACE "/" "\\\\" csc_out "${location}/${test}_csharp.exe")
    ELSE(WIN32)
     SET(csc_out "${location}/${test}_csharp.exe")
    ENDIF(WIN32)
    #FILE(TO_NATIVE_PATH "${location}/${test}_csharp.exe" csc_out)

    SET(csc_ttype "/target:exe") # "exe" for console apps, "winexe" for GUI/Windows apps

    SET(csc_unsafe "")
    IF("${test}" STREQUAL "ManualTestPerformanceArrayAccess")
      SET(csc_unsafe "/unsafe")
    ENDIF("${test}" STREQUAL "ManualTestPerformanceArrayAccess")

    IF(WIN32)
     STRING(REGEX REPLACE "/" "\\\\" csc_src "${CMAKE_CURRENT_SOURCE_DIR}/${test}.cs")
    ELSE(WIN32)
     SET(csc_src "${CMAKE_CURRENT_SOURCE_DIR}/${test}.cs")
    ENDIF(WIN32)

    ADD_CUSTOM_COMMAND(
      OUTPUT "${location}/${test}_csharp.exe"
      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${test}.cs ${deps}
      COMMAND ${csc_EXECUTABLE}
      ARGS ${CSC_DEBUG_FLAG} ${csc_unsafe} "/out:${csc_out}" ${csc_ttype} ${CSC_PLATFORM_FLAG}
        ${refs} ${csc_src}
      COMMENT "Building csharp wrapper for test '${test}'..."
      )

    ADD_CUSTOM_TARGET(
      "${test}_csharp" ALL
      DEPENDS "${location}/${test}_csharp.exe"
      )

    FOREACH(d ${Vehicles_kits})
      ADD_DEPENDENCIES("${test}_csharp" "${d}_csharp")
    ENDFOREACH(d)

    IF(EXECUTABLE_OUTPUT_PATH)
      SET(exefile ${EXECUTABLE_OUTPUT_PATH}/${Mummy_TESTING_CONFIG_PREFIX}${test}_csharp.exe)
    ELSE(EXECUTABLE_OUTPUT_PATH)
      SET(exefile ${test}_csharp.exe)
    ENDIF(EXECUTABLE_OUTPUT_PATH)

    ADD_CSHARP_TEST(${test}_csharp "${exefile}")
  ENDFOREACH(test)
ENDIF(csc_EXECUTABLE)
