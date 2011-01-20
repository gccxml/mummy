#ifndef _TestPerformance_h
#define _TestPerformance_h

#include "VehiclesConfig.h"

// TestPerformance is a "yet to be documented" example class...

class tests_dll TestPerformance
{
public:
  static int Main(int argc, char *argv[]);

private:
  // intentionally unimplemented to prevent instantiating and copying:
  TestPerformance();
  TestPerformance(const TestPerformance& other);
  TestPerformance& operator=(const TestPerformance& other);
};

#endif
