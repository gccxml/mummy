MACRO(ADD_CSHARP_EXECUTABLE ace_ExeName ace_DependentTargets ace_References ace_LinkResources ace_SnkFile)
  IF("${ARGN}" STREQUAL "")
    MESSAGE(FATAL_ERROR "ADD_CSHARP_EXECUTABLE requires at least one optional argument.")
  ENDIF("${ARGN}" STREQUAL "")

  IF(DEFINED MUMMY_ADD_CSHARP_SRCDIR)
    SET(ace_SrcDir "${MUMMY_ADD_CSHARP_SRCDIR}")
  ELSE()
    SET(ace_SrcDir "${CMAKE_CURRENT_SOURCE_DIR}")
  ENDIF()
  IF(DEFINED MUMMY_ADD_CSHARP_BINDIR)
    SET(ace_BinDir "${MUMMY_ADD_CSHARP_BINDIR}")
  ELSE()
    SET(ace_BinDir "${CMAKE_CURRENT_BINARY_DIR}")
  ENDIF()

  SET(ace_ExeDir "${ace_BinDir}/${CMAKE_CFG_INTDIR}")
  IF(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    SET(ace_ExeDir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}")
  ELSEIF(EXECUTABLE_OUTPUT_PATH)
    SET(ace_ExeDir "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}")
  ENDIF()

  SET(ace_CscRefs "")
  SET(ace_CMakeRefs "")
  FOREACH(ace_r ${ace_References})
    SET(ace_rr "${ace_r}")
    IF(NOT "${ace_rr}" MATCHES "^System\\.")
      IF(NOT "${ace_rr}" MATCHES "/")
        SET(ace_rr "${ace_ExeDir}/${ace_rr}")
      ENDIF(NOT "${ace_rr}" MATCHES "/")
      IF(NOT "${ace_rr}" MATCHES "dll$")
        SET(ace_rr "${ace_rr}.dll")
      ENDIF(NOT "${ace_rr}" MATCHES "dll$")
      SET(ace_CMakeRefs ${ace_CMakeRefs} "${ace_rr}")
    ENDIF(NOT "${ace_rr}" MATCHES "^System\\.")
    IF(WIN32)
      STRING(REGEX REPLACE "/" "\\\\" ace_rr "${ace_rr}")
    ENDIF(WIN32)
    SET(ace_CscRefs ${ace_CscRefs} "/reference:${ace_rr}")
  ENDFOREACH(ace_r)

  SET(ace_CscLinkResourceArgs "")
  FOREACH(ace_lr ${ace_LinkResources})
    SET(ace_item "${ace_lr}")
    IF(NOT "${ace_item}" MATCHES "/")
      SET(ace_item "${ace_ExeDir}/${ace_item}")
    ENDIF(NOT "${ace_item}" MATCHES "/")
    IF(WIN32)
      STRING(REGEX REPLACE "/" "\\\\" ace_item "${ace_item}")
    ENDIF(WIN32)
    SET(ace_CscLinkResourceArgs ${ace_CscLinkResourceArgs} "/linkresource:${ace_item}")
  ENDFOREACH(ace_lr)

  SET(ace_CscArgs "")
  FOREACH(ace_f ${ARGN})
    SET(ace_CscArgs "${ace_CscArgs}\"${ace_f}\"\n")
  ENDFOREACH(ace_f)

  SET(ace_CscOut "${ace_ExeDir}/${ace_ExeName}.exe")
  SET(ace_CscDoc "${ace_ExeDir}/${ace_ExeName}.xml")

  IF(WIN32)
    STRING(REGEX REPLACE "/" "\\\\" ace_CscArgs "${ace_CscArgs}")
    STRING(REGEX REPLACE "/" "\\\\" ace_CscOut "${ace_CscOut}")
    STRING(REGEX REPLACE "/" "\\\\" ace_CscDoc "${ace_CscDoc}")
  ENDIF(WIN32)

  IF(CMAKE_CONFIGURATION_TYPES AND DEFINED MUMMY_ADD_CSHARP_CONFIG)
    STRING(REPLACE "${CMAKE_CFG_INTDIR}" "${MUMMY_ADD_CSHARP_CONFIG}"
      ace_CscLinkResourceArgs "${ace_CscLinkResourceArgs}")
    FOREACH(ace_f ${ace_CscLinkResourceArgs})
      SET(ace_CscArgs "${ace_CscArgs}\"${ace_f}\"\n")
    ENDFOREACH(ace_f)
    SET(ace_CscLinkResourceArgs "")
  ENDIF()

  IF(NOT "${ace_SnkFile}" STREQUAL "")
    SET(ace_f "${ace_SnkFile}")
    IF(WIN32)
      STRING(REGEX REPLACE "/" "\\\\" ace_f "${ace_f}")
    ENDIF(WIN32)

    # If the keyfile name ends in ".pub.snk" then it's a public key
    # half of a strong name signature and we should add the "delaysign"
    # flag. Otherwise, assume it's a full key pair for a strong name
    # signature and just specify the keyfile.
    #
    IF(ace_f MATCHES "\\.pub\\.snk$")
      SET(ace_CscArgs "${ace_CscArgs}/delaysign+\n")
    ENDIF(ace_f MATCHES "\\.pub\\.snk$")

    SET(ace_CscArgs "${ace_CscArgs}/keyfile:\"${ace_f}\"\n")
  ENDIF(NOT "${ace_SnkFile}" STREQUAL "")

  SET(CscArgs ${ace_CscArgs})
  CONFIGURE_FILE(
    "${ace_SrcDir}/CscArgs.txt.in"
    "${ace_BinDir}/${ace_ExeName}.CscArgs.txt"
    @ONLY
    )

  IF(csc_EXECUTABLE)
    ADD_CUSTOM_COMMAND(
      OUTPUT "${ace_ExeDir}/${ace_ExeName}.exe"
      DEPENDS ${ARGN} "${ace_BinDir}/${ace_ExeName}.CscArgs.txt" ${ace_CMakeRefs}
      COMMAND ${csc_EXECUTABLE}
      ARGS ${CSC_DEBUG_FLAG} ${CSC_PLATFORM_FLAG}
        "/out:${ace_CscOut}" "/doc:${ace_CscDoc}" "/target:exe"
        ${ace_CscLinkResourceArgs}
        ${ace_CscRefs}
        "@${ace_BinDir}/${ace_ExeName}.CscArgs.txt"
      COMMENT "Building C# executable '${ace_ExeName}'..."
      )

    ADD_CUSTOM_TARGET(
      "${ace_ExeName}" ALL
      DEPENDS "${ace_ExeDir}/${ace_ExeName}.exe"
      )

    IF(NOT "${ace_DependentTargets}" STREQUAL "")
      ADD_DEPENDENCIES("${ace_ExeName}" ${ace_DependentTargets})
    ENDIF(NOT "${ace_DependentTargets}" STREQUAL "")
  ENDIF(csc_EXECUTABLE)
ENDMACRO(ADD_CSHARP_EXECUTABLE)


MACRO(ADD_CSHARP_LIBRARY acl_LibName acl_DependentTargets acl_References acl_LinkResources acl_SnkFile)
  IF("${ARGN}" STREQUAL "")
    MESSAGE(FATAL_ERROR "ADD_CSHARP_LIBRARY requires at least one optional argument.")
  ENDIF("${ARGN}" STREQUAL "")

  IF(DEFINED MUMMY_ADD_CSHARP_SRCDIR)
    SET(acl_SrcDir "${MUMMY_ADD_CSHARP_SRCDIR}")
  ELSE()
    SET(acl_SrcDir "${CMAKE_CURRENT_SOURCE_DIR}")
  ENDIF()
  IF(DEFINED MUMMY_ADD_CSHARP_BINDIR)
    SET(acl_BinDir "${MUMMY_ADD_CSHARP_BINDIR}")
  ELSE()
    SET(acl_BinDir "${CMAKE_CURRENT_BINARY_DIR}")
  ENDIF()

  SET(acl_ExeDir "${acl_BinDir}/${CMAKE_CFG_INTDIR}")
  IF(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    SET(acl_ExeDir "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}")
  ELSEIF(EXECUTABLE_OUTPUT_PATH)
    SET(acl_ExeDir "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}")
  ENDIF()

  SET(acl_CscRefs "")
  SET(acl_CMakeRefs "")
  FOREACH(acl_r ${acl_References})
    SET(acl_rr "${acl_r}")
    IF(NOT "${acl_rr}" MATCHES "^System\\.")
      IF(NOT "${acl_rr}" MATCHES "/")
        SET(acl_rr "${acl_ExeDir}/${acl_rr}")
      ENDIF(NOT "${acl_rr}" MATCHES "/")
      IF(NOT "${acl_rr}" MATCHES "dll$")
        SET(acl_rr "${acl_rr}.dll")
      ENDIF(NOT "${acl_rr}" MATCHES "dll$")
      SET(acl_CMakeRefs ${acl_CMakeRefs} "${acl_rr}")
    ENDIF(NOT "${acl_rr}" MATCHES "^System\\.")
    IF(WIN32)
      STRING(REGEX REPLACE "/" "\\\\" acl_rr "${acl_rr}")
    ENDIF(WIN32)
    SET(acl_CscRefs ${acl_CscRefs} "/reference:${acl_rr}")
  ENDFOREACH(acl_r)

  SET(acl_CscLinkResourceArgs "")
  FOREACH(acl_lr ${acl_LinkResources})
    SET(acl_item "${acl_lr}")
    IF(NOT "${acl_item}" MATCHES "/")
      SET(acl_item "${acl_ExeDir}/${acl_item}")
    ENDIF(NOT "${acl_item}" MATCHES "/")
    IF(WIN32)
      STRING(REGEX REPLACE "/" "\\\\" acl_item "${acl_item}")
    ENDIF(WIN32)
    SET(acl_CscLinkResourceArgs ${acl_CscLinkResourceArgs} "/linkresource:${acl_item}")
  ENDFOREACH(acl_lr)

  SET(acl_CscArgs "")
  FOREACH(acl_f ${ARGN})
    SET(acl_CscArgs "${acl_CscArgs}\"${acl_f}\"\n")
  ENDFOREACH(acl_f)

  SET(acl_CscOut "${acl_ExeDir}/${acl_LibName}.dll")
  SET(acl_CscDoc "${acl_ExeDir}/${acl_LibName}.xml")

  IF(WIN32)
    STRING(REGEX REPLACE "/" "\\\\" acl_CscArgs "${acl_CscArgs}")
    STRING(REGEX REPLACE "/" "\\\\" acl_CscOut "${acl_CscOut}")
    STRING(REGEX REPLACE "/" "\\\\" acl_CscDoc "${acl_CscDoc}")
  ENDIF(WIN32)

  IF(CMAKE_CONFIGURATION_TYPES AND DEFINED MUMMY_ADD_CSHARP_CONFIG)
    STRING(REPLACE "${CMAKE_CFG_INTDIR}" "${MUMMY_ADD_CSHARP_CONFIG}"
      acl_CscLinkResourceArgs "${acl_CscLinkResourceArgs}")
    FOREACH(acl_f ${acl_CscLinkResourceArgs})
      SET(acl_CscArgs "${acl_CscArgs}\"${acl_f}\"\n")
    ENDFOREACH()
    SET(acl_CscLinkResourceArgs "")
  ENDIF()

  IF(NOT "${acl_SnkFile}" STREQUAL "")
    SET(acl_f "${acl_SnkFile}")
    IF(WIN32)
      STRING(REGEX REPLACE "/" "\\\\" acl_f "${acl_f}")
    ENDIF(WIN32)

    # If the keyfile name ends in ".pub.snk" then it's a public key
    # half of a strong name signature and we should add the "delaysign"
    # flag. Otherwise, assume it's a full key pair for a strong name
    # signature and just specify the keyfile.
    #
    IF(acl_f MATCHES "\\.pub\\.snk$")
      SET(acl_CscArgs "${acl_CscArgs}/delaysign+\n")
    ENDIF(acl_f MATCHES "\\.pub\\.snk$")

    SET(acl_CscArgs "${acl_CscArgs}/keyfile:\"${acl_f}\"\n")
  ENDIF(NOT "${acl_SnkFile}" STREQUAL "")

  SET(CscArgs ${acl_CscArgs})
  CONFIGURE_FILE(
    "${acl_SrcDir}/CscArgs.txt.in"
    "${acl_BinDir}/${acl_LibName}.CscArgs.txt"
    @ONLY
    )

  IF(csc_EXECUTABLE)
    ADD_CUSTOM_COMMAND(
      OUTPUT "${acl_ExeDir}/${acl_LibName}.dll"
      DEPENDS ${ARGN} "${acl_BinDir}/${acl_LibName}.CscArgs.txt" ${acl_CMakeRefs}
      COMMAND ${csc_EXECUTABLE}
      ARGS ${CSC_DEBUG_FLAG} ${CSC_PLATFORM_FLAG}
        "/out:${acl_CscOut}" "/doc:${acl_CscDoc}" "/target:library"
        ${acl_CscLinkResourceArgs}
        ${acl_CscRefs}
        "@${acl_BinDir}/${acl_LibName}.CscArgs.txt"
      COMMENT "Building C# library '${acl_LibName}'..."
      )

    ADD_CUSTOM_TARGET(
      "${acl_LibName}" ALL
      DEPENDS "${acl_ExeDir}/${acl_LibName}.dll"
      )

    IF(NOT "${acl_DependentTargets}" STREQUAL "")
      ADD_DEPENDENCIES("${acl_LibName}" ${acl_DependentTargets})
    ENDIF(NOT "${acl_DependentTargets}" STREQUAL "")
  ENDIF(csc_EXECUTABLE)
ENDMACRO(ADD_CSHARP_LIBRARY)


MACRO(ADD_CSHARP_TEST testname)
  IF(mono_EXECUTABLE)
    ADD_TEST(${testname} ${mono_EXECUTABLE} ${ARGN})
  ELSE(mono_EXECUTABLE)
    ADD_TEST(${testname} ${ARGN})
  ENDIF(mono_EXECUTABLE)
ENDMACRO(ADD_CSHARP_TEST)


MACRO(ADD_EXPORTLAYER_LIBRARY ael_WrappedLibName ael_DependentLibs)
  IF("${ARGN}" STREQUAL "")
    MESSAGE(FATAL_ERROR "ADD_EXPORTLAYER_LIBRARY requires at least one optional argument.")
  ENDIF("${ARGN}" STREQUAL "")

  ADD_LIBRARY(
    "${ael_WrappedLibName}.Unmanaged" SHARED
    ${ARGN}
    )

  TARGET_LINK_LIBRARIES("${ael_WrappedLibName}.Unmanaged" ${ael_DependentLibs})

  ADD_DEPENDENCIES("${ael_WrappedLibName}.Unmanaged" "${ael_WrappedLibName}")
ENDMACRO(ADD_EXPORTLAYER_LIBRARY)


MACRO(WRAP_CLASSES_FOR_CSHARP wcfc_WrappedLibName wcfc_DependentTargets wcfc_DependentLibs)
  IF("${ARGN}" STREQUAL "")
    MESSAGE(FATAL_ERROR "WRAP_CLASSES_FOR_CSHARP requires at least one optional argument.")
  ENDIF("${ARGN}" STREQUAL "")

  SET(wcfc_SrcDir "${CMAKE_CURRENT_SOURCE_DIR}")
  SET(wcfc_BinDir "${CMAKE_CURRENT_BINARY_DIR}")

  CONFIGURE_FILE(
    "${wcfc_SrcDir}/MummySettings.xml.in"
    "${wcfc_BinDir}/xml/${wcfc_WrappedLibName}.MummySettings.xml"
    @ONLY
  )

  CONFIGURE_FILE(
    "${wcfc_SrcDir}/WrappedObject.cs.in"
    "${wcfc_BinDir}/csharp/${wcfc_WrappedLibName}.WrappedObject.cs"
    @ONLY
  )

  MAKE_DIRECTORY("${wcfc_BinDir}/export-layer")

  SET(wcfc_CsharpFiles "${wcfc_BinDir}/csharp/${wcfc_WrappedLibName}.WrappedObject.cs")
  SET(wcfc_ExportLayerFiles "")
  SET(wcfc_GccxmlIncludes "")

IF(${gccxml_EXECUTABLE} MATCHES "castxml")
    SET(executable_output -o ${wcfc_BinDir})

	IF(MSVC)
		SET(compiler msvc)
	ELSE(MSVC)
		SET(compiler gnu)
	ENDIF(MSVC)

	SET(executable_options --castxml-start _cable_ --castxml-gccxml --castxml-cc-${compiler} ${CMAKE_CXX_COMPILER} -D__CASTXML__)
ELSE(${gccxml_EXECUTABLE} MATCHES "castxml")
    SET(executable_output -fxml=${wcfc_BinDir})
	SET(executable_options -fxml-start=_cable_ --gccxml-compiler ${CMAKE_CXX_COMPILER})
ENDIF(${gccxml_EXECUTABLE} MATCHES "castxml")

  FOREACH(instruction ${ARGN})
    IF("${instruction}" MATCHES "(.+),(.+)")
      STRING(REGEX REPLACE "(.+),(.+)" "\\1" wcfc_Class "${instruction}")
      STRING(REGEX REPLACE "(.+),(.+)" "\\2" wcfc_Header "${instruction}")
    ELSE("${instruction}" MATCHES "(.+),(.+)")
      MESSAGE(FATAL_ERROR "WRAP_CLASSES_FOR_CSHARP arg instruction='${instruction}' does not match expected regular expression. Optional arguments to WRAP_CLASSES_FOR_CSHARP should follow the pattern 'class,header'...")
    ENDIF("${instruction}" MATCHES "(.+),(.+)")

    STRING(REGEX REPLACE "::" "_" wcfc_FileBaseName "${wcfc_Class}")

    IF("${wcfc_Class}" MATCHES "^.*::([^:]+)$")
      STRING(REGEX REPLACE "^.*::([^:]+)$" "\\1" wcfc_ClassName "${wcfc_Class}")
    ELSE("${wcfc_Class}" MATCHES "^.*::([^:]+)$")
      SET(wcfc_ClassName "${wcfc_Class}")
    ENDIF("${wcfc_Class}" MATCHES "^.*::([^:]+)$")

    SET(wcfc_GccxmlFile "/xml/${wcfc_FileBaseName}.xml")
    SET(wcfc_CsharpFile "${wcfc_BinDir}/csharp/${wcfc_FileBaseName}.cs")
    SET(wcfc_ExportLayerFile "${wcfc_BinDir}/export-layer/${wcfc_FileBaseName}EL.cxx")

    CONFIGURE_FILE(
      "${wcfc_SrcDir}/gccxml.cxx.in"
      "${wcfc_BinDir}/xml/${wcfc_FileBaseName}_gccxml.cxx"
      @ONLY
    )

    ADD_CUSTOM_COMMAND(
      OUTPUT ${wcfc_BinDir}${wcfc_GccxmlFile}
      COMMAND ${gccxml_EXECUTABLE}
      ARGS
		${executable_output}${wcfc_GccxmlFile}
		${executable_options}
        ${wcfc_GccxmlIncludes}
        -DCABLE_CONFIGURATION
        ${wcfc_BinDir}/xml/${wcfc_FileBaseName}_gccxml.cxx
      DEPENDS
        ${wcfc_BinDir}/xml/${wcfc_FileBaseName}_gccxml.cxx
        ${wcfc_Header}
        ${gccxml_EXECUTABLE}
      )

    ADD_CUSTOM_COMMAND(
      OUTPUT ${wcfc_CsharpFile} ${wcfc_ExportLayerFile}
      COMMAND ${mummy_EXECUTABLE}
      ARGS
        --settings-file ${wcfc_BinDir}/xml/${wcfc_WrappedLibName}.MummySettings.xml
        --gccxml-file ${wcfc_BinDir}${wcfc_GccxmlFile}
        --csharp-file ${wcfc_CsharpFile}
        --export-layer-file ${wcfc_ExportLayerFile}
      DEPENDS
        ${wcfc_BinDir}/xml/${wcfc_WrappedLibName}.MummySettings.xml
        ${wcfc_BinDir}${wcfc_GccxmlFile}
        ${mummy_EXECUTABLE}
      )

    SET(wcfc_CsharpFiles ${wcfc_CsharpFiles} "${wcfc_CsharpFile}")
    SET(wcfc_ExportLayerFiles ${wcfc_ExportLayerFiles} "${wcfc_ExportLayerFile}")
  ENDFOREACH(instruction)

  ADD_CSHARP_LIBRARY(${wcfc_WrappedLibName} ${wcfc_DependentTargets} "" "" "" ${wcfc_CsharpFiles})

  ADD_EXPORTLAYER_LIBRARY(${wcfc_WrappedLibName} ${wcfc_DependentLibs} "${wcfc_ExportLayerFiles}")
ENDMACRO(WRAP_CLASSES_FOR_CSHARP)
