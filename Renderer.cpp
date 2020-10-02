#ifndef RENDERER_CPP
#define RENDERER_CPP

#include <cmath>
#include <vector>
#include <thread>
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
            DirectionTimesDistance = Direction * (ScreenHeight / (2.f * tan(fieldOfView / 2.f)));
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
            // return HorizontalAxis * x + VerticalAxis * y + DirectionTimesDistance;
            return Vec3f(
                HorizontalAxis.X * x + VerticalAxis.X * y + DirectionTimesDistance.X,
                HorizontalAxis.Y * x + VerticalAxis.Y * y + DirectionTimesDistance.Y,
                HorizontalAxis.Z * x + VerticalAxis.Z * y + DirectionTimesDistance.Z);
        }
        void SetDirection(const Vec3f &direction)
        {
            /*
            Algorithm:
            1. assign 'Direction' to 'direction'
            2. compute the angles a and b, by which we need to consecutively rotate the vector (0,0,1) around the X and Y axes in order to get 'direction'
            3. assign: 'HorizontalAxis' to its default value (1,0,0) and 'VerticalAxis' to its default value (0,1,0)
            4. rotate 'HorizontalAxis' and 'VerticalAxis' by the angles a and b around axes X and Y, respectively
            */

            // 1
            Direction = direction;

            // 2
            const float cosA_and_cosB = direction.Z,
                        sinB = direction.X,
                        sinA = direction.Y;

            // 3
            HorizontalAxis = Vec3f(1,0,0);
            VerticalAxis = Vec3f(0,1,0);

            // 4
            HorizontalAxis.RotateX(sinA, cosA_and_cosB);
            HorizontalAxis.RotateY(sinB, cosA_and_cosB);

            VerticalAxis.RotateX(sinA, cosA_and_cosB);
            VerticalAxis.RotateY(sinB, cosA_and_cosB);

            DirectionTimesDistance = Direction * ScreenDistance;
        }
        private:
            Vec3f DirectionTimesDistance;
    };

public:
    const int Width, Height;
    byte *const FrameBuffer;
    const int TotalThreads;
    std::vector<Shape*> Shapes;
    std::vector<Light*> Lights;
    Camera Eye;

    Renderer(const uint32_t frameWidth = 512, const uint32_t frameHeight = 512, 
        const int numberOfThreads = 8)
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
    int WorkingThreads;
    void RenderFramePart(int y, const int endY)
    {
        int x;
        byte *p = FrameBuffer + 4 * Width * (Height / 2 - y); // offset
        for( ; y > endY; --y) // going from top
        {
            for(x = -Width / 2; x < Width / 2; ++x) // going from left
            {
                Vec3b color = static_cast<Vec3b>(cast_ray(Eye.Position, Eye.GetScreenPixelPosition(x, y).Normalize()));
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

    bool scene_intersect(const Vec3f &orig, const Vec3f &dir, Vec3f &closestShapeHitPoint, 
        Vec3f &closestShapeNormal, Material &material)
    {
        byte i;
        float closestShapeDistance = MAXFLOAT, distance;
        Vec3f normal, hitPoint;
        for(i = 0; i < Shapes.size(); ++i)
        {
            if(Shapes[i]->RayIntersect(orig, dir, distance, hitPoint, normal) &&
                distance < closestShapeDistance)
            {
                closestShapeDistance = distance;
                closestShapeHitPoint = hitPoint;
                closestShapeNormal = normal;
                material = Shapes[i]->_material;
            }
        }
        return closestShapeDistance < 1000;
    }

    bool scene_intersect(const Vec3f &orig, const Vec3f &dir, Vec3f &closestShapeHitPoint, 
        Vec3f &closestShapeNormal)
    {
        byte i;
        float closestShapeDistance = MAXFLOAT, distance;
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

    Vec3f cast_ray(const Vec3f &orig, const Vec3f &dir, const byte depth = 0)
    {
        Vec3f point, N;
        Material material;

        if (depth>=3 || !scene_intersect(orig, dir, point, N, material))
            return Vec3f(0.f, 0.f, 0.f); // background color

        Vec3f reflect_dir = reflect(dir, N).Normalize();
        Vec3f refract_dir = refract(dir, N, material.refractive_index).Normalize();
        // Vec3f reflect_orig = reflect_dir*N < 0 ? point - N*1e-3 : point + N*1e-3; // offset the original point to avoid occlusion by the object itself
        Vec3f reflect_orig = point + N*1e-3;
        // Vec3f refract_orig = refract_dir*N < 0 ? point - N*1e-3 : point + N*1e-3;
        Vec3f refract_orig = point - N*1e-3;
        Vec3f reflect_color = cast_ray(reflect_orig, reflect_dir, depth + 1);
        Vec3f refract_color = cast_ray(refract_orig, refract_dir, depth + 1);

        float diffuse_light_intensity = 0, specular_light_intensity = 0;
        for (size_t i=0; i < Lights.size(); i++)
        {
            Vec3f light_dir      = (Lights[i]->position - point).Normalize();
            float light_distance = (Lights[i]->position - point).Norm();

            Vec3f shadow_orig = light_dir*N < 0 ? point - N*1e-3 : point + N*1e-3; // checking if the point lies in the shadow of the Lights[i]
            Vec3f shadow_pt, shadow_N;
            if (scene_intersect(shadow_orig, light_dir, shadow_pt, shadow_N) 
                && (shadow_pt-shadow_orig).Norm() < light_distance)
                continue;

            diffuse_light_intensity  += Lights[i]->intensity * std::max(0.f, light_dir*N);
            specular_light_intensity += powf(std::max(0.f, -reflect(-light_dir, N)*dir), 
                                             material.specular_exponent)*Lights[i]->intensity;
        }
        return material.diffuse_color * diffuse_light_intensity * material.albedo[0] +
            Vec3f(1.f, 1.f, 1.f)*specular_light_intensity * material.albedo[1] +
            reflect_color*material.albedo[2] + refract_color*material.albedo[3];
    }
};
#endif // RENDERER_CPP