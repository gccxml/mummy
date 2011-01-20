#ifndef _AbstractCar_h
#define _AbstractCar_h

#include "AbstractVehicle.h"

// AbstractCar is a "yet to be documented" example class...

class cars_dll AbstractCar : public AbstractVehicle
{
public:
  AbstractCar();
  virtual ~AbstractCar();

protected:
  virtual const char *VGetVehicleType() const;
};

#endif
