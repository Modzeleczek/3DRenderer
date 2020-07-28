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
    virtual bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &d) const = 0;
};

struct Sphere : public Shape
{
    float Radius;

    Sphere(const Vec3f &center, const float radius, const Vec3f &color)
        : Shape(center, color), Radius(radius) {}

    bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &d) const
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

    /*Vec3f getNormal(const Vec3f &hit) const
    {
        return (hit - center).normalize();
    }*/
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

    bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &d) const
    {
        d = ( Normal*(Center - orig) ) / (Normal*dir);
        Vec3f hit(orig + d*dir);
        if( (hit - Center).norm() <= Radius )
            return true;
        return false;
    }
};

struct Plane : public PlainShape
{
    Plane(const Vec3f &center, const Vec3f &direction, const Vec3f &color)
        : PlainShape(center, direction, color) {}

    bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &d) const
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

    bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &d) const
    {
        d = ( Normal*(Center - orig) ) / (Normal*dir);
        /*Vec3f hit(orig + d*dir);
        float dxz = sqrtf(powf(hit.x - Center.x, 2) + powf(hit.z - Center.z, 2)),
            dyz = sqrtf(powf(hit.y - Center.y, 2) + powf(hit.z - Center.z, 2));*/
        float dz2 = powf(orig.z + d*dir.z - Center.z, 2);
        return (sqrtf(powf(orig.x + d*dir.x - Center.x, 2) + dz2) <= Width / 2.f &&//dxz
                sqrtf(powf(orig.y + d*dir.y - Center.y, 2) + dz2) <= Height / 2.f);//dyz
    }

    /*Vec3f getNormal(const Vec3f &hit) const
    {
        return Direction;
    }*/
};

int main()
{
    const int width = 800, height = 600;
    const float fov = M_PI / 2.f;
    uint8_t *gifBuffer = new uint8_t[width * height * 4], *p;
    GifWriter g;
    const int delay = 5;
	GifBegin(&g, "output.gif", width, height, delay);

    const int noOfShapes = 3;
    PlainShape *shapes[noOfShapes];

    /*shapes[0] = new Circle(Vec3f(-5.0, 5, -12), 2, Vec3f(0, 1, 0).normalize(), Vec3f(1, 1, 1));
    shapes[1] = new Circle(Vec3f(7, 5, -18), 0.5, Vec3f(1, 2, 0).normalize(), Vec3f(0, 0, 0));
    shapes[2] = new Circle(Vec3f(-2, -3, -15), 1, Vec3f(2, 1, 0).normalize(), Vec3f(0.4, 0.4, 0.3));*/
    shapes[0] = new Circle(Vec3f(-2, 3, -12), 2, Vec3f(1, 1, 0).normalize(), Vec3f(0.5, 0, 0));
    shapes[1] = new Plane(Vec3f(-5, -3, -12), Vec3f(1, 0, 0).normalize(), Vec3f(0, 0.5, 0));
    shapes[2] = new Plane(Vec3f(5, -3, -12), Vec3f(1, 0, 0).normalize(), Vec3f(0.4, 0.4, 0.3));
    /*shapes[0] = new Rectangle(Vec3f(-5.0, 5, -12), 2, 1.5, Vec3f(0, 1, 0).normalize(), Vec3f(1, 1, 1));
    shapes[1] = new Rectangle(Vec3f(7, 5, -18), 0.5, 0.5, Vec3f(1, 2, 0).normalize(), Vec3f(0, 0, 0));
    shapes[2] = new Rectangle(Vec3f(-2, -3, -15), 1, 0.75, Vec3f(2, 1, 0).normalize(), Vec3f(0.4, 0.4, 0.3));*/

    Vec3f hit, N, color, orig(0, 0, 0), dir(0, 0, -height / (2.f * tan(fov / 2.f)));
    int y, x, i;

    const float velocity = 0.5f, angularVelocity = M_PI / 100.f;

    float shapes_dist, checkerboard_dist, d;
    uint32_t frameCounter = 0;
    while(frameCounter < 50)
    {
        p = gifBuffer;
        for(y = 0; y < height; ++y)
        {
            for(x = 0; x < width; ++x)
            {
                dir.x =  (x + 0.5) -  width / 2.f;
                dir.y = -(y + 0.5) + height / 2.f;
                
                shapes_dist = std::numeric_limits<float>::max();
                for (i = 0; i < noOfShapes; ++i)
                {
                    if (shapes[i]->ray_intersect(orig, dir, d) && d < shapes_dist)
                    {
                        shapes_dist = d;
                        //hit = orig + dir * d;
                        color = shapes[i]->Color;
                    }
                }

                checkerboard_dist = std::numeric_limits<float>::max();
                if (fabs(dir.y) > 1e-3)
                {
                    d = -(orig.y + 4) / dir.y; // the checkerboard plane has equation y = -4
                    hit = orig + dir * d;
                    if (d > 0 /*&& fabs(hit.x) < 10 && hit.z < -5 && hit.z > -30*/ 
                    && d < shapes_dist)
                    {
                        checkerboard_dist = d;
                        N = Vec3f(0, 1, 0);
                        color = (int(.5 * hit.x + 1000) + int(.5 * hit.z)) & 1 ? Vec3f(.3, .3, .3) : Vec3f(.3, .2, .1);
                    }
                }
                d = std::min(shapes_dist, checkerboard_dist);
                if(d < 1000)
                {
                    //TODO: im dalej jest punkt zderzenia promienia z obiektem, tym ciemniejszy ma być piksel
                    *(p++) = 255 * color.x;
                    *(p++) = 255 * color.y;
                    *(p++) = 255 * color.z;
                }
                else
                {
                    //background color
                    *(p++) = 255 * 0.f;//r
                    *(p++) = 255 * 0.f;//g
                    *(p++) = 255 * 0.f;//b
                }
                ++p;//alpha is ignored
            }
        }
        GifWriteFrame(&g, gifBuffer, width, height, delay);
        //shapes[0]->Normal.rotateZ(angularVelocity);
        shapes[0]->Center.x += velocity;
        ++frameCounter;
    }
    for(i = 0; i < noOfShapes; ++i)
        if(shapes[i] != NULL)
            delete shapes[i];

    GifEnd(&g);
    delete[] gifBuffer;

    return 0;
}