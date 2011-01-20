#include "TestCallback.h"

#include "Vehicle.h"
#include "FuelLevelChanged.h"

#include <iostream>

void SDE_CC TheCallback(AbstractVehicle *sender, unsigned long eid, void *clientdata, const void *calldata)
{
  std::cerr << "TheCallback" << std::endl;
  std::cerr << "  sender: " << sender << std::endl;
  std::cerr << "  eid: " << eid << std::endl;
  std::cerr << "  clientdata: " << clientdata << std::endl;
  std::cerr << "  calldata: " << calldata << std::endl;

  if (calldata)
    {
    if (1 == eid)
      {
      std::cerr << "  interpreted calldata: '" << * (const int *) calldata << "'" << std::endl;
      }
    if (2 == eid)
      {
      std::cerr << "  interpreted calldata: '" << (const char *) calldata << "'" << std::endl;
      }
    if (3 == eid)
      {
      std::cerr << "  interpreted calldata: '" << * (const double *) calldata << "'" << std::endl;
      }
    if (4 == eid)
      {
      std::cerr << "  interpreted calldata: '" << ((AbstractVehicle *) calldata)->GetVIN() << "'" << std::endl;
      }
    }
}

void SDE_CC GlobalFuelLevelChangedCallback(AbstractVehicle* sender, FuelLevelChangedEventArgs* data)
{
  std::cerr << "GlobalFuelLevelChangedCallback" << std::endl;
  std::cerr << "  VIN: " << sender->GetVIN() << std::endl;
  std::cerr << "  oldlevel: " << data->GetOldLevel() << std::endl;
  std::cerr << "  level: " << data->GetLevel() << std::endl;
}

void TestCallback::FuelLevelChangedCallback(AbstractVehicle* sender, FuelLevelChangedEventArgs* data)
{
  std::cerr << "TestCallback::FuelLevelChangedCallback" << std::endl;
  std::cerr << "  VIN: " << sender->GetVIN() << std::endl;
  std::cerr << "  oldlevel: " << data->GetOldLevel() << std::endl;
  std::cerr << "  level: " << data->GetLevel() << std::endl;
}

TestCallback::TestCallback()
{
}

TestCallback::~TestCallback()
{
}

int TestCallback::Main(int argc, char *argv[])
{
  int err = 0; // 0 == no error

  std::cerr << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << std::endl;
  std::cerr << "Begin TestCallback::Main" << std::endl;

  int ai = 0;
  for (ai= 0; ai<argc; ++ai)
    {
    std::cerr << "  arg" << ai << ": " << argv[ai] << std::endl;
    }

  Vehicle *vn = Vehicle::New();
  Vehicle& v1 = *vn;

  unsigned int i = 0;
  unsigned int maxi = v1.GetMaxIndex();

  std::cerr << "Set/GetValue loop..." << std::endl;
  v1.TSetCallback(TheCallback, 0);
  for (i= 0; i<maxi; ++i)
    {
    v1.TSetValue(i, v1.TGetValue(i+1));
    if (0 == i)
      {
      v1.TSetCallback(0, 0);
      }
    }
  std::cerr << "  i: " << i << std::endl;

  std::cerr << "Set/GetDoubleValue loop..." << std::endl;
  v1.TSetCallback(TheCallback, 0);
  for (i= 0; i<maxi; ++i)
    {
    v1.TSetDoubleValue(i, v1.TGetDoubleValue(i+1));
    if (0 == i)
      {
      v1.TSetCallback(0, 0);
      }
    }
  std::cerr << "  i: " << i << std::endl;

  std::cerr << "Set/GetStringValue loop..." << std::endl;
  v1.TSetCallback(TheCallback, 0);
  for (i= 0; i<maxi; ++i)
    {
    v1.TSetStringValue(i, v1.TGetStringValue(i+1));
    if (0 == i)
      {
      v1.TSetCallback(0, 0);
      }
    }
  std::cerr << "  i: " << i << std::endl;

  std::cerr << "Set/GetObjectValue loop..." << std::endl;
  v1.TSetCallback(TheCallback, 0);
  for (i= 0; i<maxi; ++i)
    {
    v1.TSetObjectValue(i, v1.TGetObjectValue(i+1));
    if (0 == i)
      {
      v1.TSetCallback(0, 0);
      }
    }
  std::cerr << "  i: " << i << std::endl;

  std::cerr << "MethodWithBoolRefArg loop..." << std::endl;
  for (i= 0; i<30; ++i)
    {
    bool arg = false;
    v1.MethodWithBoolRefArg(arg);
    std::cerr << "  i: " << i << "  --  arg: " << (arg ? "true" : "false") << std::endl;
    }

  std::cerr << "Test FuelLevelChangedCallback" << std::endl;
  TestCallback tcb1;
  std::cerr << "Initial v1.GetFuelLevel(): " << v1.GetFuelLevel() << std::endl;
  v1.SetFuelLevel(0.5);
  v1.SetFuelLevel(0.0);
  v1.SetFuelLevel(0.5);
  unsigned int h1id = v1.TGetFuelLevelChangedEvent()->attach(&tcb1, &TestCallback::FuelLevelChangedCallback);
  unsigned int h2id = v1.TGetFuelLevelChangedEvent()->attach(GlobalFuelLevelChangedCallback);
  v1.SetFuelLevel(0.75);
  v1.SetFuelLevel(0.75);
  v1.SetFuelLevel(1.0);
  v1.SetFuelLevel(1.0);
  v1.SetFuelLevel(1.0);

  // Call these "I exist only for gccxml testing of signatures and attributes"
  // methods just to improve coverage results:
  //
  v1.TGetFuelLevelChangedEvent()->ReturnMyLong();
  v1.TGetFuelLevelChangedEvent()->ReturnMyNonClassLong();

  v1.TGetFuelLevelChangedEvent()->detach(h1id);
  v1.TGetFuelLevelChangedEvent()->detach(h2id);

  v1.Delete();

  std::cerr << "End TestCallback::Main" << std::endl;

  return err;
}
