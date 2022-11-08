#ifndef RENDERER_CPP
#define RENDERER_CPP

#define _USE_MATH_DEFINES
#include <cmath>
#include <float.h>
#include <vector>
#include <thread>
#include <atomic>
#include "../include/Vector.hpp"
#include "Shapes.cpp"

class LocalCoordinateSystem
{
protected:
    // A unit vector, which indicates coordinate system's horizontal axis.
    Vec3f HorizontalAxis;
    // A unit vector, which indicates coordinate system's vertical axis.
    Vec3f VerticalAxis;
    // A vector, which indicates the facing of the coordinate system.
    Vec3f Direction;
    // The 3 vectors, HorizontalAxis, VerticalAxis and Direction, together make a 
    // local (possibly rotated) coordinate system.
    
    LocalCoordinateSystem(const Vec3f &horizontalAxis = Vec3f(1,0,0),
    const Vec3f &verticalAxis = Vec3f(0,1,0), const Vec3f &direction = Vec3f(0,0,-1))
        : HorizontalAxis(horizontalAxis), VerticalAxis(verticalAxis), Direction(direction) {}

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
    void SetDirection(const Vec3f &direction)
    {
        Direction = direction;
        if(Direction.X == 0 && Direction.Y == 0)
        {
            VerticalAxis = Vec3f(0,1,0);
            if(Direction.Z == -1)
                HorizontalAxis = Vec3f(1,0,0);
            else // if(Direction.Z == 1)
                HorizontalAxis = Vec3f(-1,0,0);
        }
        else
        {
            const float x = Direction.X, y = Direction.Y, z = Direction.Z, commonCoefficient = (1.f + z) / (x*x + y*y);

            // HorizontalAxis = (y^2 * (1 + z) / (x^2 + y^2) - z, -x*y * (1 + z) / (x^2 + y^2), x)
            // VerticalAxis = (-x*y * (1 + z) / (x^2 + y^2), x^2 * (1 + z) / (x^2 + y^2) - z, y)*/

            HorizontalAxis.X = y*y * commonCoefficient - z;
            HorizontalAxis.Y = -x*y * commonCoefficient;
            HorizontalAxis.Z = x;

            VerticalAxis.X = -x*y * commonCoefficient;
            VerticalAxis.Y = x*x * commonCoefficient - z;
            VerticalAxis.Z = y;
        }
    }
};

class Renderer
{
private:
    class Camera : public LocalCoordinateSystem
    {
    public:
        // Position of the camera.
        Vec3f Position;
    private:
        // Distance between camera and screen depending on field of view.
        float ScreenDistance;
        // A vector from camera's position to the center of the screen.
        Vec3f DirectionTimesDistance;
        // Used to compute distance between the camera and the screen.
        const int ScreenHeight;

    public:
        Camera(uint32_t frameHeight = 512, float fieldOfView = M_PI / 3.f, 
            const Vec3f &position = Vec3f(0,0,0)) : ScreenHeight(frameHeight), Position(position)
        {
            SetFieldOfView(fieldOfView);
        }
        void SetFieldOfView(float fieldOfView)
        {
            ScreenDistance = ScreenHeight / (2.f * tan(fieldOfView / 2.f));
            DirectionTimesDistance = Direction * ScreenDistance;
        }
        void RotateX(float angle)
        {
            this->LocalCoordinateSystem::RotateX(angle);
            DirectionTimesDistance = Direction * ScreenDistance;
        }
        void RotateY(float angle)
        {
            this->LocalCoordinateSystem::RotateY(angle);
            DirectionTimesDistance = Direction * ScreenDistance;
        }
        void RotateZ(float angle)
        {
            this->LocalCoordinateSystem::RotateZ(angle);
            DirectionTimesDistance = Direction * ScreenDistance;
        }
        void RotateAxis(const Vec3f &axis, float angle)
        {
            this->LocalCoordinateSystem::RotateAxis(axis, angle);
            DirectionTimesDistance = Direction * ScreenDistance;
        }
        Vec3f GetScreenPixelPosition(const int x, const int y)
        {
            // return HorizontalAxis * x + VerticalAxis * y + DirectionTimesDistance;
            return Vec3f(
                HorizontalAxis.X * x + VerticalAxis.X * y + DirectionTimesDistance.X,
                HorizontalAxis.Y * x + VerticalAxis.Y * y + DirectionTimesDistance.Y,
                HorizontalAxis.Z * x + VerticalAxis.Z * y + DirectionTimesDistance.Z);
        }
        virtual void SetDirection(const Vec3f &direction)
        {
            this->LocalCoordinateSystem::SetDirection(direction);
            DirectionTimesDistance = Direction * ScreenDistance;
        }
    };

public:
    const int Width, Height;
    byte *const FrameBuffer;
    const byte TotalThreads;
    std::vector<Shape*> Shapes;
    std::vector<Light*> Lights;
    Camera Eye;

    Renderer(const uint32_t frameWidth = 512, const uint32_t frameHeight = 512, 
        const byte numberOfThreads = 8)
        : Width(frameWidth), Height(frameHeight), TotalThreads(numberOfThreads),
          FrameBuffer(new byte[Width * Height * 4]), // 4 bytes per pixel (RGBA)
          Eye(frameHeight) {}
    ~Renderer()
    {
        if(FrameBuffer)
            delete[] FrameBuffer;
        for(size_t i = 0; i < Shapes.size(); ++i)
            if(Shapes[i])
                delete Shapes[i];
        for(size_t i = 0; i < Lights.size(); ++i)
            if(Lights[i])
                delete Lights[i];
    }

