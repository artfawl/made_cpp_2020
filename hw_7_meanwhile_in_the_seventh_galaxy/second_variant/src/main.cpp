#include <iostream>

#include "floor.h"

int main() {
  std::cout << "Possibilities count: " << Floor<2, 3>::value << std::endl;
  return 0;
}
