#include "BlatMobile.h"
#include <iostream>

void BlatMobile::CycleTurbo(BlatMobile* bm)
{
  std::cerr << "C++ BlatMobile::CycleTurbo() called..." << std::endl;

  if (bm)
  {
    std::cerr << "Calling ActivateTurboBoost from C++ BlatMobile::CycleTurbo()..." << std::endl;
    bm->ActivateTurboBoost();

    std::cerr << "Calling DeactivateTurboBoost from C++ BlatMobile::CycleTurbo()..." << std::endl;
    bm->DeactivateTurboBoost();
  }
  else
  {
    std::cerr << "NULL BlatMobile, cannot call ActivateTurboBoost or DeactivateTurboBoost..." << std::endl;
  }
}

BlatMobile* BlatMobile::Create()
{
  BlatMobile* rcv = new BlatMobile;
  return rcv;
}

void BlatMobile::Destroy()
{
  this->DecrRefCount();
}

void BlatMobile::IncrRefCount()
{
  ++m_RefCount;
}

void BlatMobile::DecrRefCount()
{
  --m_RefCount;
  if (0 == m_RefCount)
  {
    delete this;
  }
}

unsigned int BlatMobile::GetRefCount() const
{
  return m_RefCount;
}

void BlatMobile::ActivateTurboBoost()
{
  std::cerr << "C++ BlatMobile::ActivateTurboBoost() called..." << std::endl;
}

void BlatMobile::DeactivateTurboBoost()
{
  std::cerr << "C++ BlatMobile::DeactivateTurboBoost() called..." << std::endl;
}

void BlatMobile::VUnsignedChar(unsigned char arg)
{
  unsigned char local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VUnsignedShort(unsigned short arg)
{
  unsigned short local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VUnsignedInt(unsigned int arg)
{
  unsigned int local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VUnsignedLong(unsigned long arg)
{
  unsigned long local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VUnsignedInt64(VehicleUInt64 arg)
{
  VehicleUInt64 local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VChar(char arg)
{
  char local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VShort(short arg)
{
  short local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VInt(int arg)
{
  int local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VLong(long arg)
{
  long local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VInt64(VehicleInt64 arg)
{
  VehicleInt64 local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VBool(bool arg)
{
  bool local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VFloat(float arg)
{
  float local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VDouble(double arg)
{
  double local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VVoidPointer(void* arg)
{
  void* local = arg;
  std::cerr << local << std::endl;
}

void BlatMobile::VObjectPointer(BlatMobile* arg)
{
  BlatMobile* local = arg;
  std::cerr << local << std::endl;
}

BlatMobile::BlatMobile()
{
  m_RefCount = 0;
  this->IncrRefCount();
}

BlatMobile::~BlatMobile()
{
}
