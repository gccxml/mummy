#include <iostream>

class simple;

#ifdef _WIN32
#define SIMPLE_DLL_IMPORT __declspec(dllimport)
#else
#define SIMPLE_DLL_IMPORT
#endif

extern "C" SIMPLE_DLL_IMPORT
simple* simple_new(unsigned int* mteStatus, unsigned int* mteIndex, unsigned int* rawRefCount);

extern "C" SIMPLE_DLL_IMPORT
void simple_delete(simple* pThis);

int main(int, char* [])
{
  unsigned int mteStatus = 0;
  unsigned int mteIndex = 0;
  unsigned int rawRefCount = 0;

  simple* instance = simple_new(&mteStatus, &mteIndex, &rawRefCount);

  std::cout << "instance: " << instance << std::endl;
  std::cout << "mteStatus: " << mteStatus << std::endl;
  std::cout << "mteIndex: " << mteIndex << std::endl;
  std::cout << "rawRefCount: " << rawRefCount << std::endl;
  std::cout << "(int) rawRefCount: " << (int) rawRefCount << std::endl;

  simple_delete(instance);
  instance = 0;

  return 0;
}
