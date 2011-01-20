#include "Mustang.h"

Mustang *Mustang::New()
{
  return new Mustang;
}

Mustang::Mustang()
{
}

Mustang::~Mustang()
{
}

double Mustang::VGetMileage() const
{
  return 6.28;
}

//int Mustang::TestDefaultArgsWarning1(int arg1, int arg2)
//{
//  return 0;
//}

//int Mustang::TestDefaultArgsWarning2(int arg1, int arg2, int arg3)
//{
//  return 0;
//}

int Mustang::main(int, char* [])
{
  return 0;
}

int Mustang::VirtualMainPlus(double, int, char* [], long)
{
  return 0;
}

int Mustang::NonVirtualMain(int, char* [])
{
  return 0;
}

int Mustang::StillWontBeWrapped(int, char* [], char* [])
{
  return 0;
}
