#ifndef RENDERER_CPP
#define RENDERER_CPP

#include <cmath>
#include <vector>
#include "Vector.hpp"
#include "Shapes.cpp"

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
        const int FrameHeight;

    public:
        Camera(uint32_t frameHeight = 512, float fieldOfView = M_PI / 3.f, const Vec3f &position = Vec3f(0,0,0))
            : FrameHeight(frameHeight)
        {
            Position = position;
            Direction = Vec3f(0,0,-1);
            HorizontalAxis = Vec3f(1,0,0);
            VerticalAxis = Vec3f(0,1,0);
            SetFieldOfView(fieldOfView);
        }
        void SetFieldOfView(float fieldOfView)
        {
            ScreenDistance = FrameHeight / (2.f * tan(fieldOfView / 2.f));
        }
        void RotateX(float angle)
        {
            Direction.RotateX(angle);
            HorizontalAxis.RotateX(angle);
            VerticalAxis.RotateX(angle);
        }
        void RotateY(float angle)
        {
            Direction.RotateY(angle);
            HorizontalAxis.RotateY(angle);
            VerticalAxis.RotateY(angle);
        }
        void RotateZ(float angle)
        {
            Direction.RotateZ(angle);
            HorizontalAxis.RotateZ(angle);
            VerticalAxis.RotateZ(angle);
        }
        void RotateAxis(const Vec3f &axis, float angle)
        {
            Direction.RotateAxisQuaternion(axis, angle);
            HorizontalAxis.RotateAxisQuaternion(axis, angle);
            VerticalAxis.RotateAxisQuaternion(axis, angle);
        }
        Vec3f GetScreenPixelPosition(const int x, const int y)
        {
            return HorizontalAxis * x + VerticalAxis * y + Direction * ScreenDistance;
        }
    };

public:
    const int Width, Height;
    byte *const FrameBuffer;
    std::vector<Shape*> Shapes;
    Camera Eye;

    Renderer(uint32_t frameWidth = 512, uint32_t frameHeight = 512)
        : Width(frameWidth), Height(frameHeight), 
          FrameBuffer(new byte[Width * Height * 4]), // 4 bytes per pixel (RGBA)
          Eye(frameHeight) {}
    ~Renderer()
    {
        if(FrameBuffer)
            delete[] FrameBuffer;
        for(size_t i = 0; i < Shapes.size(); ++i)
            if(Shapes[i])
                delete Shapes[i];
    }

    void RenderFrame()
    {
        int y, x;
        byte *p = FrameBuffer;
        for(y = Height / 2; y > -Height / 2; --y) // going from top
        {
            for(x = -Width / 2; x < Width / 2; ++x) // going from left
            {
                CastRay(Eye.GetScreenPixelPosition(x, y).Normalize(), p);
                p += 4;
                // Adding 4, because every pixel is coded by four bytes. The fourth byte is 
                // alpha value, which is ignored by GifWriter.
            }
        }
    }

private:
    void CastRay(const Vec3f &direction, byte *pixelPointer)
    {
        byte i, closestIndex = 0;
        float closestShapeDistance = MAXFLOAT, distance;
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
            *(pixelPointer++) = Shapes[closestIndex]->Color.r;
            *(pixelPointer++) = Shapes[closestIndex]->Color.g;
            *(pixelPointer++) = Shapes[closestIndex]->Color.b;
        }
        else
        {
            // background color Vec3b(0,0,0)
            for(i = 0; i < 3; ++i)
                *(pixelPointer++) = 0;
        }
    }
};
#endif // RENDERER_CPP