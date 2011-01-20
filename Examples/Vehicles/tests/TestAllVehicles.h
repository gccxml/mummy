#ifndef _TestAllVehicles_h
#define _TestAllVehicles_h

#include "VehiclesConfig.h"

// TestAllVehicles is a "yet to be documented" example class...

class tests_dll TestAllVehicles
{
public:
  static int Main(int argc, char *argv[]);

private:
  // intentionally unimplemented to prevent instantiating and copying:
  TestAllVehicles();
  TestAllVehicles(const TestAllVehicles& other);
  TestAllVehicles& operator=(const TestAllVehicles& other);
};

#endif
