#include "ImageSaver.hpp"
#include "Vector.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

struct Circle
{
    float Radius;
    Vec3f Center, Normal, Color;

    Circle(const Vec3f &c, const float radius, const Vec3f normal, const Vec3f color)
        : Radius(radius), Center(c), Normal(normal), Color(color) {}

    bool ray_intersect(const Vec3f &orig, const Vec3f &dir, float &l) const
    {
        l = ( Normal*(Center - orig) ) / (Normal*dir);
        Vec3f p(orig + l*dir);
        if( (p - Center).norm() <= Radius )
            return true;
        return false;
    }
};

int main()
{
    const int width = 800, height = 600;
    const float fov = M_PI / 3.f;
    Vec3f *pixels = new Vec3f[width * height], *p = pixels;

    const int noOfCircles = 3;
    Circle *circles[3];

    circles[0] = new Circle(Vec3f(-1.0, -1.5, -12), 0.25, Vec3f(2, 3, 1).normalize(), Vec3f(0.3, 0.1, 0.1));
    circles[1] = new Circle(Vec3f(7, 5, -18), 0.5, Vec3f(1, 2, 0).normalize(), Vec3f(1.0, 1.0, 1.0));
    circles[2] = new Circle(Vec3f(-2, -3, -15), 1, Vec3f(2, 1, 0).normalize(), Vec3f(0.4, 0.4, 0.3));

    Vec3f hit, N, color, orig(0, 0, 0), dir(0, 0, -height / (2.f * tan(fov / 2.f)));
    int y, x, i;	
    for(y = 0; y < height; ++y)
    {
        for(x = 0; x < width; ++x)
        {
            dir.x =  (x + 0.5) -  width / 2.f;
            dir.y = -(y + 0.5) + height / 2.f;
            
            float shapes_dist = std::numeric_limits<float>::max();
            for (i = 0; i < noOfCircles; ++i)
            {
                float d;
                if (circles[i]->ray_intersect(orig, dir, d) && d < shapes_dist)
                {
                    shapes_dist = d;
                    hit = orig + dir * d;
                    if(hit * circles[i]->Normal > 0)
                        N = -circles[i]->Normal;
                    else
                        N = circles[i]->Normal;
                    color = circles[i]->Color;
                }
            }

            float checkerboard_dist = std::numeric_limits<float>::max();
            if (fabs(dir.y) > 1e-3)
            {
                float d = -(orig.y + 4)/dir.y; // the checkerboard plane has equation y = -4
                Vec3f pt = orig + dir * d;
                if (d > 0 && fabs(pt.x) < 10 && pt.z < -10 && pt.z > -30 && d < shapes_dist)
                {
                    checkerboard_dist = d;
                    hit = pt;
                    N = Vec3f(0, 1, 0);
                    color = (int(.5 * hit.x + 1000) + int(.5 * hit.z)) & 1 ? Vec3f(.3, .3, .3) : Vec3f(.3, .2, .1);
                }
            }
            if(std::min(shapes_dist, checkerboard_dist) < 1000)//return true
                *p = color;//operator=
            else//return false
            {
                p->x = 0.f; p->y = 0.3f; p->z = 0.2f;//background color
            }
            ++p;
        }
    }

    for(i = 0; i < noOfCircles; ++i)
        delete circles[i];

    BMPSaver("output", width, height, pixels).Save();

    delete[] pixels;
    return 0;
}