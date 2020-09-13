#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>

struct Vec3f
{
    float x, y, z;
    Vec3f(float X, float Y, float Z);
    Vec3f();
    float& operator[](const size_t i);
    const float& operator[](const size_t i) const;
    float norm() const;
    Vec3f& normalize(float coefficient = 1.f);
    void rotateX(float angle);
    void rotateY(float angle);
    void rotateZ(float angle);
    void rotateAxis(const Vec3f &axis, float angle);
};

float operator*(const Vec3f &lhs, const Vec3f &rhs);
Vec3f operator+(Vec3f lhs, const Vec3f &rhs);
Vec3f operator-(Vec3f lhs, const Vec3f &rhs);
Vec3f operator*(const Vec3f &lhs, const float &rhs);
Vec3f operator*(const float &lhs, const Vec3f &rhs);
Vec3f operator-(const Vec3f &lhs);
Vec3f cross(Vec3f v1, Vec3f v2);
std::ostream& operator<<(std::ostream& out, const Vec3f& v);

struct Vec4f
{
    float x, y, z, w;
    Vec4f(float X, float Y, float Z, float W);
    Vec4f();
    float& operator[](const size_t i);
    const float& operator[](const size_t i) const;
};
#endif //VECTOR_HPP
