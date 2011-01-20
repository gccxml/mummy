#ifndef _BlatMobile_h
#define _BlatMobile_h

#include "VehiclesConfig.h"

// BlatMobile is a "yet to be documented" example class...
// This class is used to test/demonstrate the "shadow class"
// feature of mummy...

class base_dll BlatMobile
{
public:
  static void CycleTurbo(BlatMobile* bm);

  static BlatMobile* Create();
  virtual void Destroy();
  void IncrRefCount();
  void DecrRefCount();
  iwhPropGet unsigned int GetRefCount() const;

  virtual void ActivateTurboBoost();
  virtual void DeactivateTurboBoost();

  // Use all supported data types to get full coverage of the
  // shadow layer generator:
  //
  virtual void VUnsignedChar(unsigned char arg);
  virtual void VUnsignedShort(unsigned short arg);
  virtual void VUnsignedInt(unsigned int arg);
  virtual void VUnsignedLong(unsigned long arg);
  virtual void VUnsignedInt64(VehicleUInt64 arg);
  virtual void VChar(char arg);
  virtual void VShort(short arg);
  virtual void VInt(int arg);
  virtual void VLong(long arg);
  virtual void VInt64(VehicleInt64 arg);
  virtual void VBool(bool arg);
  virtual void VFloat(float arg);
  virtual void VDouble(double arg);
  virtual void VVoidPointer(void* arg);
  virtual void VObjectPointer(BlatMobile* arg);

protected:
  BlatMobile();
  virtual ~BlatMobile();

private:
  unsigned int m_RefCount;
};

#endif
