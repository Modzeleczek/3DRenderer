#include "Vector.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include "gif.h"
#include <vector>

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

class Renderer
{
private:
    class Camera
    {
    public:
        // position of the camera
        Vec3f Position;
    private:
        // a unit vector, which indicates screen's horizontal axis
        Vec3f HorizontalAxis;
        // a unit vector, which indicates screen's vertical axis
        Vec3f VerticalAxis;
        // a vector, which is perpendicular to the screen (indicates camera's direction)
        Vec3f Direction;
        // The 3 vectors, HorizontalAxis, VerticalAxis and Direction, together make a 
        // rotated coordinate system.
        // distance between camera and screen depending on field of view
        float ScreenDistance;

    public:
        Camera(uint32_t frameHeight = 512, float fieldOfView = M_PI / 3.f, const Vec3f &position = Vec3f(0,0,0))
        {
            Position = position;
            Direction = Vec3f(0,0,-1);
            HorizontalAxis = Vec3f(1,0,0);
            VerticalAxis = Vec3f(0,1,0);
            SetFieldOfView(frameHeight, fieldOfView);
        }
        void SetFieldOfView(int frameHeight, float fieldOfView)
        {
            ScreenDistance = frameHeight / (2.f * tan(fieldOfView / 2.f));
        }
        void RotateX(float angle)
        {
            Direction.rotateX(angle);
            HorizontalAxis.rotateX(angle);
            VerticalAxis.rotateX(angle);
        }
        void RotateY(float angle)
        {
            Direction.rotateY(angle);
            HorizontalAxis.rotateY(angle);
            VerticalAxis.rotateY(angle);
        }
        void RotateZ(float angle)
        {
            Direction.rotateZ(angle);
            HorizontalAxis.rotateZ(angle);
            VerticalAxis.rotateZ(angle);
        }
        void RotateAxis(const Vec3f &axis, float angle)
        {
            Direction.rotateAxisQuaternion(axis, angle);
            HorizontalAxis.rotateAxisQuaternion(axis, angle);
            VerticalAxis.rotateAxisQuaternion(axis, angle);
        }
        Vec3f GetScreenPixelPosition(const int x, const int y)
        {
            return HorizontalAxis * x + VerticalAxis * y + Direction * ScreenDistance;
        }
    };

    int Width, Height;
    byte *FrameBuffer;

public:
    std::vector<Shape*> Shapes;

    Camera Eye;
    Renderer(uint32_t frameWidth = 512, uint32_t frameHeight = 512)
    {
        Width = frameWidth;
        Height = frameHeight;
        FrameBuffer = new byte[Width * Height * 4]; // 4 bytes per pixel (RGBA)

        Eye = Camera(frameHeight);
    }
    ~Renderer()
    {
        if(FrameBuffer)
            delete[] FrameBuffer;
        for(size_t i = 0; i < Shapes.size(); ++i)
            if(Shapes[i])
                delete Shapes[i];
    }

    //    int & Width()       { return Width_; } // get, set
    const int & GetWidth() const { return Width; } // get only
    const int & GetHeight() const { return Height; }
    const byte* GetFrameBuffer() const { return FrameBuffer; }

    void RenderFrame()
    {
        int y, x;
        byte *p = FrameBuffer;
        for(y = Height / 2; y > -Height / 2; --y) // going from top
        {
            for(x = -Width / 2; x < Width / 2; ++x) // going from left
            {
                CastRay(Eye.GetScreenPixelPosition(x, y).normalize(), p);
                p += 4;
                // Adding 4, because every pixel is coded by four bytes. The fourth byte is 
                // alpha value, which is ignored by GifWriter.
            }
        }
    }

private:
    void CastRay(const Vec3f &direction, byte *p)
    {
        byte i, closestIndex = 0;
        float closestShapeDistance = std::numeric_limits<float>::max(), distance;
        for(i = 1; i < Shapes.size(); ++i)
        {
            if(Shapes[i]->RayIntersect(Eye.Position, direction, distance) &&
            distance < closestShapeDistance)
            {
                closestShapeDistance = distance;
                closestIndex = i;
            }
        }
        if(closestShapeDistance < 1000)
        {
            *(p++) = Shapes[closestIndex]->Color.r;
            *(p++) = Shapes[closestIndex]->Color.g;
            *(p++) = Shapes[closestIndex]->Color.b;
        }
        else
        {
            // background color Vec3b(0,0,0)
            for(i = 0; i < 3; ++i)
                *(p++) = 0;
        }
    }
};

inline Vec3b randomColor()
{
    return Vec3b( rand() & 255, rand() & 255, rand() & 255 );
}
//#define randomColor() {Vec3b( rand() & 255, rand() & 255, rand() & 255 )}

int main()
{
    Renderer renderer;
    GifWriter writer;
    const uint32_t delay = 5;
	GifBegin(&writer, "output.gif", renderer.GetWidth(), renderer.GetHeight(), delay);

    srand(time(0));
    renderer.Shapes.push_back(new Circle(Vec3f(6,3,-12), 3, Vec3f(1,1,0).normalize(), randomColor()));
    renderer.Shapes.push_back(new Plane(Vec3f(-5,-3,-12), Vec3f(1,0,0).normalize(), randomColor()));
    renderer.Shapes.push_back(new Plane(Vec3f(5,-3,-12), Vec3f(-1,0,1).normalize(), randomColor()));
    renderer.Shapes.push_back(new Plane(Vec3f(0,-4,0), Vec3f(0,1,0).normalize(), randomColor()));
    renderer.Shapes.push_back(new Rectangle(Vec3f(6,3,-12), 3, 6, Vec3f(0,0,0), randomColor()));
    renderer.Shapes.push_back(new Sphere(Vec3f(0,1,-5), 1, randomColor()));
    renderer.Shapes.push_back(new Ellipse(Vec3f(-4,4,-3), Vec3f(-3,-1,-2), 1, Vec3f(0,0,1).normalize(), 
        randomColor()));

    // Negative angle rotates clockwise.
    renderer.Eye.RotateY(-M_PI / 2);

    const uint32_t totalFrames = 256;
    const float rotationVelocity = M_PI * 1.f / (float) totalFrames;
    uint32_t frameCounter = 0;
    while(frameCounter < totalFrames)
    {
        renderer.RenderFrame();
        GifWriteFrame(&writer, renderer.GetFrameBuffer(), 
            renderer.GetWidth(), renderer.GetHeight(), delay);
        renderer.Eye.RotateY(rotationVelocity);
        ++frameCounter;
    }
    GifEnd(&writer);

    return 0;
}