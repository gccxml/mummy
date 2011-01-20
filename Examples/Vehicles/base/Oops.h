#ifndef _Oops_h
#define _Oops_h

#include "VehiclesConfig.h"
#include <string>

// Yeah-yeah-yeah-...
//
#if defined(_MSC_VER)
#pragma warning(disable:4251) /* class needs to have dll-interface to be used by clients */
#endif

// This is the sample exception class...

class base_dll Oops
{
public:
  // Constructor/destructor:
  Oops(const char* description);
  virtual ~Oops();

  // Exception classes need a clone method:
  Oops* Clone();

  // Exception classes need a disposal method:
  void Delete();

  // Exception classes need some wrappable way to extract information
  // out of them:
  //
  const char* Description();

protected:
  virtual Oops* CloneInternal();

private:
  std::string m_Description;

  // unimplemented : can only be instantiated by the public ctor or Clone
  Oops();
};

#endif
