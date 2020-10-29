#pragma once

#include <cmath>
#include <iostream>
#include <vector>

using std::vector;

const double EPS = 1e-6;
const double PI = acos(-1);

class WrongData : public std::exception {};

class Shape;

struct Point {
  double x;
  double y;

  Point() : x(0), y(0) {}

  Point(double x_, double y_) : x(x_), y(y_) {}

  Point(const Point& other) = default;

  Point& operator=(const Point& other) = default;

  bool operator==(const Point& other) const {
    if (std::abs(x - other.x) < EPS && std::abs(y - other.y) < EPS) return true;
    return false;
  }

  bool operator!=(const Point& other) const { return !(*this == other); }

  ~Point() = default;
};

double dist(Point one, Point two) {
  return sqrt(pow((one.x - two.x), 2) + pow((one.y - two.y), 2));
}

class Line {
  friend class Point;
  friend class Shape;
  friend Point intersect(Line one, Line two);
  Point start_point;
  Point direct;

 public:
  Line(Point one, Point two) : start_point(one) {
    if (one == two) throw WrongData();
    direct.x = two.x - one.x;
    direct.y = two.y - one.y;
  }

  Line(double angle_coef, double shift)
      : start_point(0, shift), direct(1, angle_coef) {}

  Line(Point start, double angle_coef)
      : start_point(start), direct(1, angle_coef) {}

  Line(const Line& other) = default;

  Line& operator=(const Line& other) = default;

  bool operator==(const Line& other) const {
    double temp = (direct.x * other.direct.x + direct.y * other.direct.y);
    temp /= dist(direct, Point(0, 0));
    temp /= dist(other.direct, Point(0, 0));
    temp = std::abs(temp);
    if (std::abs(temp - 1) < EPS) {  // проверка на сонаправленность
      Point vect(start_point.y - other.start_point.y,
                 other.start_point.x - start_point.x);
      // vect - перпендикулярен вектору, соединяющему две начальные точки
      if (std::abs(vect.x * direct.x + vect.y * direct.y < EPS)) return true;
    }
    return false;
  }

  bool operator!=(const Line& other) const { return !(*this == other); }

  ~Line() = default;
};

Point intersect(Line one, Line two) {
  if (one.direct == two.direct || one == two)
    throw WrongData();
  else {
    // (x,y) = (x0,y0) + (dir_x, dir_y) * t - способ задания прямой, t -
    // параметр t2_coord - значение параметра для 2й прямой, при котором она
    // пересекает первую (выведено на листочке)
    double t2_coord = (two.start_point.y - one.start_point.y) * one.direct.x;
    t2_coord -= (two.start_point.x - one.start_point.x) * one.direct.y;
    t2_coord /= (two.direct.x * one.direct.y - two.direct.y * one.direct.x);
    Point result(two.start_point.x + t2_coord * two.direct.x,
                 two.start_point.y + t2_coord * two.direct.y);
    return result;
  }
}

class Shape {
 protected:
  void rotate_point(Point& change, Point center, double angle) {
    Point temp;
    change.x -= center.x;
    change.y -= center.y;
    temp.x = change.x * cos(angle) - change.y * sin(angle);
    temp.y = change.x * sin(angle) + change.y * cos(angle);
    change.x = temp.x + center.x;
    change.y = temp.y + center.y;
  }

  void reflex_point(Point& change, Point center) {
    change.x = 2 * center.x - change.x;
    change.y = 2 * center.y - change.y;
  }

  void reflex_point(Point& change, Line axis) {
    Point vect;
    double coef;
    vect.x = change.x - axis.start_point.x;
    vect.y = change.y - axis.start_point.y;
    coef = (vect.x * axis.direct.x + vect.y * axis.direct.y) /
           pow(dist(axis.direct, Point(0, 0)), 2);
    change.x += 2 * (coef * axis.direct.x - vect.x);
    change.y += 2 * (coef * axis.direct.y - vect.y);
  }

  void scale_point(Point& change, Point center, double coefficient) {
    change.x = center.x + coefficient * (change.x - center.x);
    change.y = center.y + coefficient * (change.y - center.y);
  }

 public:
  virtual double perimeter() const = 0;
  virtual double area() const = 0;
  virtual bool operator==(const Shape& another) const = 0;
  virtual bool operator!=(const Shape& another) const = 0;
  virtual void rotate(Point center, double angle) = 0;
  virtual void reflex(Point center) = 0;
  virtual void reflex(Line axis) = 0;
  virtual void scale(Point center, double coefficient) = 0;
};

class Ellipse : public Shape {
 protected:
  vector<Point> focus;
  double total;
  double a, b, c;  // стандартные обозначения, поэтому 1-буквенные (мне кажется
                   // так понятнее)

 public:
  Ellipse(Point focus1, Point focus2, double r)
      : focus({focus1, focus2}), total(r) {
    c = dist(focus[0], focus[1]) / 2;
    if (total < 2 * c) throw WrongData();
    a = total / 2;
    b = sqrt(a * a - c * c);
  }

