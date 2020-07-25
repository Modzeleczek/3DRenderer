#include "Vector.hpp"
#include <iostream>
#include <cmath>
#include <cassert>

Vec3f::Vec3f(float X, float Y, float Z) : x(X), y(Y), z(Z) { }
Vec3f::Vec3f() : x(0), y(0), z(0) { }

float& Vec3f::operator[](const size_t i)
{
    assert(i < 3); return i <= 0 ? x : (1 == i ? y : z);
}
const float& Vec3f::operator[](const size_t i) const
{
    assert(i < 3); return i <= 0 ? x : (1 == i ? y : z);
}
float Vec3f::norm() const
{
    return std::sqrt(x*x + y*y + z*z);
}
Vec3f& Vec3f::normalize(float coefficient)
{
    *this = (*this) * (coefficient / norm());
    return *this;
}
void Vec3f::rotateX(float angle)
{
    const float Y = y, Z = z, sinA = sin(angle), cosA = cos(angle);
    y = Y * cosA - Z * sinA;
    z = Y * sinA + Z * cosA;
}
void Vec3f::rotateY(float angle)
{
    const float X = x, Z = z, sinA = sin(angle), cosA = cos(angle);
    x = X * cosA + Z * sinA;
    z = -X * sinA + Z * cosA;
}
void Vec3f::rotateZ(float angle)
{
    const float X = x, Y = y, sinA = sin(angle), cosA = cos(angle);
    x = X * cosA - Y * sinA;
    y = X * sinA + Y * cosA;   
}

float operator*(const Vec3f &lhs, const Vec3f &rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vec3f operator+(Vec3f lhs, const Vec3f &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}

Vec3f operator-(Vec3f lhs, const Vec3f &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    return lhs;
}

Vec3f operator*(const Vec3f &lhs, const float &rhs)
{
    return Vec3f(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

Vec3f operator*(const float &lhs, const Vec3f &rhs)
{
    return rhs * lhs;
}

Vec3f operator-(const Vec3f &lhs)
{
    return lhs * float(-1);
}

Vec3f cross(Vec3f v1, Vec3f v2)
{
    return Vec3f(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

std::ostream& operator<<(std::ostream& out, const Vec3f& v)
{
    out << v.x << ' ' << v.y << ' ' << v.z;
    return out;
}

Vec4f::Vec4f(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) { }
Vec4f::Vec4f() : x(0), y(0), z(0), w(0) { }

float& Vec4f::operator[](const size_t i)
{
    assert(i < 4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w));
}
const float& Vec4f::operator[](const size_t i) const
{
    assert(i < 4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w));
}
