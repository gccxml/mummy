#include "AbstractCar.h"

AbstractCar::AbstractCar()
{
}

AbstractCar::~AbstractCar()
{
}

const char *AbstractCar::VGetVehicleType() const
{
  return "Car";
}
