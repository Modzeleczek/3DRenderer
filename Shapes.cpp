#ifndef SHAPES_CPP
#define SHAPES_CPP

#include "Vector.hpp"
#include <cmath>

struct Shape
{
    Vec3f Center;
    Vec3b Color;
    Shape(const Vec3f &center, const Vec3b &color) : Center(center), Color(color) {}
    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const = 0;
};

struct Sphere : public Shape
{
    float Radius;

    Sphere(const Vec3f &center, const float radius, const Vec3b &color)
        : Shape(center, color), Radius(radius) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        Vec3f L = Center - origin;
        float tca = L*direction;
        float d2 = L*L - tca*tca;
        if (d2 > Radius*Radius) return false;
        float thc = sqrtf(Radius*Radius - d2);
        distance = tca - thc;// thc jest zawsze nieujemne, więc tca - thc jest zawsze mniejsze od tca + thc
        if(distance > 0) return true;
        distance = tca + thc;
        return distance > 0;
    }
};

struct Cube : Shape
{
    float Edge;

    Cube(const Vec3f &center, const float edge, const Vec3b &color)
        : Shape(center, color), Edge(edge) {}
    
    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        return false;
    }
};

struct PlainShape : Shape
{
    Vec3f Normal;
    PlainShape(const Vec3f &center, const Vec3f &normal, const Vec3b &color)
        : Shape(center, color), Normal(normal) {}
    //virtual Vec3f getNormal(const Vec3f &hit) const = 0;
};

struct Circle : PlainShape
{
    float Radius;

    Circle(const Vec3f &center, const float radius, const Vec3f &direction,
    const Vec3b &color)
        : PlainShape(center, direction, color), Radius(radius) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        distance = ( Normal*(Center - origin) ) / (Normal*direction);
        if(distance <= 0) return false;
        return (origin + distance*direction - Center).norm() <= Radius;
    }
};

struct Plane : public PlainShape
{
    Plane(const Vec3f &center, const Vec3f &direction, const Vec3b &color)
        : PlainShape(center, direction, color) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        distance = ( Normal*(Center - origin) ) / (Normal*direction);
        return distance > 0;
    }
};

struct Rectangle : public PlainShape
{
    float Width, Height;

    Rectangle(const Vec3f &center, const float width, const float height,
        const Vec3f &direction, const Vec3b &color)
        : PlainShape(center, direction, color), Width(width), Height(height) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        distance = ( Vec3f(0,0,1)*(Center - origin) ) / (Vec3f(0,0,1)*direction);
        if(distance <= 0) return false;
        Vec3f p = origin + distance*direction;
        return (p.x >= Center.x - Width && p.x < Center.x + Width &&
                p.y >= Center.y - Height && p.y < Center.y + Height);
    }
};

struct Ellipse : public PlainShape
{
    Vec3f Focus2;//Center == Focus1
    float FocusDistanceSum;

    /* additionalFocusesDistance - a value that is added to the distance between ellipse's 
    focuses to avoid situation, when user given FocusDistanceSum is less than actual distance 
    between focuses, so the ellipse does not exist */

    Ellipse(const Vec3f &center1, const Vec3f &center2, const float additionalFocusesDistance,
    const Vec3f &direction, const Vec3b &color)
        : PlainShape(center1, direction, color), Focus2(center2),
        FocusDistanceSum((center1 - center2).norm() + additionalFocusesDistance) {}

    virtual bool RayIntersect(const Vec3f &origin, const Vec3f &direction, float &distance) const override
    {
        distance = ( Normal*(Center - origin) ) / (Normal*direction);
        if(distance <= 0) return false;
        Vec3f p = origin + distance*direction;
        return ((p - Center).norm() + (p - Focus2).norm() <= FocusDistanceSum);
    }
};
#endif // SHAPES_CPP