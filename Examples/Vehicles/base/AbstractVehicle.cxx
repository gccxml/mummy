#include "AbstractVehicle.h"
#include "FuelLevelChanged.h"
#include "UnwrappedClass.h"

#include <stdlib.h> // free
#include <string.h> // strdup and strlen

#include <exception>
#include <iostream>
#include <sstream>

#if _MSC_VER >= 1400
#define strdup _strdup
#endif

AbstractVehicle::AbstractVehicle()
{
  this->VIN = strdup("11223344556677889900");
  this->Callback = 0;
  this->ClientData = 0;
  //throw std::exception("ERROR: this is the string part of the exception");
  this->FuelLevel = 0.0;
  this->FuelLevelChangedEventInstance = 0;
  memset(this->InfoArray, 0, sizeof(this->InfoArray));
#if 0
  this->RegisteredOwner = "me";
  this->m_TextualDescription = "beat up old jalopy";
#endif
}

AbstractVehicle::~AbstractVehicle()
{
  if (this->VIN)
    {
    free(this->VIN);
    this->VIN = 0;
    }

  if (this->FuelLevelChangedEventInstance)
    {
    this->FuelLevelChangedEventInstance->Delete();
    this->FuelLevelChangedEventInstance = 0;
    }
}

void AbstractVehicle::Delete()
{
  delete this;
}

const char *AbstractVehicle::GetVehicleType() const
{
  return this->VGetVehicleType();
}

double AbstractVehicle::GetMileage() const
{
  return this->VGetMileage();
}

void AbstractVehicle::SetVIN(const char *vin)
{
  if (this->VIN)
    {
    free(this->VIN);
    }

  if (vin)
    {
    this->VIN = strdup(vin);
    }
  else
    {
    this->VIN = 0;
    }
}

const char *AbstractVehicle::GetVIN() const
{
  return this->VIN ? this->VIN : "";
}

unsigned int AbstractVehicle::GetMaxIndex() const
{
  return 9876543; // 9,876,543
}

void AbstractVehicle::TSetValue(const unsigned int index, const int val)
{
  if (index > GetMaxIndex())
    {
    throw std::exception();//"ERROR: index greater than GetMaxIndex() result");
    }

  if (this->Callback)
    {
    std::ostringstream oss;
    oss << "Unmanaged code: " << this << ", 1, " << this->ClientData << ", " << &val << std::endl;
    std::cerr << oss.str().c_str();

    (*this->Callback)(this, 1, this->ClientData, reinterpret_cast<const void *>(&val));
    }

  (void) val;
}

int AbstractVehicle::TGetValue(const unsigned int index) const
{
  if (index > GetMaxIndex())
    {
    throw std::exception();//"ERROR: index greater than GetMaxIndex() result");
    }

  return 33;
}

void AbstractVehicle::TSetStringValue(const unsigned int index, const char *val)
{
  if (index > GetMaxIndex())
    {
    throw std::exception();//"ERROR: index greater than GetMaxIndex() result");
    }

  if (this->Callback)
    {
    std::ostringstream oss;
    oss << "Unmanaged code: " << this << ", 2, " << this->ClientData << ", " << val << std::endl;
    std::cerr << oss.str().c_str();

    (*this->Callback)(this, 2, this->ClientData, reinterpret_cast<const void *>(val));
    }

  (void) val;
}

const char *AbstractVehicle::TGetStringValue(const unsigned int index) const
{
  if (index > GetMaxIndex())
    {
    throw std::exception();//"ERROR: index greater than GetMaxIndex() result");
    }

  return "StringValue is 33 and a third";
}

void AbstractVehicle::TSetDoubleValue(const unsigned int index, const double val)
{
  if (index > GetMaxIndex())
    {
    throw std::exception();//"ERROR: index greater than GetMaxIndex() result");
    }

  if (this->Callback)
    {
    std::ostringstream oss;
    oss << "Unmanaged code: " << this << ", 3, " << this->ClientData << ", " << &val << std::endl;
    std::cerr << oss.str().c_str();

    (*this->Callback)(this, 3, this->ClientData, reinterpret_cast<const void *>(&val));
    }

  (void) val;
}

