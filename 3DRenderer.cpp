#include "ImageSaver.hpp"
#include "Vector.hpp"
#include <cmath>
#include <iostream>
#include <limits>

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

    virtual bool RayIntersect(const Vec3f &orig, const Vec3f &dir, float &d) const override
    {
        Vec3f L = Center - orig;
        float tca = L*dir;
        float d2 = L*L - tca*tca;
        if (d2 > Radius*Radius) return false;
        float thc = sqrtf(Radius*Radius - d2);
        d = tca - thc;// thc jest zawsze nieujemne, więc tca - thc jest zawsze mniejsze od tca + thc
        if(d > 0) return true;
        d = tca + thc;
        return d > 0;
    }
};

struct Cube : Shape
{
    float Edge;

    Cube(const Vec3f &center, const float edge, const Vec3f &color)
        : Shape(center, color), Edge(edge) {}
    
    virtual bool RayIntersect(const Vec3f &orig, const Vec3f &dir, float &d) const override
    {
        return false;
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

struct Rectangle : public PlainShape
{
    float Width, Height;

    Rectangle(const Vec3f &center, const float width, const float height,
        const Vec3f &direction, const Vec3f &color)
        : PlainShape(center, direction, color), Width(width), Height(height) {}

    virtual bool RayIntersect(const Vec3f &orig, const Vec3f &dir, float &d) const override
    {
        d = ( Vec3f(0,0,1)*(Center - orig) ) / (Vec3f(0,0,1)*dir);
        Vec3f p = orig + d*dir;
        return (p.x >= Center.x - Width && p.x < Center.x + Width &&
                p.y >= Center.y - Height && p.y < Center.y + Height);
    }
};

struct Ellipse : public PlainShape
{
    Vec3f Focus2;//Center == Focus1
    float FocusDistanceSum;

    //additionalFociDistance - dodatkowa wartość sumy odległości ponad odległość między ogniskami, żeby zapobiec sytuacji, kiedy FocusDistanceSum jest mniejsze niż odległość między ogniskami i elipsa nie istnieje
    Ellipse(const Vec3f &center1, const Vec3f &center2, const float additionalFociDistance,
    const Vec3f &direction, const Vec3f &color)
        : PlainShape(center1, direction, color), Focus2(center2),
        FocusDistanceSum((center1 - center2).norm() + additionalFociDistance) {}

    virtual bool RayIntersect(const Vec3f &orig, const Vec3f &dir, float &d) const override
    {
        d = ( Normal*(Center - orig) ) / (Normal*dir);
        if(d <= 0) return false;
        Vec3f p = orig + d*dir;
        return ((p - Center).norm() + (p - Focus2).norm() <= FocusDistanceSum);
    }
};

void CastRay(const Vec3f &origin, const Vec3f &direction, Shape **shapes,
const u_int8_t numberOfShapes, Vec3f *p)
{
    uint8_t i, closestIndex = 0;
    float closestShapeDistance = std::numeric_limits<float>::max(), distance;
    for(i = 0; i < numberOfShapes; ++i)
    {
        if(shapes[i]->RayIntersect(origin, direction, distance) &&
           distance < closestShapeDistance)
        {
            closestShapeDistance = distance;
            closestIndex = i;
        }
    }
    if(closestShapeDistance < 1000)
        *p = 255 * shapes[closestIndex]->Color;
    else
        //background color
        *p = Vec3f(0.f, 0.f, 0.f);
}

inline Vec3f randomColor()
{
    return Vec3f( (rand() & 255) / 255.f, (rand() & 255) / 255.f, (rand() & 255) / 255.f );
}
//#define randomColor() {Vec3f( (rand() & 255) / 255.f, (rand() & 255) / 255.f, (rand() & 255) / 255.f )}

int main()
{
    const int width = 1280, height = 720;
    const float fov = M_PI / 2.f;
    Vec3f *const bmpBuffer = new Vec3f[width * height], *p = bmpBuffer;

    const uint8_t noOfShapes = 7;
    Shape **shapes = new Shape*[noOfShapes];

    srand(time(0));
    shapes[0] = new Circle(Vec3f(6,3,-12), 3, Vec3f(1,1,0).normalize(), randomColor());
    shapes[1] = new Plane(Vec3f(-5,-3,-12), Vec3f(1,0,0).normalize(), randomColor());
    shapes[2] = new Plane(Vec3f(5,-3,-12), Vec3f(-1,0,1).normalize(), randomColor());
    shapes[3] = new Plane(Vec3f(0,-4,0), Vec3f(0,1,0).normalize(), randomColor());
    shapes[4] = new Rectangle(Vec3f(6,3,-12), 3, 6, Vec3f(0,0,0), randomColor());
    shapes[5] = new Sphere(Vec3f(0,1,-5), 1, randomColor());
    shapes[6] = new Ellipse(Vec3f(-4,4,-3), Vec3f(-3,-1,-2), 1, Vec3f(0,0,1).normalize(), randomColor());

    const float rayZ = -height / (2.f * tan(fov / 2.f));
    const Vec3f cameraPosition(0,0,0);
    Vec3f rayDirection;
    int y, x;
    for(y = 0; y < height; ++y)
    {
        for(x = 0; x < width; ++x)
        {
            rayDirection.x = x - width / 2.f;
            rayDirection.y = -y + height / 2.f;
            rayDirection.z = rayZ;
            CastRay(cameraPosition, rayDirection.normalize(), shapes, noOfShapes, p++);
        }
    }
    for(uint8_t i = 0; i < noOfShapes; ++i)
        delete shapes[i];
    delete[] shapes;

    BMPSaver("output", width, height, bmpBuffer).Save();
    delete[] bmpBuffer;

    return 0;
}