#include "Vector.hpp"
#include <iostream>
#include <cmath>
#include <cassert>

Vec3f::Vec3f(const float x, const float y, const float z) : X(x), Y(y), Z(z) {}

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
Vec3f Vec3f::Cross(const Vec3f &v1, const Vec3f &v2)
{
    return Vec3f(v1.Y*v2.Z - v1.Z*v2.Y, v1.Z*v2.X - v1.X*v2.Z, v1.X*v2.Y - v1.Y*v2.X);
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
Vec3f reflect(const Vec3f &I, const Vec3f &N)
{
    /* Rotation of I vector by 180 degrees around N vector using quaternion.
    qr = cos(180/2) = 0, s = sin(180/2) = 1
    qxyz = (N.X * 1, N.Y * 1, N.Z * 1) = N
    |N| = |I| = 1
    I' = 2.0f * (N*I) * N + (0 - (N*N)) * I = 2.0f * (N*I) * N + (0 - (1^2)) * I = 2.0f * (N*I) * N - I 
    The rotated I' vector is still pointing at the point, where the ray hit, so it has to be inversed in order to be fully reflected.
    result - -I' = I - 2.0f * (N*I) * N */
    return I - N*2.f*(I*N);
}
Vec3f refract(const Vec3f &I, const Vec3f &N, const float eta_t, const float eta_i=1.f)
{ // Snell's law
    float cosi = - std::max(-1.f, std::min(1.f, I*N));
    if (cosi<0) return refract(I, -N, eta_i, eta_t); // if the ray comes from the inside the object, swap the air and the media
    float eta = eta_i / eta_t;
    float k = 1 - eta*eta*(1 - cosi*cosi);
    return k<0 ? Vec3f(1,0,0) : I*eta + N*(eta*cosi - sqrtf(k)); // k<0 = total reflection, no ray to refract. I refract it anyways, this has no physical meaning
}
void Vec3f::RotateX(const float angle) { RotateX(sin(angle), cos(angle)); }
void Vec3f::RotateX(const float sinA, const float cosA)
{
    const float y = Y, z = Z;
    Y = y * cosA - z * sinA;
    Z = y * sinA + z * cosA;
}
void Vec3f::RotateY(const float angle) { RotateY(sin(angle), cos(angle)); }
void Vec3f::RotateY(const float sinA, const float cosA)
{
    const float x = X, z = Z;
    X = x * cosA + z * sinA;
    Z = -x * sinA + z * cosA;
}
void Vec3f::RotateZ(const float angle) { RotateZ(sin(angle), cos(angle)); }
void Vec3f::RotateZ(const float sinA, const float cosA)
{
    const float x = X, y = Y;
    X = x * cosA - y * sinA;
    Y = x * sinA + y * cosA;   
}
void Vec3f::RotateAxisMatrix(const Vec3f &axis, const float angle)
{
    // https://www.continuummechanics.org/rotationmatrix.html
    float vx = X, vy = Y, vz = Z, c = cos(angle), s = sin(angle);
    X = (c + (1 - c)*axis.X*axis.X)*vx +        ((1 - c)*axis.X*axis.Y - s*axis.Z)*vy +	((1 - c)*axis.X*axis.Z + s*axis.Y)*vz;
    Y = ((1 - c)*axis.X*axis.Y + s*axis.Z)*vx +	(c + (1 - c)*axis.Y*axis.Y)*vy +		((1 - c)*axis.Y*axis.Z - s*axis.X)*vz;
	Z = ((1 - c)*axis.X*axis.Z - s*axis.Y)*vx +	((1 - c)*axis.Y*axis.Z + s*axis.X)*vy +	(c + (1 - c)*axis.Z*axis.Z)*vz;
}
void Vec3f::RotateAxisQuaternion(const Vec3f &axis, const float angle)
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
Vec3f::operator Vec3b()
{
    return Vec3b(255 * X, 255 * Y, 255 * Z);
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

Vec3f operator*(const Vec3f &v, const float factor)
{
    return Vec3f(v.X * factor, v.Y * factor, v.Z * factor);
}

Vec3f operator*(const float factor, const Vec3f &v)
{
    return v * factor;
}

Vec3f operator-(const Vec3f &v)
{
    return v * -1.f;
}

std::ostream& operator<<(std::ostream &out, const Vec3f &v)
{
    out << v.X << ' ' << v.Y << ' ' << v.Z;
    return out;
}


Vec3b::Vec3b(const byte r, const byte g, const byte b) : R(r), G(g), B(b) {}

byte& Vec3b::operator[](const size_t i)
{
    assert(i < 3); return i <= 0 ? R : (1 == i ? G : B);
}
const byte& Vec3b::operator[](const size_t i) const
{
    assert(i < 3); return i <= 0 ? R : (1 == i ? G : B);
}


Vec4f::Vec4f(const float x, const float y, const float z, const float w) : X(x), Y(y), Z(z), W(w) {}

float& Vec4f::operator[](const size_t i)
{
    assert(i < 4); return i <= 0 ? X : (1 == i ? Y : (2 == i ? Z : W));
}
const float& Vec4f::operator[](const size_t i) const
{
    assert(i < 4); return i <= 0 ? X : (1 == i ? Y : (2 == i ? Z : W));
}