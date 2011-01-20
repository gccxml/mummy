SET(deps "base")

DEFINE_KIT(
  "suvs" deps
  "GenericSUV"
)

ADD_LIBRARY(suvs ${suvs_source_files})

IF(Vehicles_INSTALL_TARGETS)
  INSTALL(TARGETS suvs RUNTIME DESTINATION "." LIBRARY DESTINATION ".")
ENDIF(Vehicles_INSTALL_TARGETS)

FOREACH(d ${deps})
  TARGET_LINK_LIBRARIES(suvs "${${d}_libs}")
ENDFOREACH(d)
