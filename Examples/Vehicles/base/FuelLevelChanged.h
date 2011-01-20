#ifndef _FUELLEVELCHANGED_H
#define _FUELLEVELCHANGED_H

#include "AbstractVehicleEvent.h" // parent class
#include "FuelLevelChangedEventArgs.h" // args class


// This is for gccxml attributes testing/development,
// it's not just random madness:
//
gxa(a1) typedef gxa(a2) long gxa(a3) MyNonClassLong gxa(a4);


class iwhEvent FuelLevelChanged : public AbstractVehicleEvent<FuelLevelChangedEventArgs*>
{
public:
  // More gccxml attributes testing/development:
  //
  gxa(b1) typedef gxa(b2) long gxa(b3) MyLong gxa(b4);

  gxa(l1) MyLong ReturnMyLong()
  {
    return (MyLong) 10;
  }

  MyNonClassLong ReturnMyNonClassLong()
  {
    return (MyNonClassLong) 11;
  }

  // The real stuff of this class:
  //
  typedef void (SDE_CC *Handler)(AbstractVehicle* sender, FuelLevelChangedEventArgs* data);

  unsigned int AddHandler(Handler of)
  {
    return (unsigned int) attach(of);
  }

  void RemoveHandler(unsigned int ofid)
  {
    detach((EventHandlerId) ofid);
  }

  static FuelLevelChanged* New()
  {
    return new FuelLevelChanged;
  }

  virtual void Delete()
  {
    delete this;
  }

protected:
  FuelLevelChanged()
  {
  }

  virtual ~FuelLevelChanged()
  {
  }
};

#endif // _FUELLEVELCHANGED_H