  Ellipse(const Ellipse& other) = default;

  Ellipse& operator=(const Ellipse& other) = default;

  double perimeter() const override {
    double temp = (a - b) / (a + b);
    temp = 1 + 3 * temp * temp / (10 + sqrt(4 - 3 * temp * temp));
    return PI * (a + b) * temp;
  }

  double area() const override { return PI * a * b; }

  bool operator==(const Ellipse& another) const {
    if (total == another.total &&
        (focus[0] == another.focus[0] && focus[1] == another.focus[1] ||
         focus[0] == another.focus[1] && focus[1] == another.focus[0])) {
      return true;
    }
    return false;
  }

  bool operator!=(const Shape& another) const { return !(*this == another); }

  bool operator==(const Shape& another) const override {
    const Ellipse* formed = dynamic_cast<const Ellipse*>(&another);
    if (formed) return *this == *formed;
    return false;
  }

  void rotate(Point center, double angle) override {
    for (size_t i = 0; i < focus.size(); ++i)
      rotate_point(focus[i], center, angle);
  }

  void reflex(Point center) override {
    for (size_t i = 0; i < focus.size(); ++i) reflex_point(focus[i], center);
  }

  void reflex(Line axis) override {
    for (size_t i = 0; i < focus.size(); ++i) reflex_point(focus[i], axis);
  }

  void scale(Point center, double coefficient) override {
    for (size_t i = 0; i < focus.size(); ++i)
      scale_point(focus[i], center, coefficient);
    a *= std::abs(coefficient);
    b *= std::abs(coefficient);
    c *= std::abs(coefficient);
    total *= std::abs(coefficient);
  }

  std::pair<Point, Point> focuses() const {
    return std::make_pair(focus[0], focus[1]);
  }

  double eccentricity() const { return (c / a); }

  ~Ellipse() = default;
};

class Circle : public Ellipse {
 public:
  Circle(Point center, double radius) : Ellipse(center, center, 2 * radius) {}

  Circle(const Circle& other) = default;

  Circle& operator=(const Circle& other) = default;

  Point center() { return focus[0]; }

  double radius() const { return a; }

  ~Circle() = default;
};

class Polygon : public Shape {
 protected:
  vector<Point> nodes;

 public:
  Polygon(vector<Point> points) {
    if (points.size() < 1) throw WrongData();
    for (size_t i = 0; i < points.size(); ++i) nodes.push_back(points[i]);
  }

  Polygon(const Polygon& other) = default;

  Polygon& operator=(const Polygon& other) = default;

  double perimeter() const override {
    double ans = 0;
    for (size_t i = 0; i < nodes.size() - 1; ++i)
      ans += dist(nodes[i], nodes[i + 1]);
    ans += dist(nodes[0], nodes[nodes.size() - 1]);
    return ans;
  }

  double area() const override {
    double ans = 0;
    for (size_t i = 0; i < nodes.size() - 1; ++i)
      ans += (nodes[i].x + nodes[i + 1].x) * (nodes[i].y - nodes[i + 1].y);
    ans += (nodes[nodes.size() - 1].x + nodes[0].x) *
           (nodes[nodes.size() - 1].y - nodes[0].y);
    return std::abs(ans) / 2;
  }

  bool operator==(const Polygon& another) const {
    if (nodes.size() != another.nodes.size()) {
      return false;
    } else {
      size_t size = nodes.size();
      for (size_t i = 0; i < size; ++i) {
        if (nodes[i] == another.nodes[0]) {  // находим первый равный элемент и
                                             // сравниваем "наложением"
          bool res = true;
          for (size_t j = 1; j < size; ++j)
            res = res && (nodes[(i + j) % size] == another.nodes[j]);
          if (res) return res;
          res = true;
          for (size_t j = size - 1; j > 0; --j)
            res = res && (nodes[(i - j + size) % size] == another.nodes[j]);
          return res;
        }
      }
      return false;
    }
  }

  bool operator==(const Shape& another) const override {
    const Polygon* formed = dynamic_cast<const Polygon*>(&another);
    if (formed) return *this == *formed;
    return false;
  }

  bool operator!=(const Shape& another) const { return !(*this == another); }

  void rotate(Point center, double angle) override {
    for (size_t i = 0; i < nodes.size(); ++i) {
      rotate_point(nodes[i], center, angle);
    }
  }

  void reflex(Point center) override {
    for (size_t i = 0; i < nodes.size(); ++i) {
      reflex_point(nodes[i], center);
    }
  }

  void reflex(Line axis) override {
    for (size_t i = 0; i < nodes.size(); ++i) {
      reflex_point(nodes[i], axis);
    }
  }

  void scale(Point center, double coefficient) override {
    for (size_t i = 0; i < nodes.size(); ++i) {
      scale_point(nodes[i], center, coefficient);
    }
  }

  int verticesCount() const { return nodes.size(); }

  const std::vector<Point> getVertices() const { return nodes; }

  ~Polygon() = default;
};

