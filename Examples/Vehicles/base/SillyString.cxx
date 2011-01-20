#include "SillyString.h"

#if 0

SillyString::SillyString()
  : stl_string()
  {
  }

SillyString::SillyString(const value_type* s)
  : stl_string(s)
  {
  }

SillyString::SillyString(const value_type* s, size_type n)
  : stl_string(s, n)
  {
  }

SillyString::SillyString(const stl_string& s, size_type pos, size_type n)
  : stl_string(s, pos, n)
  {
  }

#endif
