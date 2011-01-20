#include "GenericMotorCycle.h"

GenericMotorCycle *GenericMotorCycle::New()
{
  return new GenericMotorCycle;
}

GenericMotorCycle::GenericMotorCycle()
{
}

GenericMotorCycle::~GenericMotorCycle()
{
}

const char *GenericMotorCycle::VGetVehicleType() const
{
  return "MotorCycle";
}

double GenericMotorCycle::VGetMileage() const
{
  return 70.71;
}
