#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>

struct Vec3f
{
    float x, y, z;
    Vec3f(float X = 0, float Y = 0, float Z = 0);
    float& operator[](const size_t i);
    const float& operator[](const size_t i) const;
    float Norm() const;
    Vec3f& Normalize();
    void RotateX(float angle);
    void RotateY(float angle);
    void RotateZ(float angle);
    void RotateAxisMatrix(const Vec3f &axis, float angle);
    void RotateAxisQuaternion(const Vec3f &axis, float angle);
};

float operator*(const Vec3f &lhs, const Vec3f &rhs);
Vec3f operator+(Vec3f lhs, const Vec3f &rhs);
Vec3f operator-(Vec3f lhs, const Vec3f &rhs);
Vec3f operator*(const Vec3f &lhs, const float &rhs);
Vec3f operator*(const float &lhs, const Vec3f &rhs);
Vec3f operator-(const Vec3f &lhs);
Vec3f Cross(const Vec3f &v1, const Vec3f &v2);
std::ostream& operator<<(std::ostream& out, const Vec3f& v);

struct Vec4f
{
    float x, y, z, w;
    Vec4f(float X = 0, float Y = 0, float Z = 0, float W = 0);
    float& operator[](const size_t i);
    const float& operator[](const size_t i) const;
};

typedef uint8_t byte;
struct Vec3b
{
    byte r, g, b;
    Vec3b(byte R = 0, byte G = 0, byte B = 0);
    byte& operator[](const size_t i);
    const byte& operator[](const size_t i) const;
};

#endif //VECTOR_HPP
