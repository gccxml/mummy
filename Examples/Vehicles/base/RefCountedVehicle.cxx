#include "RefCountedVehicle.h"
#include <iostream>

RefCountedVehicle* RefCountedVehicle::Create()
{
  RefCountedVehicle* rcv = new RefCountedVehicle;
  rcv->IncrRefCount(0);
  return rcv;
}

void RefCountedVehicle::Destroy()
{
  this->DecrRefCount(0);
}

void RefCountedVehicle::IncrRefCount(void*)
{
  ++m_RefCount;
}

void RefCountedVehicle::DecrRefCount(void*)
{
  --m_RefCount;
  if (0 == m_RefCount)
  {
    delete this;
  }
}

unsigned int RefCountedVehicle::GetRefCount() const
{
  return m_RefCount;
}

const RefCountedVehicle* RefCountedVehicle::GetThisVehicle() const
{
  ++m_CallCounter;

  // Alternate between returning valid pointers and NULL pointers
  // for complete branch coverage in the export layer... (Call multiple
  // times sequentially from the wrapped side for complete coverage...)
  //
  if (0 != m_CallCounter%2)
    {
    return 0;
    }

  return this;
}

RefCountedVehicle* RefCountedVehicle::GetThatVehicle() const
{
  ++m_CallCounter;

  // Alternate between returning valid pointers and NULL pointers
  // for complete branch coverage in the export layer... (Call multiple
  // times sequentially from the wrapped side for complete coverage...)
  //
  if (0 != m_CallCounter%2)
    {
    return 0;
    }

  return RefCountedVehicle::Create();
}

RefCountedVehicle* RefCountedVehicle::GetTheOtherVehicle() const
{
  ++m_CallCounter;

  // Alternate between returning valid pointers and NULL pointers
  // for complete branch coverage in the export layer... (Call multiple
  // times sequentially from the wrapped side for complete coverage...)
  //
  if (0 != m_CallCounter%2)
    {
    return 0;
    }

  if (0 == m_OtherVehicle)
  {
    m_OtherVehicle = RefCountedVehicle::Create();
  }

  return m_OtherVehicle;
}

//----------------------------------------------------------------------------
// These methods are non-iwhCounted-hint methods that are aliases for the
// iwhCounted-hint method GetThatVehicle... We achieve the same effect
// as having the hint without modifying the C++ header or source by
// adding the method names to the MummySettings.xml countedMethodsRegex
// attribute...
RefCountedVehicle* RefCountedVehicle::GetThatVehicleAlias() const
{
  return this->GetThatVehicle();
}

RefCountedVehicle* RefCountedVehicle::GetThatVehicleByAnyOtherName() const
{
  return this->GetThatVehicle();
}

//----------------------------------------------------------------------------
//RefCountedVehicle* RefCountedVehicle::GetNoHint() const
//{
//  return 0;
//}

//void RefCountedVehicle::SetThis()
//{
//}

int RefCountedVehicle::NumberOfConstructorCalls = 0;
int RefCountedVehicle::NumberOfDestructorCalls = 0;

RefCountedVehicle::RefCountedVehicle()
{
  ++NumberOfConstructorCalls;

  m_RefCount = 0;
  m_OtherVehicle = 0;
  m_CallCounter = 0;
}

RefCountedVehicle::~RefCountedVehicle()
{
  ++NumberOfDestructorCalls;

  if (m_OtherVehicle)
  {
    m_OtherVehicle->Destroy();
  }
}

void RefCountedVehicle::Check()
{
  if (NumberOfConstructorCalls != NumberOfDestructorCalls)
    {
    std::cerr << "error: RefCountedVehicle::Check ctor/dtor mismatch - mem leak?" << std::endl;
    }
}

// Intentional no-op.
// Exists only to test BTX/ETX style exclusion.
//
void RefCountedVehicle::MethodExcludedByBTXETX()
{
}
