#include "Vector.hpp"
#include <iostream>
#include <cmath>
#include <cassert>

Vec3f::Vec3f(float x, float y, float z) : X(x), Y(y), Z(z) { }

float& Vec3f::operator[](const size_t i)
{
    assert(i < 3); return i <= 0 ? X : (1 == i ? Y : Z);
}
const float& Vec3f::operator[](const size_t i) const
{
    assert(i < 3); return i <= 0 ? X : (1 == i ? Y : Z);
}
Vec3f& Vec3f::operator+=(const Vec3f &v)
{
    X += v.X;
    Y += v.Y;
    Z += v.Z;
    return *this;
}
float Vec3f::Norm() const
{
    return sqrtf(X*X + Y*Y + Z*Z);
}
Vec3f& Vec3f::Normalize()
{
    *this = (*this) * (1.f / Norm());
    return *this;
}
void Vec3f::RotateX(float angle)
{
    const float y = Y, z = Z, sinA = sin(angle), cosA = cos(angle);
    Y = y * cosA - z * sinA;
    Z = y * sinA + z * cosA;
}
void Vec3f::RotateY(float angle)
{
    const float x = X, z = Z, sinA = sin(angle), cosA = cos(angle);
    X = x * cosA + z * sinA;
    Z = -x * sinA + z * cosA;
}
void Vec3f::RotateZ(float angle)
{
    const float x = X, y = Y, sinA = sin(angle), cosA = cos(angle);
    X = x * cosA - y * sinA;
    Y = x * sinA + y * cosA;   
}
void Vec3f::RotateAxisMatrix(const Vec3f &axis, float angle)
{
    // https://www.continuummechanics.org/rotationmatrix.html
    float vx = X, vy = Y, vz = Z, c = cos(angle), s = sin(angle);
    X = (c + (1 - c)*axis.X*axis.X)*vx +        ((1 - c)*axis.X*axis.Y - s*axis.Z)*vy +	((1 - c)*axis.X*axis.Z + s*axis.Y)*vz;
    Y = ((1 - c)*axis.X*axis.Y + s*axis.Z)*vx +	(c + (1 - c)*axis.Y*axis.Y)*vy +		((1 - c)*axis.Y*axis.Z - s*axis.X)*vz;
	Z = ((1 - c)*axis.X*axis.Z - s*axis.Y)*vx +	((1 - c)*axis.Y*axis.Z + s*axis.X)*vy +	(c + (1 - c)*axis.Z*axis.Z)*vz;
}
void Vec3f::RotateAxisQuaternion(const Vec3f &axis, float angle)
{
    // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
    // q = cos(angle/2) + (axis.x*i + axis.y*j + axis.z*k)*sin(angle/2)
    const float qr = cos(angle / 2.f), s = sin(angle / 2.f);
    Vec3f qxyz(axis.X * s, axis.Y * s, axis.Z * s);

    // https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
    *this = 2.0f * (qxyz*(*this)) * qxyz
          + (qr*qr - (qxyz*qxyz)) * (*this)
          + 2.0f * qr * Cross(qxyz, *this);
}

float operator*(const Vec3f &v1, const Vec3f &v2)
{
    return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
}

Vec3f operator+(Vec3f v1, const Vec3f &v2)
{
    v1.X += v2.X;
    v1.Y += v2.Y;
    v1.Z += v2.Z;
    return v1;
}

Vec3f operator-(Vec3f v1, const Vec3f &v2)
{
    v1.X -= v2.X;
    v1.Y -= v2.Y;
    v1.Z -= v2.Z;
    return v1;
}

Vec3f operator*(const Vec3f &v, float factor)
{
    return Vec3f(v.X * factor, v.Y * factor, v.Z * factor);
}

Vec3f operator*(float factor, const Vec3f &v)
{
    return v * factor;
}

Vec3f operator-(const Vec3f &v)
{
    return v * -1.f;
}

Vec3f Cross(const Vec3f &v1, const Vec3f &v2)
{
    return Vec3f(v1.Y*v2.Z - v1.Z*v2.Y, v1.Z*v2.X - v1.X*v2.Z, v1.X*v2.Y - v1.Y*v2.X);
}

std::ostream& operator<<(std::ostream &out, const Vec3f &v)
{
    out << v.X << ' ' << v.Y << ' ' << v.Z;
    return out;
}


Vec3b::Vec3b(byte r, byte g, byte b) : R(r), G(g), B(b) { }

byte& Vec3b::operator[](const size_t i)
{
    assert(i < 3); return i <= 0 ? R : (1 == i ? G : B);
}
const byte& Vec3b::operator[](const size_t i) const
{
    assert(i < 3); return i <= 0 ? R : (1 == i ? G : B);
}