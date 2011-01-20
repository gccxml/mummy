#ifndef _SignatureSeries_h
#define _SignatureSeries_h

#include "VehiclesConfig.h"

// SignatureSeries is a "yet to be documented" example class...

#define SS_DECLARE_METHODS(type, type_name) \
public: \
  void Method_##type_name##_param_plain(type arg); \
  void Method_##type_name##_param_const(const type arg); \
  void Method_##type_name##_param_ref(type& arg); \
  void Method_##type_name##_param_constref(const type& arg); \
  void Method_##type_name##_param_datapointer(type* arg); \
  void Method_##type_name##_param_constdatapointer(const type* arg); \
  void Method_##type_name##_param_dataconstpointer(type* const arg); \
  void Method_##type_name##_param_constdataconstpointer(const type* const arg); \
  type Method_##type_name##_return_plain(); \
  const type Method_##type_name##_return_const(); \
  type& Method_##type_name##_return_ref(); \
  const type& Method_##type_name##_return_constref(); \
  type* Method_##type_name##_return_datapointer(); \
  const type* Method_##type_name##_return_constdatapointer(); \
  type* const Method_##type_name##_return_dataconstpointer(); \
  const type* const Method_##type_name##_return_constdataconstpointer(); \
private: \
  type m_##type_name; \
public:

class base_dll SignatureSeries
{
public:
  SignatureSeries();
  virtual ~SignatureSeries();

  // Try each of "plain" "ref" "pointer" "const ref" and "const pointer"
  // as parameter type and as return type for each fundamental data type:
  //
  SS_DECLARE_METHODS(int, int);
  //SS_DECLARE_METHODS(unsigned int, unsigned_int);

  void ThrowExceptionOnPurpose(const char *text);
};

#endif
