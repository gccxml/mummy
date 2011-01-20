SET(deps "")

DEFINE_KIT(
  "base" deps
  "AbstractVehicle"
  "BlatMobile"
  "FuelLevelChanged"
  "FuelLevelChangedEventArgs"
  "Oops"
  "RefCountedVehicle"
  "SignatureSeries"
  "SimpleVehicle"
  "Vehicle"
  "VehicleDescriptor"
)

# Extra files (not wrapped, or force to show up in VS project UI, for example):
SET(base_source_files ${base_source_files}
  "AbstractVehicleEvent.h"
  "InlineWrapperHints.h"
  "SenderDataEvent.h"
  "SillyString.cxx"
  "SillyString.h"
  "UnwrappedClass.cxx"
  "UnwrappedClass.h"
)

ADD_LIBRARY(base ${base_source_files})

IF(Vehicles_INSTALL_TARGETS)
  INSTALL(TARGETS base RUNTIME DESTINATION "." LIBRARY DESTINATION ".")
ENDIF(Vehicles_INSTALL_TARGETS)
