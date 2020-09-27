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
        // used to compute screen distance
        const int ScreenHeight;

    public:
        Camera(uint32_t frameHeight = 512, float fieldOfView = M_PI / 3.f, const Vec3f &position = Vec3f(0,0,0))
            : ScreenHeight(frameHeight)
        {
            Position = position;
            Direction = Vec3f(0,0,-1);
            HorizontalAxis = Vec3f(1,0,0);
            VerticalAxis = Vec3f(0,1,0);
            SetFieldOfView(fieldOfView);
        }
        void SetFieldOfView(float fieldOfView)
        {
            ScreenDistance = ScreenHeight / (2.f * tan(fieldOfView / 2.f));
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
        void SetDirection(const Vec3f &direction)
        {
            /*
            Algorytm:
            - wyznacz kąt a, o jaki trzeba obrócić wektor (0,0,1) wokół osi X i kąt b, o jaki trzeba obrócić wektor (0,0,1) wokół osi Y, aby otrzymać direction
            - ustaw: HorizontalAxis na domyślną wartość (1,0,0), VerticalAxis na domyślną wartość (0,1,0)
            - obróć HorizontalAxis i VerticalAxis o kąty a, b odpowiednio wokół osi X, Y
            - wpisz direction do Direction
            
            cosB = direction.Z / 1
            sinB = direction.X / 1
            cosA = direction.Z / 1
            sinA = direction.Y / 1
            */

            const float cosA_and_cosB = direction.Z,
                        sinB = direction.X,
                        sinA = direction.Y;

            // set HorizontalAxis to (1,0,0)
            HorizontalAxis = Vec3f(1,0,0);
            // set VerticalAxis to (0,1,0)
            VerticalAxis = Vec3f(0,1,0);

            // vector's coordinates before rotation
            float x, y, z;

            // rotate HorizontalAxis around the X axis
            y = HorizontalAxis.Y;
            z = HorizontalAxis.Z;
            HorizontalAxis.Y = y * cosA_and_cosB - z * sinA;
            HorizontalAxis.Z = y * sinA + z * cosA_and_cosB;

            x = HorizontalAxis.X;
            z = HorizontalAxis.Z;
            // rotate HorizontalAxis around the Y axis
            HorizontalAxis.X = x * cosA_and_cosB + z * sinB;
            HorizontalAxis.Z = -x * sinB + z * cosA_and_cosB;

            // rotate VerticalAxis around the X axis
            y = VerticalAxis.Y;
            z = VerticalAxis.Z;
            VerticalAxis.Y = y * cosA_and_cosB - z * sinA;
            VerticalAxis.Z = y * sinA + z * cosA_and_cosB;

            x = VerticalAxis.X;
            z = VerticalAxis.Z;
            // rotate VerticalAxis around the Y axis
            VerticalAxis.X = x * cosA_and_cosB + z * sinB;
            VerticalAxis.Z = -x * sinB + z * cosA_and_cosB;
            
            Direction = direction;
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
            *(pixelPointer++) = Shapes[closestIndex]->Color.R;
            *(pixelPointer++) = Shapes[closestIndex]->Color.G;
            *(pixelPointer++) = Shapes[closestIndex]->Color.B;
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