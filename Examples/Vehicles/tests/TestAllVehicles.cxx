#include "TestAllVehicles.h"

#include "Vehicle.h"
#include "Mustang.h"
#include "GenericMotorCycle.h"
#include "GenericSUV.h"
#include "RefCountedVehicle.h"
#include "UnwrappedClass.h"

#include <iostream>

int PrintAbstractVehicle(const char *context, AbstractVehicle *av)
{
  std::cerr << "context: " << context << std::endl;
  std::cerr << "  av: " << av << std::endl;
  std::cerr << "  GetVIN: " << (char*)(av->GetVIN() ? av->GetVIN() : "(null)") << std::endl;
  std::cerr << "  GetVehicleType: " << av->GetVehicleType() << std::endl;
  std::cerr << "  GetMileage: " << av->GetMileage() << std::endl;
  std::cerr << std::endl;

  return 0;
}

int TestAllVehicles::Main(int argc, char* argv[])
{
  int err = 0; // 0 == no error

  std::cerr << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << std::endl;
  std::cerr << "Begin TestAllVehicles::Main" << std::endl;

  Vehicle* vn = Vehicle::New();
  Vehicle& v1 = *vn;

  PrintAbstractVehicle("Vehicle", &v1);


  Mustang* mn = Mustang::New();
  Mustang& v2 = *mn;

  v2.SetVIN("non-default special VIN");
  PrintAbstractVehicle("Mustang", &v2);
  v2.StillWontBeWrapped(argc, argv, argv);


  GenericMotorCycle* mc3 =  GenericMotorCycle::New();
  GenericMotorCycle& v3 = *mc3;

  v3.SetVIN(0);
  PrintAbstractVehicle("GenericMotorCycle", &v3);


  GenericSUV* sn = GenericSUV::New();
  GenericSUV& v4 = *sn;

  v4.SetVIN(0);
  PrintAbstractVehicle("GenericSUV", &v4);


  std::cerr << "Calling RefCountedVehicle methods..." << std::endl;
  RefCountedVehicle* rcv = RefCountedVehicle::Create();
  rcv->MethodExcludedByBTXETX();
  rcv->Destroy();
  rcv = 0;


  UnwrappedClass* uwc = v4.YouCannotCallThisMethodFromTheWrappedLayer();
  delete uwc;

  v1.Delete();
  v2.Delete();
  v3.Delete();
  v4.Delete();

  std::cerr << "End TestAllVehicles::Main" << std::endl;

  return err;
}