class Rectangle : public Polygon {
 public:
  Rectangle(Point first, Point third, double coef)
      : Polygon({first, {0, 0}, third, {0, 0}}) {
    double tang = coef >= 1 ? coef : (1 / coef);
    Point vect1(third.x - first.x, third.y - first.y);
    vect1.x /= (1 + tang * tang);
    vect1.y /= (1 + tang * tang);
    Point vect2(-vect1.y * tang, vect1.x * tang);
    Point second(first.x + vect1.x + vect2.x, first.y + vect1.y + vect2.y);
    Point fourth(first.x + third.x - second.x, first.y + third.y - second.y);
    nodes[1] = second;
    nodes[3] = fourth;
  }

  Rectangle(const Rectangle& other) = default;

  Rectangle& operator=(const Rectangle& other) = default;

  Point center() const {
    Point cent((nodes[0].x + nodes[2].x) / 2, (nodes[0].y + nodes[2].y) / 2);
    return cent;
  }

  std::pair<Line, Line> diagonals() const {
    Line first(nodes[0], nodes[2]);
    Line second(nodes[1], nodes[3]);
    return std::make_pair(first, second);
  }

  ~Rectangle() = default;
};

class Square : public Rectangle {
 public:
  Square(Point first, Point third) : Rectangle(first, third, 1) {}

  Square(const Square& other) = default;

  Square& operator=(const Square& other) = default;

  Circle circumscribedCircle() const {
    Point center = this->center();
    Circle result(center, dist(center, nodes[0]));
    return result;
  }

  Circle inscribedCircle() const {
    Point center = this->center();
    Circle result(center, dist(nodes[0], nodes[1]) / 2);
    return result;
  }

  ~Square() = default;
};

class Triangle : public Polygon {
  Line give_cent_perp(int first,
                      int second) const {  // серединный перпендикуляр
    Point center1((nodes[first].x + nodes[second].x) / 2,
                  (nodes[first].y + nodes[second].y) / 2);
    Point perp_vect1(-(nodes[first].y -
                       nodes[second].y),  // вектор, перпендикулярный стороне
                     nodes[first].x - nodes[second].x);
    perp_vect1.x += center1.x;
    perp_vect1.y += center1.y;
    Line cent_perp(center1, perp_vect1);
    return cent_perp;
  }

  Line give_bissec(int num) const {  // биссектриса
    int pred = (num + 2) % 3;
    int nxt = (num + 1) % 3;
    double coef = dist(nodes[pred], nodes[num]) / dist(nodes[num], nodes[nxt]);
    Point vec_ac(nodes[nxt].x - nodes[pred].x, nodes[nxt].y - nodes[pred].y);
    vec_ac.x *= (coef / (1 + coef));
    vec_ac.x += nodes[pred].x;
    vec_ac.y *=
        (coef / (1 + coef));  // координата точки пересечения биссек и стороны
    vec_ac.y += nodes[pred].y;
    Line bissec(nodes[num], vec_ac);
    return bissec;
  }

  Line give_perp(int num) const {  // перпендикуляр
    int pred = (num + 2) % 3;
    int nxt = (num + 1) % 3;
    Point vec_perp(-(nodes[nxt].y - nodes[pred].y),
                   nodes[nxt].x - nodes[pred].x);
    vec_perp.x += nodes[num].x;
    vec_perp.y += nodes[num].y;
    Line perp(nodes[num], vec_perp);
    return perp;
  }

 public:
  Triangle(Point first, Point second, Point third)
      : Polygon({first, second, third}) {
    if (Line(first, second) == Line(second, third)) throw WrongData();
  }

  Triangle(const Triangle& other) = default;

  Triangle& operator=(const Triangle& other) = default;

  Circle circumscribedCircle() const {
    Line first_cent_perp = give_cent_perp(0, 1);
    Line second_cent_perp = give_cent_perp(1, 2);
    Point circ_cent = intersect(first_cent_perp, second_cent_perp);
    double radius = dist(circ_cent, nodes[0]);
    return Circle(circ_cent, radius);
  }

  Circle inscribedCircle() const {
    Line first_bissec = give_bissec(0);
    Line second_bissec = give_bissec(1);
    Point cent = intersect(first_bissec, second_bissec);
    double radius = 2 * this->area() / this->perimeter();
    Circle result(cent, radius);
    return result;
  }

  Point centroid() const {
    double x = 0, y = 0;
    for (size_t i = 0; i < nodes.size(); ++i) {
      x += nodes[i].x;
      y += nodes[i].y;
    }
    return Point(x / 3, y / 3);
  }

  Point orthocenter() const {
    Line perp_1 = give_perp(0);
    Line perp_2 = give_perp(1);
    return intersect(perp_1, perp_2);
  }

  Line EulerLine() const { return Line(this->centroid(), this->orthocenter()); }

  Circle ninePointsCircle() {
    Circle result = this->circumscribedCircle();
    result.scale(this->orthocenter(), 0.5);
    return result;
  }

  ~Triangle() = default;
};
