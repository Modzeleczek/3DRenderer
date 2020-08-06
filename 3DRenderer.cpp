#include "ImageSaver.hpp"
#include "Vector.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include "gif.h"

struct Shape
{
    Vec3f Center, Color;
    Shape(const Vec3f &center, const Vec3f &color) : Center(center), Color(color) {}
    virtual bool RayIntersect(const Vec3f &orig, const Vec3f &dir, float &d) const = 0;
};

struct Sphere : public Shape
{
    float Radius;

    Sphere(const Vec3f &center, const float radius, const Vec3f &color)
        : Shape(center, color), Radius(radius) {}

    bool RayIntersect(const Vec3f &orig, const Vec3f &dir, float &d) const override
    {
        Vec3f L = Center - orig;
        float tca = L*dir;
        float d2 = L*L - tca*tca;
        if (d2 > Radius*Radius) return false;
        float thc = sqrtf(Radius*Radius - d2);
        d = tca - thc;
        float t1 = tca + thc;
        if (d < 0) d = t1;
        if (d < 0) return false;
        return true;
    }
};

struct PlainShape : Shape
{
    Vec3f Normal;
    PlainShape(const Vec3f &center, const Vec3f &normal, const Vec3f &color)
        : Shape(center, color), Normal(normal) {}
    //virtual Vec3f getNormal(const Vec3f &hit) const = 0;
};

struct Circle : PlainShape
{
    float Radius;

    Circle(const Vec3f &center, const float radius, const Vec3f &direction,
    const Vec3f &color)
        : PlainShape(center, direction, color), Radius(radius) {}

    virtual bool RayIntersect(const Vec3f &orig, const Vec3f &dir, float &d) const override
    {
        d = ( Normal*(Center - orig) ) / (Normal*dir);
        return (orig + d*dir - Center).norm() <= Radius;
    }
};

struct Plane : public PlainShape
{
    Plane(const Vec3f &center, const Vec3f &direction, const Vec3f &color)
        : PlainShape(center, direction, color) {}

    virtual bool RayIntersect(const Vec3f &orig, const Vec3f &dir, float &d) const override
    {
        d = ( Normal*(Center - orig) ) / (Normal*dir);
        return d > 0;
    }
};

struct Rectangle : public PlainShape//nie działa dobrze
{
    float Width, Height;

    Rectangle(const Vec3f &center, const float width, const float height,
        const Vec3f &direction, const Vec3f &color)
        : PlainShape(center, direction, color), Width(width), Height(height) {}

    virtual bool RayIntersect(const Vec3f &orig, const Vec3f &dir, float &d) const override
    {
        d = ( Normal*(Center - orig) ) / (Normal*dir);
        /*Vec3f hit(orig + d*dir);
        float dxz = sqrtf(powf(hit.x - Center.x, 2) + powf(hit.z - Center.z, 2)),
            dyz = sqrtf(powf(hit.y - Center.y, 2) + powf(hit.z - Center.z, 2));*/
        float dz2 = powf(orig.z + d*dir.z - Center.z, 2);
        return (sqrtf(powf(orig.x + d*dir.x - Center.x, 2) + dz2) <= Width / 2.f &&//dxz
                sqrtf(powf(orig.y + d*dir.y - Center.y, 2) + dz2) <= Height / 2.f);//dyz
    }
};

int main()
{
    const int width = 800, height = 600;
    const float fov = M_PI / 2.f;
    Vec3f *const bmpBuffer = new Vec3f[width * height], *p = bmpBuffer;

    const int noOfShapes = 4;
    PlainShape *shapes[noOfShapes];

    shapes[0] = new Circle(Vec3f(-2, 3, -12), 2, Vec3f(1, 1, 0).normalize(), Vec3f(0.5, 0, 0));
    shapes[1] = new Plane(Vec3f(-5, -3, -12), Vec3f(1, 0, 0).normalize(), Vec3f(0, 0.5, 0));
    shapes[2] = new Plane(Vec3f(5, -3, -12), Vec3f(-1, 0, 1).normalize(), Vec3f(0.4, 0.4, 0.3));
    shapes[3] = new Plane(Vec3f(0, -4, 0), Vec3f(0, 1, 0).normalize(), Vec3f(0, 0, 1));

    Vec3f color, cameraPosition(0, 0, 0), rayDirection(0, 0, -height / (2.f * tan(fov / 2.f)));
    int y, x, i;

    float closestShapeDistance, distance;
    for(y = 0; y < height; ++y)
    {
        for(x = 0; x < width; ++x)
        {
            rayDirection.x =  x -  width / 2.f;
            rayDirection.y = -y + height / 2.f;
            
            closestShapeDistance = std::numeric_limits<float>::max();
            for(i = 0; i < noOfShapes; ++i)
            {
                if(shapes[i]->RayIntersect(cameraPosition, rayDirection, distance) &&
                   distance < closestShapeDistance)
                {
                    closestShapeDistance = distance;
                    color = shapes[i]->Color;
                }
            }
            if(closestShapeDistance < 1000)
            {
                p->x = 255 * color.x;
                p->y = 255 * color.y;
                p->z = 255 * color.z;
            }
            else
            {
                //background color
                p->x = 255 * 0.f;//r
                p->y = 255 * 0.f;//g
                p->z = 255 * 0.f;//b
            }
            ++p;
        }
    }
    for(i = 0; i < noOfShapes; ++i)
        delete shapes[i];

    BMPSaver("output", width, height, bmpBuffer).Save();
    delete[] bmpBuffer;

    return 0;
}