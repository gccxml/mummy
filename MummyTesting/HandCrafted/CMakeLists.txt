# Build as is, force static lib:
#
ADD_LIBRARY(handCraftedLib STATIC simple.cxx)

# Forcing static lib on Linux means we have to use the -fPIC flag for compiling
# sources in the target:
#
IF(NOT WIN32 AND NOT APPLE)
  SET_TARGET_PROPERTIES(handCraftedLib PROPERTIES COMPILE_FLAGS "-fPIC")
ENDIF(NOT WIN32 AND NOT APPLE)

# Build wrapper layers:
#
# Both gccxml_EXECUTABLE and mummy_EXECUTABLE should have been found by parent
# project... verify:
#
IF(NOT gccxml_EXECUTABLE)
  MESSAGE(FATAL_ERROR "error: no gccxml_EXECUTABLE - cannot build wrapper layers")
ENDIF(NOT gccxml_EXECUTABLE)
IF(NOT mummy_EXECUTABLE)
  MESSAGE(FATAL_ERROR "error: no mummy_EXECUTABLE - cannot build wrapper layers")
ENDIF(NOT mummy_EXECUTABLE)

SET(gccxml_output "${CMAKE_CURRENT_BINARY_DIR}/simple_gccxml.xml")
SET(mummy_exportlayer_output "${CMAKE_CURRENT_BINARY_DIR}/simple_exportlayer.cxx")
SET(mummy_csharp_output "${CMAKE_CURRENT_BINARY_DIR}/handCraftedSimple.cs")
SET(mummy_settings "${CMAKE_CURRENT_SOURCE_DIR}/handCraftedMummySettings.xml")

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/handCraftedSimple.cs"
  "${CMAKE_CURRENT_BINARY_DIR}/handCraftedSimpleExtra.cs"
  @ONLY
  )

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/simple_gccxml.cxx.in"
  "${CMAKE_CURRENT_BINARY_DIR}/simple_gccxml.cxx"
  @ONLY
  )

IF(${gccxml_EXECUTABLE} MATCHES "castxml")
    SET(executable_output -o ${gccxml_output})

	IF(MSVC)
		SET(castxml_options -fms-compatibility -fms-extensions -fmsc-version=${MSVC_VERSION} --castxml-cc-msvc ${CMAKE_CXX_COMPILER})
	ELSE(MSVC)
		SET(castxml_options --castxml-cc-gnu ${CMAKE_CXX_COMPILER})
	ENDIF(MSVC)

	SET(executable_options --castxml-start _cable_ --castxml-gccxml ${castxml_options} -D__CASTXML__)
ELSE(${gccxml_EXECUTABLE} MATCHES "castxml")
    SET(executable_output -fxml=${gccxml_output})
	SET(executable_options -fxml-start=_cable_ --gccxml-compiler ${CMAKE_CXX_COMPILER})
ENDIF(${gccxml_EXECUTABLE} MATCHES "castxml")

ADD_CUSTOM_COMMAND(
  OUTPUT ${gccxml_output}
  COMMAND ${gccxml_EXECUTABLE}
  ARGS
	${executable_output}
	${executable_options}
    -I${CMAKE_CURRENT_SOURCE_DIR}
    -DCABLE_CONFIGURATION
    ${CMAKE_CURRENT_BINARY_DIR}/simple_gccxml.cxx
  DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/simple_gccxml.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/simple.h
    ${gccxml_EXECUTABLE}
  )

ADD_CUSTOM_COMMAND(
  OUTPUT ${mummy_csharp_output} ${mummy_exportlayer_output}
  COMMAND ${mummy_EXECUTABLE}
  ARGS
    --settings-file ${mummy_settings}
    --gccxml-file ${gccxml_output}
    --csharp-file ${mummy_csharp_output}
    --export-layer-file ${mummy_exportlayer_output}
  DEPENDS
    ${mummy_settings}
    ${gccxml_output}
    ${mummy_EXECUTABLE}
  )

# Build the "export-layer" library:
#
ADD_LIBRARY(handCraftedExportLayer SHARED ${mummy_exportlayer_output})
TARGET_LINK_LIBRARIES(handCraftedExportLayer
  handCraftedLib
  Kitware.mummy.Runtime.Unmanaged
  )
