#ifndef _AbstractVehicle_h
#define _AbstractVehicle_h

#include "VehiclesConfig.h"

#include "FuelLevelChanged.h"
#include "SillyString.h"
#include "VehicleDescriptor.h"

class UnwrappedClass;

//----------------------------------------------------------------------------
/// <summary>
/// Root class of the Vehicles example class hierarchy.
/// </summary>
class base_dll AbstractVehicle
{
public:
  //--------------------------------------------------------------------------
  /// <summary>
  /// Disposal method. Concrete subclasses are created via factory methods
  /// (static methods with no parameters that return the created object type
  /// and are named "New"). All instances created via a factory method must
  /// eventually be disposed of with a call to Delete.
  /// </summary>
  virtual void Delete();

  //--------------------------------------------------------------------------
  /// <summary>
  /// Get the type of the Vehicle.
  /// </summary>
  /// <returns>Vehicle type as a string.</returns>
  iwhPropGet const char *GetVehicleType() const;

  //--------------------------------------------------------------------------
  /// <summary>
  /// Get the average mileage (miles per gallon) of the Vehicle.
  /// </summary>
  /// <returns>Mileage measured in miles per gallon.</returns>
  iwhPropGet double GetMileage() const;

  //--------------------------------------------------------------------------
  /// <summary>
  /// Set the Vehicle ID number.
  /// </summary>
  /// <param name='vin'>The Vehicle ID number as a string.</param>
  iwhPropSet void SetVIN(const char *vin);

  //--------------------------------------------------------------------------
  /// <summary>
  /// Get the Vehicle ID number.
  /// </summary>
  /// <returns>The Vehicle ID number as a string.</returns>
  iwhPropGet const char *GetVIN() const;

  // Value accessors:

  //--------------------------------------------------------------------------
  iwhPropGet unsigned int GetMaxIndex() const;

  //--------------------------------------------------------------------------
  // Integers:
  virtual void TSetValue(const unsigned int index, const int val);
  virtual int TGetValue(const unsigned int index) const;

  //--------------------------------------------------------------------------
  // Strings:
  virtual void TSetStringValue(const unsigned int index, const char *val);
  virtual const char *TGetStringValue(const unsigned int index) const;

  //--------------------------------------------------------------------------
  // Doubles:
  virtual void TSetDoubleValue(const unsigned int index, const double val);
  virtual double TGetDoubleValue(const unsigned int index) const;

  //--------------------------------------------------------------------------
  // Objects:
  virtual void TSetObjectValue(const unsigned int index, const AbstractVehicle *val);
  virtual const AbstractVehicle *TGetObjectValue(const unsigned int index) const;

  //--------------------------------------------------------------------------
  // Event proof of concept:
#ifdef _WIN32
#define AVCB_CC __stdcall
#else
#define AVCB_CC
#endif

  typedef void (AVCB_CC *AVCallback)(AbstractVehicle *sender, unsigned long eid, void *clientdata, const void *calldata);
  virtual void TSetCallback(AVCallback f, void *clientdata);
  virtual AVCallback TGetCallback() const;
  virtual void* TGetClientData() const;

  //--------------------------------------------------------------------------
  // More object oriented event proof of concept:
  iwhPropSet void SetFuelLevel(double level);
  iwhPropGet double GetFuelLevel() const;
  virtual iwhEvent FuelLevelChanged *TGetFuelLevelChangedEvent() const;

  //--------------------------------------------------------------------------
  // Array access testing:
  #define InfoArraySize 37
  iwhPropSet void SetInfoArray(iwhArraySize(37) const float *info);
  iwhPropGet iwhArraySize(37) const float* GetInfoArray() const;
  virtual int TGetInfoArrayLength() const;
  virtual float* InfoArrayBase();
  virtual void TSetInfoArrayElementUnchecked(int i, float f);
  virtual float TGetInfoArrayElementUnchecked(int i) const;
  virtual void TSetInfoArrayElementChecked(int i, float f);
  virtual float TGetInfoArrayElementChecked(int i) const;

  //--------------------------------------------------------------------------
  // Return-struct-by-value test:
  virtual VehicleDescriptor TGetDescriptor() const;
  virtual void TSetDescriptor(VehicleDescriptor desc);
  virtual void TSetDescriptorRef(VehicleDescriptor &desc);
  virtual void TSetDescriptorConstRef(const VehicleDescriptor &desc);

  //--------------------------------------------------------------------------
  // Pass/return enum test:
  void TSetManufacturerID(VehicleDescriptor::ManufacturerID mid);
  VehicleDescriptor::ManufacturerID TGetManufacturerID();

  //BTX
  virtual int TGet_BTX_ETX_Int() const;
  //ETX

  //--------------------------------------------------------------------------
  virtual iwhExclude int Get_iwhExclude_Int() const;

  //--------------------------------------------------------------------------
  // bool method and property test:
  virtual bool TGetHasLicensePlate() const;
  virtual void TSetHasLicensePlate(bool hasLicensePlate);
  virtual iwhPropGet bool GetRegistered() const;
  virtual iwhPropSet void SetRegistered(bool registered);

  //--------------------------------------------------------------------------
  // string method and property test:
#if 0
  virtual void AppendToTextualDescription(const SillyString& text);
  virtual SillyString TextualDescription() const;
  virtual iwhPropGet SillyString GetRegisteredOwner() const;
  virtual iwhPropSet void SetRegisteredOwner(const SillyString& owner);
#endif

  //--------------------------------------------------------------------------
  // exclude UnwrappedClass test:
  virtual UnwrappedClass* YouCannotCallThisMethodFromTheWrappedLayer();

  //--------------------------------------------------------------------------
  // "bool &" arg test:
  virtual void MethodWithBoolRefArg(bool &arg);

  //--------------------------------------------------------------------------
protected:
  AbstractVehicle();
  virtual ~AbstractVehicle();

  virtual double VGetMileage() const = 0;
  virtual const char *VGetVehicleType() const = 0;

  //--------------------------------------------------------------------------
private:
  char *VIN;
  AVCallback Callback;
  void *ClientData;
  double FuelLevel;
  mutable FuelLevelChanged *FuelLevelChangedEventInstance;
  float InfoArray[InfoArraySize];
  bool HasLicensePlate;
  bool Registered;
#if 0
  SillyString RegisteredOwner;
  SillyString m_TextualDescription;
#endif
};

#endif
