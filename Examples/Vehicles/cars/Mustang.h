#ifndef _Mustang_h
#define _Mustang_h

#include "AbstractCar.h"

// Mustang is a "yet to be documented" example class...

class cars_dll Mustang : public AbstractCar
{
public:
  static Mustang *New();

//  int TestDefaultArgsWarning1(int arg1, int arg2 = 2);
//  int TestDefaultArgsWarning2(int arg1, int arg2 = 2, int arg3 = 3);

  static int main(int argc, char* argv[]);
  virtual int VirtualMainPlus(double threshold, int argc, char* argv[], long flag);
  int NonVirtualMain(int argc, char* argv[]);
  int StillWontBeWrapped(int argc, char* argv[], char* argz[]);

protected:
  Mustang();
  virtual ~Mustang();

  virtual double VGetMileage() const;
};

#endif
