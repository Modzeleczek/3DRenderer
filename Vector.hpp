#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>

struct Vec3f
{
    float X, Y, Z;
    Vec3f(const float x = 0, const float y = 0, const float z = 0);
    float& operator[](const size_t i);
    const float& operator[](const size_t i) const;
    Vec3f& operator+=(const Vec3f &v);
    static Vec3f Cross(const Vec3f &v1, const Vec3f &v2); // cross product
    float Norm() const;
    Vec3f& Normalize();
    void RotateX(const float angle);
    void RotateX(const float sinA, const float cosA);
    void RotateY(const float angle);
    void RotateY(const float sinA, const float cosA);
    void RotateZ(const float angle);
    void RotateZ(const float sinA, const float cosA);
    void RotateAxisMatrix(const Vec3f &axis, const float angle);
    void RotateAxisQuaternion(const Vec3f &axis, const float angle);
    operator Vec3b();
};

float operator*(const Vec3f &v1, const Vec3f &v2); // dot product
Vec3f operator+(Vec3f v1, const Vec3f &v2);
Vec3f operator-(Vec3f v1, const Vec3f &v2);
Vec3f operator*(const Vec3f &v, const float factor);
Vec3f operator*(const float factor, const Vec3f &v);
Vec3f operator-(const Vec3f &v);
std::ostream& operator<<(std::ostream &out, const Vec3f &v);

Vec3f reflect(const Vec3f &I, const Vec3f &N);
Vec3f refract(const Vec3f &I, const Vec3f &N, const float eta_t, const float eta_i=1.f);


struct Vec4f
{
    float X, Y, Z, W;
    Vec4f(const float x = 0, const float y = 0, const float z = 0, const float w = 0);
    float& operator[](const size_t i);
    const float& operator[](const size_t i) const;
};


typedef uint8_t byte;
struct Vec3b
{
    byte R, G, B;
    Vec3b(const byte r = 0, const byte g = 0, const byte b = 0);
    byte& operator[](const size_t i);
    const byte& operator[](const size_t i) const;
};
#endif //VECTOR_HPP