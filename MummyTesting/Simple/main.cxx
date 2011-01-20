#include "simple.h"
#include <iostream>

int main(int, char* [])
{
  simple instance;

  std::cout << "&instance: " << &instance << std::endl;

  return 0;
}
