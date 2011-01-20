#ifndef _SillyString_h
#define _SillyString_h

#if 0

#include "VehiclesConfig.h"
#include <string>

// http://en.wikipedia.org/wiki/Silly_string
//
// SillyString is a "yet to be documented" example class...
// This class is used to test/demonstrate the "map to native
// C# string" feature of mummy... (via the two iwh* hints
// used here):

// Disable selected MSVC compiler warning messages
#if defined(_MSC_VER)
#if defined(BUILD_SHARED_LIBS)
#pragma warning(disable:4251) /* class needs to have dll-interface to be used by clients */
//#pragma message(" info: disabled warning 4251...")
#pragma warning(disable:4275) /* non dll-interface class ... used as base for dll-interface class */
#endif
#endif

class iwhMapToType(string) iwhStringMethod(c_str) base_dll SillyString : public std::string
{
public:
  // The original string type
  typedef std::string stl_string;

public:
  // String member types
  typedef stl_string::value_type             value_type;
  typedef stl_string::pointer                pointer;
  typedef stl_string::reference              reference;
  typedef stl_string::const_reference        const_reference;
  typedef stl_string::size_type              size_type;
  typedef stl_string::difference_type        difference_type;
  typedef stl_string::iterator               iterator;
  typedef stl_string::const_iterator         const_iterator;
  typedef stl_string::reverse_iterator       reverse_iterator;
  typedef stl_string::const_reverse_iterator const_reverse_iterator;

  // SillyString constructors
  SillyString();
  SillyString(const value_type* s);
  SillyString(const value_type* s, size_type n);
  SillyString(const stl_string& s, size_type pos = 0, size_type n = npos);
};

#endif

#endif
