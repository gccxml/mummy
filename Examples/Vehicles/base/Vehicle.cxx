#include "Vehicle.h"

Vehicle *Vehicle::New()
{
  return new Vehicle;
}

Vehicle::Vehicle()
{
}

Vehicle::~Vehicle()
{
}

const char *Vehicle::VGetVehicleType() const
{
  return "GenericVehicle";
}

double Vehicle::VGetMileage() const
{
  return -1.0;
}
