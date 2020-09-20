#include "Vector.hpp"
#include <iostream>
#include <cmath>
#include <cassert>

Vec3f::Vec3f(float X, float Y, float Z) : x(X), y(Y), z(Z) { }

float& Vec3f::operator[](const size_t i)
{
    assert(i < 3); return i <= 0 ? x : (1 == i ? y : z);
}
const float& Vec3f::operator[](const size_t i) const
{
    assert(i < 3); return i <= 0 ? x : (1 == i ? y : z);
}
float Vec3f::Norm() const
{
    return std::sqrt(x*x + y*y + z*z);
}
Vec3f& Vec3f::Normalize()
{
    *this = (*this) * (1.f / Norm());
    return *this;
}
void Vec3f::RotateX(float angle)
{
    const float Y = y, Z = z, sinA = sin(angle), cosA = cos(angle);
    y = Y * cosA - Z * sinA;
    z = Y * sinA + Z * cosA;
}
void Vec3f::RotateY(float angle)
{
    const float X = x, Z = z, sinA = sin(angle), cosA = cos(angle);
    x = X * cosA + Z * sinA;
    z = -X * sinA + Z * cosA;
}
void Vec3f::RotateZ(float angle)
{
    const float X = x, Y = y, sinA = sin(angle), cosA = cos(angle);
    x = X * cosA - Y * sinA;
    y = X * sinA + Y * cosA;   
}
void Vec3f::RotateAxisMatrix(const Vec3f &axis, float angle)
{
    // https://www.continuummechanics.org/rotationmatrix.html
    float vx = x, vy = y, vz = z, c = cos(angle), s = sin(angle);
    x = (c + (1 - c)*axis.x*axis.x)*vx +        ((1 - c)*axis.x*axis.y - s*axis.z)*vy +	((1 - c)*axis.x*axis.z + s*axis.y)*vz;
    y = ((1 - c)*axis.x*axis.y + s*axis.z)*vx +	(c + (1 - c)*axis.y*axis.y)*vy +		((1 - c)*axis.y*axis.z - s*axis.x)*vz;
	z = ((1 - c)*axis.x*axis.z - s*axis.y)*vx +	((1 - c)*axis.y*axis.z + s*axis.x)*vy +	(c + (1 - c)*axis.z*axis.z)*vz;
}
void Vec3f::RotateAxisQuaternion(const Vec3f &axis, float angle)
{
    // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
    // q = cos(angle/2) + (axis.x*i + axis.y*i + axis.z*i)*sin(angle/2)
    const float qr = cos(angle/2), s = sin(angle/2);
    Vec3f qxyz(axis.x * s, axis.y * s, axis.z * s);

    // https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
    *this = 2.0f * (qxyz*(*this)) * qxyz
          + (qr*qr - (qxyz*qxyz)) * (*this)
          + 2.0f * qr * Cross(qxyz, *this);
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

Vec3f Cross(const Vec3f &v1, const Vec3f &v2)
{
    return Vec3f(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}

std::ostream& operator<<(std::ostream& out, const Vec3f& v)
{
    out << v.x << ' ' << v.y << ' ' << v.z;
    return out;
}


Vec4f::Vec4f(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) { }

float& Vec4f::operator[](const size_t i)
{
    assert(i < 4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w));
}
const float& Vec4f::operator[](const size_t i) const
{
    assert(i < 4); return i <= 0 ? x : (1 == i ? y : (2 == i ? z : w));
}


Vec3b::Vec3b(byte R, byte G, byte B) : r(R), g(G), b(B) { }

byte& Vec3b::operator[](const size_t i)
{
    assert(i < 3); return i <= 0 ? r : (1 == i ? g : b);
}
const byte& Vec3b::operator[](const size_t i) const
{
    assert(i < 3); return i <= 0 ? r : (1 == i ? g : b);
}