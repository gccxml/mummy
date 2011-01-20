#ifndef _RefCountedVehicle_h
#define _RefCountedVehicle_h

#include "VehiclesConfig.h"

// RefCountedVehicle is a "yet to be documented" example class...

class base_dll RefCountedVehicle
{
public:
  static RefCountedVehicle* Create();
  virtual void Destroy();
  virtual void IncrRefCount(void* cookie);
  virtual void DecrRefCount(void* cookie);
  virtual iwhPropGet unsigned int GetRefCount() const;

  // Description:
  // These methods all return a RefCountedVehicle. Each one returns a
  // different sort of RefCountedVehicle. We return a this, a counted
  // "that" Vehicle, and a non-counted (contained) other vehicle.
  // The iwhCounted hint tells mummy that GetThatVehicle returns an
  // instance that is already ref-counted. A missing hint tells mummy
  // that the returned object is not ref-counted. Given this information,
  // mummy generates appropriate ref-counting code where needed.
  iwhPropGet const RefCountedVehicle* GetThisVehicle() const;
  iwhPropGet iwhCounted RefCountedVehicle* GetThatVehicle() const;
  iwhPropGet RefCountedVehicle* GetTheOtherVehicle() const;

  //----------------------------------------------------------------------------
  /// <summary>
  /// Methods to test iwhCounted/countedMethodsRegex equivalence
  /// </summary>
  /// <remarks>
  /// These methods are non-iwhCounted-hint methods that are aliases for the
  /// iwhCounted-hint method GetThatVehicle... We achieve the same effect
  /// as having the hint without modifying the C++ header or source by
  /// adding the method names to the MummySettings.xml countedMethodsRegex
  /// attribute...
  /// </remarks>
  iwhPropGet RefCountedVehicle* GetThatVehicleAlias() const;
  iwhPropGet RefCountedVehicle* GetThatVehicleByAnyOtherName() const;

  //RefCountedVehicle* GetNoHint() const;
  //void SetThis();

  // This method is commented with "XML 'special' characters" & stuff in order
  // to increase coverage results for the < EncodeStringForXml > function.
  static void Check();

  //BTX
  // This method will not be wrapped because of the //BTX //ETX VTK-style
  // wrapper exclusion comment markers.
  static void MethodExcludedByBTXETX();
  //ETX

protected:
  RefCountedVehicle();
  virtual ~RefCountedVehicle();

private:
  static int NumberOfConstructorCalls;
  static int NumberOfDestructorCalls;

  unsigned int m_RefCount;
  mutable RefCountedVehicle* m_OtherVehicle;
  mutable unsigned int m_CallCounter;
};

#endif
