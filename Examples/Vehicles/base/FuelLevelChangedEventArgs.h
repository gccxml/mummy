#ifndef _FUELLEVELCHANGEDEVENTARGS_H
#define _FUELLEVELCHANGEDEVENTARGS_H

#include "VehiclesConfig.h"

// FuelLevelChangedEventArgs is a "yet to be documented" example class...

class base_dll FuelLevelChangedEventArgs
{
public:
  iwhPropSet void SetOldLevel(double d) { this->OldLevel = d; }
  iwhPropGet double GetOldLevel() const { return this->OldLevel; }

  iwhPropSet void SetLevel(double d) { this->Level = d; }
  iwhPropGet double GetLevel() const { return this->Level; }

  static FuelLevelChangedEventArgs* New() { return new FuelLevelChangedEventArgs; }
  virtual void Delete() { delete this; }

protected:
  FuelLevelChangedEventArgs() {}
  virtual ~FuelLevelChangedEventArgs() {}

private:
  double OldLevel;
  double Level;
};

#endif // _FUELLEVELCHANGEDEVENTARGS_H
