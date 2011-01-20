#ifndef _Vehicle_h
#define _Vehicle_h

#include "AbstractVehicle.h"

// Vehicle is a "yet to be documented" example class...

class base_dll Vehicle : public AbstractVehicle
{
public:
  static Vehicle *New();

protected:
  Vehicle();
  virtual ~Vehicle();

  virtual const char *VGetVehicleType() const;
  virtual double VGetMileage() const;
};

#endif
