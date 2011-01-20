#ifndef _GenericMotorCycle_h
#define _GenericMotorCycle_h

#include "AbstractVehicle.h"

// GenericMotorCycle is a "yet to be documented" example class...

class motorcycles_dll GenericMotorCycle : public AbstractVehicle
{
public:
  static GenericMotorCycle *New();

protected:
  GenericMotorCycle();
  virtual ~GenericMotorCycle();

  virtual const char *VGetVehicleType() const;
  virtual double VGetMileage() const;
};

#endif
