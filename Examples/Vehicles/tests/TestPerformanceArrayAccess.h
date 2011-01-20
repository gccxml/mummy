#ifndef _TestPerformanceArrayAccess_h
#define _TestPerformanceArrayAccess_h

#include "VehiclesConfig.h"

// TestPerformanceArrayAccess is a "yet to be documented" example class...

class tests_dll TestPerformanceArrayAccess
{
public:
  static int Main(int argc, char *argv[]);

private:
  // intentionally unimplemented to prevent instantiating and copying:
  TestPerformanceArrayAccess();
  TestPerformanceArrayAccess(const TestPerformanceArrayAccess& other);
  TestPerformanceArrayAccess& operator=(const TestPerformanceArrayAccess& other);
};

#endif
