#include "geometry.h"
#include <iostream>
#include <cmath>

using std::cin;
using std::cout;

int main() {
    const vector<int> rOfL = { 1, 3, 4 };
    Point a(0, 0), b(1, 0), c(0.5, 0.5), d(0.5, -0.5);
    Polygon zuko({ b, d, a, c, });
    Rectangle f(a, b, 1);
    cout << (f == zuko);
    return 0;
}