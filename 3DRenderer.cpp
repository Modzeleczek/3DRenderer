#include "Vector.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include "gif.h"

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

void CastRay(const Vec3f &origin, const Vec3f &direction, Shape **shapes,
const byte numberOfShapes, byte *p)
{
    byte i, closestIndex = 0;
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
    {
        *(p++) = shapes[closestIndex]->Color.r;
        *(p++) = shapes[closestIndex]->Color.g;
        *(p++) = shapes[closestIndex]->Color.b;
    }
    else
    {
        // background color Vec3b(0,0,0)
        for(i = 0; i < 3; ++i)
            *(p++) = 0;
    }
}

inline Vec3b randomColor()
{
    return Vec3b( rand() & 255, rand() & 255, rand() & 255 );
}
//#define randomColor() {Vec3b( rand() & 255, rand() & 255, rand() & 255 )}

int main()
{
    const int width = 64, height = 64;
    const float fov = M_PI / 3.f;

    GifWriter writer;
    const int delay = 5;
	GifBegin(&writer, "output.gif", width, height, delay);
    byte *const frameBuffer = new byte[width * height * 4], *p;

    const byte noOfShapes = 7;
    Shape **shapes = new Shape*[noOfShapes];

    srand(time(0));
    shapes[0] = new Circle(Vec3f(6,3,-12), 3, Vec3f(1,1,0).normalize(), randomColor());
    shapes[1] = new Plane(Vec3f(-5,-3,-12), Vec3f(1,0,0).normalize(), randomColor());
    shapes[2] = new Plane(Vec3f(5,-3,-12), Vec3f(-1,0,1).normalize(), randomColor());
    shapes[3] = new Plane(Vec3f(0,-4,0), Vec3f(0,1,0).normalize(), randomColor());
    shapes[4] = new Rectangle(Vec3f(6,3,-12), 3, 6, Vec3f(0,0,0), randomColor());
    shapes[5] = new Sphere(Vec3f(0,1,-5), 1, randomColor());
    shapes[6] = new Ellipse(Vec3f(-4,4,-3), Vec3f(-3,-1,-2), 1, Vec3f(0,0,1).normalize(), randomColor());

    Vec3f rotationAxis = Vec3f(1,1,-1).normalize();
    dynamic_cast<Circle*>(shapes[0])->Normal.rotateAxisQuaternion(rotationAxis, M_PI / 2);

    // position of the camera
    const Vec3f cameraPosition(0,0,0);
    // a unit vector, which indicates screen's horizontal axis
    Vec3f screenHorizontal(1,0,0);
    // a unit vector, which indicates screen's vertical axis
    Vec3f screenVertical(0,1,0);
    // a vector, which is perpendicular to the screen (indicates camera's direction)
    Vec3f cameraDirection(0,0,-1);
    // The 3 vectors, screenHorizontal, screenVertical and cameraDirection, together make a 
    // rotated coordinate system.
    const float screenDistance = height / (2.f * tan(fov / 2.f));

    // Negative angle rotates clockwise.
    screenHorizontal.rotateY(-M_PI / 2);
    screenVertical.rotateY(-M_PI / 2);
    cameraDirection.rotateY(-M_PI / 2);

    const uint32_t totalFrames = 512;
    const float rotationVelocity = M_PI * 1.f / (float) totalFrames;
    Vec3f rayDirection;
    int y, x;
    uint32_t frameCounter = 0;
    while(frameCounter < totalFrames)
    {
        p = frameBuffer;
        for(y = height / 2.f; y > -height / 2.f; --y) // going from top
        {
            for(x = -width / 2.f; x < width / 2.f; ++x) // going from left
            {
                rayDirection =
                screenHorizontal * x +
                screenVertical * y +
                cameraDirection * screenDistance;
                CastRay(cameraPosition, rayDirection.normalize(), shapes, noOfShapes, p);
                p += 4;
                // Adding 4, because every pixel is coded by four bytes. The fourth byte is 
                // alpha value, which is ignored by GifWriter.
            }
        }
        screenHorizontal.rotateY(rotationVelocity);
        screenVertical.rotateY(rotationVelocity);
        cameraDirection.rotateY(rotationVelocity);
        GifWriteFrame(&writer, frameBuffer, width, height, delay);
        ++frameCounter;
    }
    for(byte i = 0; i < noOfShapes; ++i)
        delete shapes[i];
    delete[] shapes;

    GifEnd(&writer);
    delete[] frameBuffer;

    return 0;
}