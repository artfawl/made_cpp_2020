#include <functional>
#include <iostream>

using std::cout;

using std::function;

template <class First>
auto compose(function<First> func) {
  return [&](auto... args) -> decltype(auto) { return func(args...); };
}

template <class First, class... Other>
auto compose(function<First> func, function<Other>... functions) {
  return [&](auto... args) -> decltype(auto) {
    auto temp = compose(functions...);
    return func(temp(args...));
  };
}