double AbstractVehicle::TGetDoubleValue(const unsigned int index) const
{
  if (index > GetMaxIndex())
    {
    throw std::exception();//"ERROR: index greater than GetMaxIndex() result");
    }

  return 33.0 + 1.0/3.0;
}

void AbstractVehicle::TSetObjectValue(const unsigned int index, const AbstractVehicle *val)
{
  if (index > GetMaxIndex())
    {
    throw std::exception();//"ERROR: index greater than GetMaxIndex() result");
    }

  if (this->Callback)
    {
    std::ostringstream oss;
    oss << "Unmanaged code: " << this << ", 4, " << this->ClientData << ", " << val << std::endl;
    std::cerr << oss.str().c_str();

    (*this->Callback)(this, 4, this->ClientData, reinterpret_cast<const void *>(val));
    }

  (void) val;
}

const AbstractVehicle *AbstractVehicle::TGetObjectValue(const unsigned int index) const
{
  if (index > GetMaxIndex())
    {
    throw std::exception();//"ERROR: index greater than GetMaxIndex() result");
    }

  static int TGetObjectValueCounter = 0;
  ++TGetObjectValueCounter;

  // Alternate between returning valid pointers and NULL pointers
  // for complete branch coverage in the export layer... (Call multiple
  // times sequentially from the wrapped side for complete coverage...)
  //
  if (0 != TGetObjectValueCounter%2)
    {
    return 0;
    }

  return this;
}

void AbstractVehicle::TSetCallback(AVCallback f, void *clientdata)
{
  this->Callback = f;
  this->ClientData = clientdata;
}

AbstractVehicle::AVCallback AbstractVehicle::TGetCallback() const
{
  return this->Callback;
}

void* AbstractVehicle::TGetClientData() const
{
  return this->ClientData;
}

void AbstractVehicle::SetFuelLevel(double level)
{
  if (this->FuelLevel != level)
    {
    double oldLevel = this->FuelLevel;

    this->FuelLevel = level;

    if (this->FuelLevelChangedEventInstance)
      {
      FuelLevelChangedEventArgs* data = FuelLevelChangedEventArgs::New();
      data->SetOldLevel(oldLevel);
      data->SetLevel(level);
      this->FuelLevelChangedEventInstance->Invoke(this, data);

      // intentional mem leak for now - since adding TheTable, duplicate pointer
      // values get messed up with non-ref-counted systems... doesn't matter for
      // VTK or other ref-count-based object hierarchies... but does for us...
      // so live with this small mem-leak for now:
      //

      data->Delete();
      }
    }
}

double AbstractVehicle::GetFuelLevel() const
{
  return this->FuelLevel;
}

FuelLevelChanged *AbstractVehicle::TGetFuelLevelChangedEvent() const
{
  if (0 == this->FuelLevelChangedEventInstance)
  {
    this->FuelLevelChangedEventInstance = FuelLevelChanged::New();
  }

  return this->FuelLevelChangedEventInstance;
}


void AbstractVehicle::SetInfoArray(const float *info)
{
  memcpy(this->InfoArray, info, sizeof(this->InfoArray));
}

const float* AbstractVehicle::GetInfoArray() const
{
  return this->InfoArray;
}

int AbstractVehicle::TGetInfoArrayLength() const
{
  return sizeof(this->InfoArray)/sizeof(this->InfoArray[0]);
}

float* AbstractVehicle::InfoArrayBase()
{
  return this->InfoArray;
}

void AbstractVehicle::TSetInfoArrayElementUnchecked(int i, float f)
{
  this->InfoArray[i] = f;
}

float AbstractVehicle::TGetInfoArrayElementUnchecked(int i) const
{
  return this->InfoArray[i];
}

void AbstractVehicle::TSetInfoArrayElementChecked(int i, float f)
{
  if (i>=0 && i<this->TGetInfoArrayLength())
  {
    this->InfoArray[i] = f;
  }
  else
  {
    throw new std::exception();
  }
}

