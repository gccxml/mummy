#include "SignatureSeries.h"
#include "Oops.h"
#include <iostream>

#define SS_IMPLEMENT_METHODS(type, type_name) \
  void SignatureSeries::Method_##type_name##_param_plain(type arg) \
    { \
    this->m_##type_name = arg; \
    std::cerr << arg << std::endl; \
    } \
  \
  void SignatureSeries::Method_##type_name##_param_const(const type arg) \
    { \
    this->m_##type_name = arg; \
    std::cerr << arg << std::endl; \
    } \
  \
  void SignatureSeries::Method_##type_name##_param_ref(type& arg) \
    { \
    this->m_##type_name = arg; \
    std::cerr << arg << " (via reference " << &arg << ")" << std::endl; \
    arg = (type) 0; \
    } \
  \
  void SignatureSeries::Method_##type_name##_param_constref(const type& arg) \
    { \
    this->m_##type_name = arg; \
    std::cerr << arg << " (via reference " << &arg << ")" << std::endl; \
    } \
  \
  void SignatureSeries::Method_##type_name##_param_datapointer(type* arg) \
    { \
    this->m_##type_name =  (type)(arg ? *arg : 999); \
    std::cerr << this->m_##type_name << " (via pointer " << arg << ")" << std::endl; \
    } \
  \
  void SignatureSeries::Method_##type_name##_param_constdatapointer(const type* arg) \
    { \
    this->m_##type_name = (type)(arg ? *arg : 888); \
    std::cerr << this->m_##type_name << " (via pointer " << arg << ")" << std::endl; \
    } \
  \
  void SignatureSeries::Method_##type_name##_param_dataconstpointer(type* const arg) \
    { \
    this->m_##type_name = (type)(arg ? *arg : 777); \
    std::cerr << this->m_##type_name << " (via pointer " << arg << ")" << std::endl; \
    } \
  \
  void SignatureSeries::Method_##type_name##_param_constdataconstpointer(const type* const arg) \
    { \
    this->m_##type_name = (type)(arg ? *arg : 666); \
    std::cerr << this->m_##type_name << " (via pointer " << arg << ")" << std::endl; \
    } \
  \
  type SignatureSeries::Method_##type_name##_return_plain() \
    { \
    return this->m_##type_name; \
    } \
  \
  const type SignatureSeries::Method_##type_name##_return_const() \
    { \
    return this->m_##type_name; \
    } \
  \
  type& SignatureSeries::Method_##type_name##_return_ref() \
    { \
    return this->m_##type_name; \
    } \
  \
  const type& SignatureSeries::Method_##type_name##_return_constref() \
    { \
    return this->m_##type_name; \
    } \
  \
  type* SignatureSeries::Method_##type_name##_return_datapointer() \
    { \
    return &this->m_##type_name; \
    } \
  \
  const type* SignatureSeries::Method_##type_name##_return_constdatapointer() \
    { \
    return &this->m_##type_name; \
    } \
  \
  type* const SignatureSeries::Method_##type_name##_return_dataconstpointer() \
    { \
    return &this->m_##type_name; \
    } \
  \
  const type* const SignatureSeries::Method_##type_name##_return_constdataconstpointer() \
    { \
    return &this->m_##type_name; \
    }


SignatureSeries::SignatureSeries()
{
  this->m_int = 1;
}

SignatureSeries::~SignatureSeries()
{
}

SS_IMPLEMENT_METHODS(int, int);
//SS_IMPLEMENT_METHODS(unsigned int, unsigned_int);

void SignatureSeries::ThrowExceptionOnPurpose(const char *text)
{
  throw Oops(text);
}
