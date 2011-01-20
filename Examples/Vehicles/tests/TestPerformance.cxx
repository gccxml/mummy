#include "TestPerformance.h"

#include "Vehicle.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h> // WIN32 SDK GetTickCount function
#else
unsigned int GetTickCount()
{
  std::cerr << "warning: fake GetTickCount used - timing results are not correct...\n";
  return 0;
}
#endif

#define TicksPerSecond 1000

void ReportTiming(const char *s, unsigned int ts, unsigned int count)
{
  std::cerr << "\n";
  std::cerr << "Unmanaged Code Performance Results:\n";
  std::cerr << "  data type: " << s << "\n";
  std::cerr << "  seconds: " << ((double)ts / (double)TicksPerSecond) << "\n";
  //std::cerr << "  ticks: " << ts << "\n";
  std::cerr << "  count: " << count << "\n";
  //std::cerr << "  ticks per operation: " << ((double)ts/(double)count) << "\n";
  std::cerr << "  operations per second: " << ((double)count / (((double)ts / (double)TicksPerSecond))) << "\n";
  std::cerr << "\n";
}

int TestPerformance::Main(int /*argc*/, char * /*argv*/ [])
{
  int err = 0; // 0 == no error
  unsigned int ts = 0;
  unsigned int begin = 0;

  std::cerr << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << std::endl;
  std::cerr << "Begin TestPerformance::Main" << std::endl;

  Vehicle *vn = Vehicle::New();
  Vehicle& v1 = *vn;

  unsigned int i = 0;
  unsigned int maxi = v1.GetMaxIndex();

  std::cerr << "Set/GetValue loop..." << std::endl;
  begin = GetTickCount();
  for (i= 0; i<maxi; ++i)
    {
    v1.TSetValue(i, v1.TGetValue(i+1));
    }
  ts = GetTickCount() - begin;
  ReportTiming("int", ts, maxi);
  std::cerr << "  i: " << i << std::endl;

  std::cerr << "Set/GetDoubleValue loop..." << std::endl;
  begin = GetTickCount();
  for (i= 0; i<maxi; ++i)
    {
    v1.TSetDoubleValue(i, v1.TGetDoubleValue(i+1));
    }
  ts = GetTickCount() - begin;
  ReportTiming("double", ts, maxi);
  std::cerr << "  i: " << i << std::endl;

  std::cerr << "Set/GetStringValue loop..." << std::endl;
  begin = GetTickCount();
  for (i= 0; i<maxi; ++i)
    {
    v1.TSetStringValue(i, v1.TGetStringValue(i+1));
    }
  ts = GetTickCount() - begin;
  ReportTiming("char* (string)", ts, maxi);
  std::cerr << "  i: " << i << std::endl;

  std::cerr << "Set/GetObjectValue loop..." << std::endl;
  begin = GetTickCount();
  for (i= 0; i<maxi; ++i)
    {
    v1.TSetObjectValue(i, v1.TGetObjectValue(i+1));
    }
  ts = GetTickCount() - begin;
  ReportTiming("AbstractVehicle* (object)", ts, maxi);
  std::cerr << "  i: " << i << std::endl;

  v1.Delete();

  std::cerr << "End TestPerformance::Main" << std::endl;

  return err;
}
