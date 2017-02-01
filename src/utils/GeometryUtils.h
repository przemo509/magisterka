#ifndef MAGISTERKA_GEOMETRYUTILS_H
#define MAGISTERKA_GEOMETRYUTILS_H

#include <cmath>

class Point {
public:
    float x, y, z;

    Point(float xx, float yy, float zz) :
            x(xx), y(yy), z(zz) {
    }

    Point() :
            x(0.0f), y(0.0f), z(0.0f) {
    }

    float distance(const Point &p) {
        float dx = x - p.x;
        float dy = y - p.y;
        float dz = z - p.z;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }
};

class Vector {
public:
    float x, y, z;

    Vector() :
            x(0.0), y(0.0), z(0.0) {
    }

    Vector(float xx, float yy, float zz) :
            x(xx), y(yy), z(zz) {
    }

    Vector(const Point &a, const Point &b) {
        x = b.x - a.x;
        y = b.y - a.y;
        z = b.z - a.z;
    }

    float length() {
        return sqrtf(x * x + y * y + z * z);
    }

    void normalize() {
        float len = length();
        if (len == 0.0) {
            return;
        }
        x /= len;
        y /= len;
        z /= len;
    }

    Vector flip() {
        return Vector(-x, -y, -z);
    }

    Vector crossProduct(const Vector &v) {
        return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    Point translate(const Point &p) const {
        return translate(p, 1.0f);
    }

    const Point translate(const Point &p, float factor) const {
        return Point(factor * x + p.x, factor * y + p.y, factor * z + p.z);
    }

    float dotProduct(const Vector &v) {
        float result = x * v.x + y * v.y + z * v.z;
        return result;
    }
};

class Plane {
public:
    float a, b, c, d; // Ax + By + Cz + D = 0
    Point p;

    Plane(const Vector &normal, Point pointOnThePlane) {
        a = normal.x;
        b = normal.y;
        c = normal.z;
        d = -a * p.x - b * p.y - c * p.z;
        p = pointOnThePlane;
    }

    Point intersection(const Point &start, const Vector &dir) {
        float f = dir.x * a + dir.y * b +
                  dir.z * c; // to byłoby zerem gdyby dir był równoległy do płaszczyzny, ale u nas nie będzie
        float t = (p.x * a + p.y * b + p.z * c - a * start.x - b * start.y - c * start.z) / f;
        return Point(start.x + t * dir.x, start.y + t * dir.y, start.z + t * dir.z);
    }
};

#endif //MAGISTERKA_GEOMETRYUTILS_H
