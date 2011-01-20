#include "GenericSUV.h"

GenericSUV *GenericSUV::New()
{
  return new GenericSUV;
}

GenericSUV::GenericSUV()
{
}

GenericSUV::~GenericSUV()
{
}

const char *GenericSUV::VGetVehicleType() const
{
  return "SUV";
}

double GenericSUV::VGetMileage() const
{
  return 13.0;
}