float AbstractVehicle::TGetInfoArrayElementChecked(int i) const
{
  if (i>=0 && i<this->TGetInfoArrayLength())
  {
    return this->InfoArray[i];
  }
  else
  {
    throw new std::exception();
  }
}

VehicleDescriptor AbstractVehicle::TGetDescriptor() const
{
  VehicleDescriptor desc;

  desc.AMarker = 12345;
  desc.BoolTest1 = true;
  desc.BoolTest2 = false;
  desc.c1 = 0x11;
  desc.ColorCode = 55;
  desc.d1 = 222222222222.222222222222;
  desc.f1 = 333333.333333f;
  desc.i1 = 0x44444444;
  desc.i64 = 0x5555555555555555LL;
  desc.l1 = 0x66666666;
  desc.MileageEstimate = 21.21f;
  desc.NumberOfCylinders = 4;
  desc.NumberOfDoors = 3;
  desc.s1 = 0x7777;
  desc.uc1 = 0x88;
  desc.ui1 = 0x99999999;
  desc.ui64 = 0xAAAAAAAAAAAAAAAAULL;
  desc.ul1 = 0xBBBBBBBB;
  desc.us1 = 0xDDDD;
  desc.enumMfgID = VehicleDescriptor::Honda;
  desc.ZMarker = 67890;

  return desc;
}

void AbstractVehicle::TSetDescriptor(VehicleDescriptor desc)
{
  VehicleDescriptor desc2 = desc;
  desc2.ZMarker = desc2.ZMarker;
}

void AbstractVehicle::TSetDescriptorRef(VehicleDescriptor &desc)
{
  VehicleDescriptor desc2 = desc;
  desc2.ZMarker = desc2.ZMarker;
}

void AbstractVehicle::TSetDescriptorConstRef(const VehicleDescriptor &desc)
{
  VehicleDescriptor desc2 = desc;
  desc2.ZMarker = desc2.ZMarker;
}

int AbstractVehicle::TGet_BTX_ETX_Int() const
{
  return 0;
}

int AbstractVehicle::Get_iwhExclude_Int() const
{
  return 0;
}

bool AbstractVehicle::TGetHasLicensePlate() const
{
  return this->HasLicensePlate;
}

void AbstractVehicle::TSetHasLicensePlate(bool hasLicensePlate)
{
  this->HasLicensePlate = hasLicensePlate;
}

bool AbstractVehicle::GetRegistered() const
{
  return this->Registered;
}

void AbstractVehicle::SetRegistered(bool registered)
{
  this->Registered = registered;
}

#if 0
void AbstractVehicle::AppendToTextualDescription(const SillyString& text)
{
  this->m_TextualDescription += "\n";
  this->m_TextualDescription += "\n";
  this->m_TextualDescription += text;
}

SillyString AbstractVehicle::TextualDescription() const
{
  return this->m_TextualDescription;
}

SillyString AbstractVehicle::GetRegisteredOwner() const
{
  return this->RegisteredOwner;
}

void AbstractVehicle::SetRegisteredOwner(const SillyString& owner)
{
  this->RegisteredOwner = owner;
}
#endif

UnwrappedClass* AbstractVehicle::YouCannotCallThisMethodFromTheWrappedLayer()
{
  return new UnwrappedClass;
}

void AbstractVehicle::TSetManufacturerID(VehicleDescriptor::ManufacturerID mid)
{
  VehicleDescriptor desc;
  desc.enumMfgID = mid;
}

VehicleDescriptor::ManufacturerID AbstractVehicle::TGetManufacturerID()
{
  return VehicleDescriptor::Ford;
}

void AbstractVehicle::MethodWithBoolRefArg(bool &arg)
{
  static int MethodWithBoolRefArg_Counter = 0;
  ++MethodWithBoolRefArg_Counter;

  // Alternate between "returning" true, false and unmodified
  // for complete branch coverage in the export layer... (Call multiple
  // times sequentially from the wrapped side for complete coverage...)
  //
  switch (MethodWithBoolRefArg_Counter%3)
    {
    case 0:
      arg = true;
    break;

    case 1:
      arg = false;
    break;

    default:
    break;
    }
}
