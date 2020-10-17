#pragma once

#include <cmath>
#include <iostream>
#include <vector>

using std::vector;

const double EPS = 1e-6;

class WrongData : public std::exception {};

struct Point {
    double x;
    double y;

    Point() : x(0), y(0) {}
    Point(double x_, double y_) : x(x_), y(y_) {}
    Point(const Point& other) {
        x = other.x;
        y = other.y;
    }

    bool operator==(const Point& other) const {
        if (std::abs(x - other.x) < EPS && std::abs(y - other.y) < EPS)
            return true;
        return false;
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }

};

double dist(Point one, Point two) {
    return sqrt(pow((one.x - two.x), 2) + pow((one.y - two.y), 2));
}

class Line {
    Point start_point;
    Point direct;

public:
    Line(Point one, Point two) : start_point(one) {
        if (one == two)
            throw WrongData();
        direct.x = two.x - one.x;
        direct.y = two.y - one.y;
    }

    Line(const Line& other) = default;

    bool operator==(const Line& other) const {
        if (direct.x == other.direct.x && direct.y == other.direct.y) {
            Point vect(start_point.y - other.start_point.y, other.start_point.x - start_point.x);
            if (vect.x * direct.x + vect.y * direct.y < EPS)
                return true;
        }
        return false;
    }

    bool operator!=(const Line& other) const {
        return !(*this == other);
    }

    friend class Polygon;
};

class Shape {
public:
    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual bool operator==(const Shape& another) const = 0;
    virtual void rotate(Point center, double angle) = 0;
    virtual void reflex(Point center) = 0;
    virtual void reflex(Line axis) = 0;
    virtual void scale(Point center, double coefficient) = 0;
};

class Polygon : public Shape {
    vector<Point> nodes;

public:
    Polygon(vector<Point> points) {
        if (points.size() < 1)
            throw WrongData();
        for (size_t i = 0; i < points.size(); ++i)
            nodes.push_back(points[i]);
    }

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
        ans += (nodes[nodes.size() - 1].x + nodes[0].x) * (nodes[nodes.size() - 1].y - nodes[0].y);
        return std::abs(ans) / 2;

    }

    bool operator==(const Polygon& another) const {
        if (nodes.size() != another.nodes.size()) {
            return false;
        }
        else {
            size_t size = nodes.size();
            for (size_t i = 0; i < size; ++i) {
                if (nodes[i] == another.nodes[0]) {
                    bool res = true;
                    for (size_t j = 1; j < size; ++j)
                        res = res && (nodes[(i + j) % size] == another.nodes[j]);
                    if (res)
                        return res;
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
        if (formed)
            return *this == *formed;
        return false;
    }

    void rotate(Point center, double angle) override {
        Point temp;
        for (size_t i = 0; i < nodes.size(); ++i) {
            nodes[i].x -= center.x;
            nodes[i].y -= center.y;
            temp.x = nodes[i].x * cos(angle) - nodes[i].y * sin(angle);
            temp.y = nodes[i].x * sin(angle) + nodes[i].y * cos(angle);
            nodes[i].x = temp.x + center.x;
            nodes[i].y = temp.y + center.y;
        }
        //std::cout << nodes[0].x << " " << nodes[0].y;
    }

    void reflex(Point center) override {
        for (size_t i = 0; i < nodes.size(); ++i) {
            nodes[i].x = 2 * center.x - nodes[i].x;
            nodes[i].y = 2 * center.y - nodes[i].y;
        }
    }

    void reflex(Line axis) override {
        Point vect;
        double coef;
        for (size_t i = 0; i < nodes.size(); ++i) {
            vect.x = nodes[i].x - axis.start_point.x;
            vect.y = nodes[i].y - axis.start_point.y;
            coef = (vect.x * axis.direct.x + vect.y * axis.direct.y) / pow(dist(axis.direct, Point(0, 0)), 2);
            nodes[i].x += 2 * (coef * axis.direct.x - vect.x);
            nodes[i].y += 2 * (coef * axis.direct.y - vect.y);
        }
    }

    void scale(Point center, double coefficient) override {
        for (size_t i = 0; i < nodes.size(); ++i) {
            nodes[i].x = center.x + coefficient * (nodes[i].x - center.x);
            nodes[i].y = center.y + coefficient * (nodes[i].y - center.y);
        }
    }

    int verticesCount() const {
        return nodes.size();
    }

    const std::vector<Point> getVertices() const {
        return nodes;
    }
};

class Rectangle : public Polygon {
private:
    Point give_point3(Point first, Point second, double coef) {
        double tang = coef >= 1 ? coef : (1 / coef);
        Point vect1(second.x - first.x, second.y - first.y);
        vect1.x /= (1 + tang * tang);
        vect1.y /= (1 + tang * tang);
        Point vect2(-vect1.y * tang, vect1.x * tang);
        Point third(first.x + vect1.x + vect2.x, first.y + vect1.y + vect2.y);
        return third;
    }

    Point give_point4(Point first, Point second, double coef) {
        Point third = give_point3(first, second, coef);
        Point fourth(first.x + second.x - third.x, first.y + second.y - third.y);
        return fourth;
    }

public:
    Rectangle(Point first, Point second, double coef) :
        Polygon({ first, give_point3(first, second, coef), second, give_point4(first, second, coef) }) {}

};
