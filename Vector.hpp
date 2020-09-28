#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>

struct Vec3f
{
    float X, Y, Z;
    Vec3f(float x = 0, float y = 0, float z = 0);
    float& operator[](const size_t i);
    const float& operator[](const size_t i) const;
    Vec3f& operator+=(const Vec3f &v);
    float Norm() const;
    Vec3f& Normalize();
    void RotateX(float angle);
    void RotateX(float sinA, float cosA);
    void RotateY(float angle);
    void RotateY(float sinA, float cosA);
    void RotateZ(float angle);
    void RotateZ(float sinA, float cosA);
    void RotateAxisMatrix(const Vec3f &axis, float angle);
    void RotateAxisQuaternion(const Vec3f &axis, float angle);
};

float operator*(const Vec3f &v1, const Vec3f &v2); // dot product
Vec3f operator+(Vec3f v1, const Vec3f &v2);
Vec3f operator-(Vec3f v1, const Vec3f &v2);
Vec3f operator*(const Vec3f &v, float factor);
Vec3f operator*(float factor, const Vec3f &v);
Vec3f operator-(const Vec3f &v);
Vec3f Cross(const Vec3f &v1, const Vec3f &v2); // cross product
std::ostream& operator<<(std::ostream &out, const Vec3f &v);


typedef uint8_t byte;
struct Vec3b
{
    byte R, G, B;
    Vec3b(byte r = 0, byte g = 0, byte b = 0);
    byte& operator[](const size_t i);
    const byte& operator[](const size_t i) const;
};
#endif //VECTOR_HPP