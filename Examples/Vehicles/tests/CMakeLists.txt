SET(deps "base" "cars" "motorcycles" "suvs")

DEFINE_KIT(
  "tests" deps
  "TestAllVehicles"
  "TestCallback"
  "TestPerformance"
  "TestPerformanceArrayAccess"
)

ADD_LIBRARY(tests ${tests_source_files})

IF(Vehicles_INSTALL_TARGETS)
  INSTALL(TARGETS tests RUNTIME DESTINATION "." LIBRARY DESTINATION ".")
ENDIF(Vehicles_INSTALL_TARGETS)

FOREACH(d ${deps})
  TARGET_LINK_LIBRARIES(tests "${${d}_libs}")
ENDFOREACH(d)


INCLUDE_DIRECTORIES(${tests_include_dirs})
FOREACH(test ${tests_classes})
  CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/TestMain.cxx.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${test}Main.cxx"
    @ONLY
    )

  ADD_EXECUTABLE(${test} "${CMAKE_CURRENT_BINARY_DIR}/${test}Main.cxx")

  IF(Vehicles_INSTALL_TARGETS)
    INSTALL(TARGETS ${test} RUNTIME DESTINATION ".")
  ENDIF(Vehicles_INSTALL_TARGETS)

  TARGET_LINK_LIBRARIES(${test} tests)

  IF(EXECUTABLE_OUTPUT_PATH)
    SET(exefile ${EXECUTABLE_OUTPUT_PATH}/${test})
  ELSE(EXECUTABLE_OUTPUT_PATH)
    SET(exefile ${test})
  ENDIF(EXECUTABLE_OUTPUT_PATH)

  IF(Mummy_RUN_LONG_TESTS)
    ADD_TEST(${test} "${exefile}")
  ELSE(Mummy_RUN_LONG_TESTS)
    IF("${test}" STREQUAL "TestAllVehicles")
      ADD_TEST(${test} "${exefile}")
    ENDIF("${test}" STREQUAL "TestAllVehicles")
  ENDIF(Mummy_RUN_LONG_TESTS)
ENDFOREACH(test)
