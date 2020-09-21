#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>

struct Vec3f
{
    float X, Y, Z;
    Vec3f(float x = 0, float y = 0, float z = 0);
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

float operator*(const Vec3f &lhs, const Vec3f &rhs); // dot product
Vec3f operator+(Vec3f lhs, const Vec3f &rhs);
Vec3f operator-(Vec3f lhs, const Vec3f &rhs);
Vec3f operator*(const Vec3f &lhs, const float &rhs);
Vec3f operator*(const float &lhs, const Vec3f &rhs);
Vec3f operator-(const Vec3f &lhs);
Vec3f Cross(const Vec3f &v1, const Vec3f &v2); // cross product
std::ostream& operator<<(std::ostream& out, const Vec3f& v);


typedef uint8_t byte;
struct Vec3b
{
    byte R, G, B;
    Vec3b(byte r = 0, byte g = 0, byte b = 0);
    byte& operator[](const size_t i);
    const byte& operator[](const size_t i) const;
};
#endif //VECTOR_HPP