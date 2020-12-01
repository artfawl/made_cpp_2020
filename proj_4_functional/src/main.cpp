#include <functional>
#include <iostream>

#include "compose.h"

using std::cout;

int main() {
  const std::function<int(int, int)> a = [](int x, int y) -> int {
    std::cout << "a" << std::endl;
    return x + y;
  };
  const std::function<int(int)> b = [](int x) -> int {
    std::cout << "b" << std::endl;
    return x * 10;
  };
  const std::function<double(double)> c = [](double x) -> double {
    std::cout << "b" << std::endl;
    return x * x;
  };

  auto kek = compose(b, c, a);
  auto ans = kek(3, 4);
  cout << ans;
  return 0;
}