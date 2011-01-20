SET(deps "base")

DEFINE_KIT(
  "motorcycles" deps
  "GenericMotorCycle"
)

ADD_LIBRARY(motorcycles ${motorcycles_source_files})

IF(Vehicles_INSTALL_TARGETS)
  INSTALL(TARGETS motorcycles RUNTIME DESTINATION "." LIBRARY DESTINATION ".")
ENDIF(Vehicles_INSTALL_TARGETS)

FOREACH(d ${deps})
  TARGET_LINK_LIBRARIES(motorcycles "${${d}_libs}")
ENDFOREACH(d)
