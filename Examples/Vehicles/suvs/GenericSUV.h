#ifndef _GenericSUV_h
#define _GenericSUV_h

#include "AbstractVehicle.h"

// GenericSUV is a "yet to be documented" example class...

class suvs_dll GenericSUV : public AbstractVehicle
{
public:
  static GenericSUV *New();

protected:
  GenericSUV();
  virtual ~GenericSUV();

  virtual const char *VGetVehicleType() const;
  virtual double VGetMileage() const;
};

#endif