    void RenderFrame()
    {
        WorkingThreads = TotalThreads;
        int y = Height / 2;
        const int deltaY = Height / TotalThreads;
        for(int i = 0; i < TotalThreads - 1; ++i)
        {
            std::thread(&Renderer::RenderFramePart, this, y, y - deltaY).detach();
            y -= deltaY;
        }
        RenderFramePart(y, y - deltaY);
        while(WorkingThreads > 0);
    }

private:
    std::atomic<byte> WorkingThreads;
    void RenderFramePart(int y, const int endY)
    {
        int x;
        byte *p = FrameBuffer + 4 * Width * (Height / 2 - y); // offset
        for( ; y > endY; --y) // going from top
        {
            for(x = -Width / 2; x < Width / 2; ++x) // going from left
            {
                Vec3b color = static_cast<Vec3b>(CastRay(Eye.Position, Eye.GetScreenPixelPosition(x, y).Normalize()));
                *p = color.R; ++p;
                *p = color.G; ++p;
                *p = color.B; ++p;
                ++p;
                // Adding 4, because every pixel is coded by four bytes. The fourth byte is 
                // alpha value, which is ignored by GifWriter.
            }
        }
        --WorkingThreads;
    }

    bool SceneIntersect(const Vec3f &orig, const Vec3f &dir, Vec3f &closestShapeHitPoint, 
        Vec3f &closestShapeNormal, Material &material)
    {
        byte i;
        float closestShapeDistance = FLT_MAX, distance;
        Vec3f normal, hitPoint;
        for(i = 0; i < Shapes.size(); ++i)
        {
            if(Shapes[i]->RayIntersect(orig, dir, distance, hitPoint, normal) &&
                distance < closestShapeDistance)
            {
                closestShapeDistance = distance;
                closestShapeHitPoint = hitPoint;
                closestShapeNormal = normal;
                material = Shapes[i]->Surface;
            }
        }
        return closestShapeDistance < 1000;
    }

    bool SceneIntersect(const Vec3f &orig, const Vec3f &dir, Vec3f &closestShapeHitPoint, 
        Vec3f &closestShapeNormal)
    {
        byte i;
        float closestShapeDistance = FLT_MAX, distance;
        Vec3f normal, hitPoint;
        for(i = 0; i < Shapes.size(); ++i)
        {
            if(Shapes[i]->RayIntersect(orig, dir, distance, hitPoint, normal) &&
                distance < closestShapeDistance)
            {
                closestShapeDistance = distance;
                closestShapeHitPoint = hitPoint;
                closestShapeNormal = normal;
            }
        }
        return closestShapeDistance < 1000;
    }

    Vec3f CastRay(const Vec3f &orig, const Vec3f &dir, const byte depth = 0)
    {
        Vec3f point, N;
        Material material;

        if (depth>=3 || !SceneIntersect(orig, dir, point, N, material))
            return Vec3f(0.f, 0.f, 0.f); // background color

        Vec3f reflect_dir = reflect(dir, N).Normalize();
        Vec3f refract_dir = refract(dir, N, material.RefractiveIndex).Normalize();
        // Vec3f reflect_orig = reflect_dir*N < 0 ? point - N*1e-3 : point + N*1e-3; // offset the original point to avoid occlusion by the object itself
        Vec3f reflect_orig = point + N*1e-3;
        // Vec3f refract_orig = refract_dir*N < 0 ? point - N*1e-3 : point + N*1e-3;
        Vec3f refract_orig = point - N*1e-3;
        Vec3f reflect_color = CastRay(reflect_orig, reflect_dir, depth + 1);
        Vec3f refract_color = CastRay(refract_orig, refract_dir, depth + 1);

        float diffuse_light_intensity = 0, specular_light_intensity = 0;
        for (size_t i=0; i < Lights.size(); i++)
        {
            Vec3f light_dir      = Lights[i]->Position - point;
            float light_distance = light_dir.NormalizeReturnNorm();

            Vec3f shadow_orig = light_dir*N < 0 ? point - N*1e-3 : point + N*1e-3; // checking if the point lies in the shadow of the Lights[i]
            Vec3f shadow_pt, shadow_N;
            if (SceneIntersect(shadow_orig, light_dir, shadow_pt, shadow_N) 
                && (shadow_pt-shadow_orig).Norm() < light_distance)
                continue;

            diffuse_light_intensity  += Lights[i]->Intensity * std::max(0.f, light_dir*N);
            specular_light_intensity += powf(std::max(0.f, -reflect(-light_dir, N)*dir), 
                                             material.SpecularExponent)*Lights[i]->Intensity;
        }
        return material.DiffuseColor * diffuse_light_intensity * material.Albedo[0] +
            Vec3f(1.f, 1.f, 1.f)*specular_light_intensity * material.Albedo[1] +
            reflect_color*material.Albedo[2] + refract_color*material.Albedo[3];
    }
};
#endif // RENDERER_CPP