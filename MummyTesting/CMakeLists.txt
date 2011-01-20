IF(csc_EXECUTABLE)
  # "/?" works with csc from Microsoft and its variations (gmcs and friends)
  # from Mono
  #
  ADD_TEST(HelpSmokeTest-csc "${csc_EXECUTABLE}" "/?")
ENDIF(csc_EXECUTABLE)

IF(gccxml_EXECUTABLE)
  ADD_TEST(HelpSmokeTest-gccxml "${gccxml_EXECUTABLE}" "--help")
  ADD_TEST(VersionSmokeTest-gccxml "${gccxml_EXECUTABLE}" "--version")
ENDIF(gccxml_EXECUTABLE)

IF(mono_EXECUTABLE)
  ADD_TEST(HelpSmokeTest-mono "${mono_EXECUTABLE}" "--help")
  ADD_TEST(VersionSmokeTest-mono "${mono_EXECUTABLE}" "--version")
ENDIF(mono_EXECUTABLE)

IF(EXECUTABLE_OUTPUT_PATH)
  SET(exefile ${EXECUTABLE_OUTPUT_PATH}/mummy)
ELSE(EXECUTABLE_OUTPUT_PATH)
  SET(exefile mummy)
ENDIF(EXECUTABLE_OUTPUT_PATH)

ADD_TEST(HelpSmokeTest-mummy "${exefile}" "--help")
ADD_TEST(VersionSmokeTest-mummy "${exefile}" "--version")

ADD_SUBDIRECTORY(HandCrafted)
ADD_SUBDIRECTORY(Simple)
ADD_SUBDIRECTORY(SuppressWarnings)
