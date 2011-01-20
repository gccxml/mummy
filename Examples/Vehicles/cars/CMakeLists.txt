SET(deps "base")

DEFINE_KIT(
  "cars" deps
  "AbstractCar"
  "Mustang"
)

ADD_LIBRARY(cars ${cars_source_files})

IF(Vehicles_INSTALL_TARGETS)
  INSTALL(TARGETS cars RUNTIME DESTINATION "." LIBRARY DESTINATION ".")
ENDIF(Vehicles_INSTALL_TARGETS)

FOREACH(d ${deps})
  TARGET_LINK_LIBRARIES(cars "${${d}_libs}")
ENDFOREACH(d)
