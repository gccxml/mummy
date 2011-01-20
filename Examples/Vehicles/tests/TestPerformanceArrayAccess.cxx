#include "TestPerformanceArrayAccess.h"

#include "Vehicle.h"

#include <iostream>

#ifdef _WIN32
#include <windows.h> // WIN32 SDK GetTickCount function
#else
extern unsigned int GetTickCount();
#endif

#define TicksPerSecond 1000

static void ReportTiming(const char *s, unsigned int ts, unsigned int count)
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

float multiplyByTwo(float f);

int TestPerformanceArrayAccess::Main(int /*argc*/, char * /*argv*/ [])
{
  int err = 0; // 0 == no error
  unsigned int ts = 0;
  unsigned int begin = 0;

  std::cerr << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << std::endl;
  std::cerr << "Begin TestPerformanceArrayAccess::Main" << std::endl;
  std::cerr << std::endl;

  Vehicle *vn = Vehicle::New();
  Vehicle& v1 = *vn;

  unsigned int i = 0;
  unsigned int maxi = 10*v1.GetMaxIndex();
  const unsigned int length = v1.TGetInfoArrayLength();
  float *af = v1.InfoArrayBase();
  float lf = 0.0;


  std::cerr << "Direct C++ array write access 'af[i] = lf' loop..." << std::endl;
  begin = GetTickCount();
  for (i= 0; i<maxi; ++i)
    {
    af[i%length] = lf;
    }
  ts = GetTickCount() - begin;
  ReportTiming("float", ts, maxi);
  std::cerr << std::endl;


  std::cerr << "Unchecked vtable write access 'v1.SetInfoArrayElementUnchecked(i, lf)' loop..." << std::endl;
  begin = GetTickCount();
  for (i= 0; i<maxi; ++i)
    {
    v1.TSetInfoArrayElementUnchecked(i%length, lf);
    }
  ts = GetTickCount() - begin;
  ReportTiming("float", ts, maxi);
  std::cerr << std::endl;


  std::cerr << "Unchecked vtable read access 'lf = v1.GetInfoArrayElementUnchecked(i)' loop..." << std::endl;
  begin = GetTickCount();
  for (i= 0; i<maxi; ++i)
    {
    lf = v1.TGetInfoArrayElementUnchecked(i%length);
    }
  ts = GetTickCount() - begin;
  ReportTiming("float", ts, maxi);
  std::cerr << std::endl;


  std::cerr << "Checked vtable write access 'v1.SetInfoArrayElementChecked(i, lf)' loop..." << std::endl;
  begin = GetTickCount();
  for (i= 0; i<maxi; ++i)
    {
    v1.TSetInfoArrayElementChecked(i%length, lf);
    }
  ts = GetTickCount() - begin;
  ReportTiming("float", ts, maxi);
  std::cerr << std::endl;


  std::cerr << "Checked vtable read access 'lf = v1.GetInfoArrayElementChecked(i)' loop..." << std::endl;
  begin = GetTickCount();
  for (i= 0; i<maxi; ++i)
    {
    lf = v1.TGetInfoArrayElementChecked(i%length);
    }
  ts = GetTickCount() - begin;
  ReportTiming("float", ts, maxi);
  std::cerr << std::endl;


  std::cerr << "End TestPerformanceArrayAccess::Main" << std::endl;

  return err;
}

float multiplyByTwo(float f)
{
  return f*2;
}
