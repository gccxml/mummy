#ifndef _TestCallback_h
#define _TestCallback_h

#include "VehiclesConfig.h"

class AbstractVehicle;
class FuelLevelChangedEventArgs;

// TestCallback is a "yet to be documented" example class...

class tests_dll TestCallback
{
public:
  static iwhTestIWH int Main(int argc, char *argv[]);

protected:
  // protected constructor/destructor means only instantiable from within Main:
  TestCallback();
  virtual ~TestCallback();

  // protected means it won't be wrapped:
  virtual void FuelLevelChangedCallback(AbstractVehicle* sender, FuelLevelChangedEventArgs* data);

private:
  // intentionally unimplemented to prevent copying:
  TestCallback(const TestCallback& other);
  TestCallback& operator=(const TestCallback& other);
};

#endif
