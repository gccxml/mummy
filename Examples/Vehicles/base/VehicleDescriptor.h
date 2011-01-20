#ifndef _VehicleDescriptor_h
#define _VehicleDescriptor_h

#include "VehiclesConfig.h"

// VehicleDescriptor is a "yet to be documented" example class...

class iwhUtility base_dll VehicleDescriptor
{
public:
  //--------------------------------------------------------------------------
  /// <summary>
  /// Test "enum" wrapping
  /// </summary>
  enum ManufacturerID
  {
    Unknown,
    Ford = 1001,
    GM,
    Chrysler,
    BMW,
    Honda,
    Nissan,
    Toyota,
    EtcEtcEtc
  };

  //--------------------------------------------------------------------------
  /// <summary>
  /// Fields
  /// </summary>
  int ZMarker;
  int ColorCode;
  float MileageEstimate;
  bool BoolTest1;
  int NumberOfDoors;
  int NumberOfCylinders;
  bool BoolTest2;
  char c1;
  unsigned char uc1;
  short s1;
  unsigned short us1;
  int i1;
  unsigned int ui1;
  long l1;
  unsigned long ul1;
  float f1;
  double d1;
  VehicleInt64 i64;
  VehicleUInt64 ui64;
  ManufacturerID enumMfgID;
  int AMarker;
};

#endif
