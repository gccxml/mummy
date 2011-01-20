#include "Oops.h"

Oops::Oops(const char* description)
{
  if (description)
  {
    this->m_Description = description;
  }
}

Oops::~Oops()
{
}

Oops* Oops::Clone()
{
  return this->CloneInternal();
}

void Oops::Delete()
{
  delete this;
}

const char* Oops::Description()
{
  return this->m_Description.c_str();
}

Oops* Oops::CloneInternal()
{
  Oops* oops = new Oops(this->m_Description.c_str());
  return oops;
}
