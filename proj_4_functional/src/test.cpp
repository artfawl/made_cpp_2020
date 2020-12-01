#include <functional>
#include <iostream>

using std::cout;

template <class First>
void f(First a) {
  cout << a << "\n";
}

template <class First, class... Other>
void f(First a, Other... args) {
  cout << a << "\n";
  f(args...);
}

int main() {
  f(1, 2, 3, 4);
  return 0;
}