#include <chrono>
#include "Vector.hpp"
#include "Shapes.cpp"
#include "Renderer.cpp"
#include "gif.h"

inline Vec3b randomColor()
{
    return Vec3b( rand() & 255, rand() & 255, rand() & 255 );
}
//#define randomColor() {Vec3b( rand() & 255, rand() & 255, rand() & 255 )}

int main()
{
    Renderer renderer(256, 256);
    GifWriter writer;
    const uint32_t delay = 20;
	GifBegin(&writer, "output.gif", renderer.Width, renderer.Height, delay);

    // predefined materials
    Material      ivory(1.0, Vec4f(0.6,  0.3, 0.1, 0.0), Vec3f(0.4, 0.4, 0.3),   50.);
    Material      glass(1.5, Vec4f(0.0,  0.5, 0.1, 0.8), Vec3f(0.6, 0.7, 0.8),  125.);
    Material red_rubber(1.0, Vec4f(0.9,  0.1, 0.0, 0.0), Vec3f(0.3, 0.1, 0.1),   10.);
    Material     mirror(1.0, Vec4f(0.0, 10.0, 0.8, 0.0), Vec3f(1.0, 1.0, 1.0), 1425.);

    Material blue_rubber(red_rubber.refractive_index, red_rubber.albedo,
        Vec3f(0.1, 0.1, 0.3), red_rubber.specular_exponent);

    // walls
    renderer.Shapes.push_back(new Plane(Vec3f(-6,0,-20), Vec3f(1,0,0).Normalize(), 
        ivory));
    renderer.Shapes.push_back(new Plane(Vec3f(5,0,-15), Vec3f(0,0,1).Normalize(), 
        red_rubber));
    renderer.Shapes.push_back(new Plane(Vec3f(0,-4,0), Vec3f(0,1,0).Normalize(), 
        blue_rubber));

    // shapes
    // renderer.Shapes.push_back(new Circle(Vec3f(-3,0,-10), 2, Vec3f(0,1,1).Normalize(), 
    //    ivory));
    renderer.Shapes.push_back(new Rectangle(Vec3f(3,2,-6), 2, 2, Vec3f(0,0,1).Normalize(), 
        ivory));
    renderer.Shapes.push_back(new Sphere(Vec3f(3,5,-10), 2, 
        mirror));
    // renderer.Shapes.push_back(new Ellipse(Vec3f(6,0,-10), Vec3f(6,0,-10), 1, Vec3f(0,0,1).Normalize(), 
    //    ivory));

    renderer.Lights.push_back(new Light(Vec3f(-5, 10,  -1), 1.5));
    renderer.Lights.push_back(new Light(Vec3f( 5, 10, -1), 1.8));
    renderer.Lights.push_back(new Light(Vec3f( 5, 20,  -1), 1.7));

    // Negative angle rotates clockwise.
    // renderer.Eye.RotateY(-M_PI / 2);

    renderer.Eye.Position.Z = 5;

    const uint32_t totalFrames = 16;
    // const float rotationVelocity = M_PI * 1.f / (float) totalFrames;
    // const float rotationVelocity = M_PI / 180.f;
     float targetX = -5.f / 2.f;
     const float cameraVelocity = 1.f;
    float velocity = -0.25f;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    renderer.Eye.SetDirection(Vec3f(0,0,1));
    renderer.Eye.RotateY(-M_PI / 12.f);
    Rectangle* r = (Rectangle*) renderer.Shapes[3];
    r->SetDirection(Vec3f(1,1,0).Normalize());
    Sphere* s = (Sphere*) renderer.Shapes[4];
    for(uint32_t frameCounter = 0; frameCounter < totalFrames; ++frameCounter)
    {
        // Vec3f dir = s->Center - r->Center;
        // r->SetDirection(dir.Normalize());
         //renderer.Eye.SetDirection(Vec3f(targetX,targetX,-10).Normalize());
        renderer.RenderFrame();
        GifWriteFrame(&writer, renderer.FrameBuffer,
            renderer.Width, renderer.Height, delay);
        // renderer.Eye.RotateY(rotationVelocity);
         //targetX += cameraVelocity;
        
        if(s->Center.Y - s->Radius <= 0)
            velocity = -velocity;
        else if(s->Center.Y + s->Radius >= 10)
            velocity = -velocity;
        s->Center.Y += velocity;
    }
    GifEnd(&writer);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::chrono::nanoseconds difference = end - begin;

    std::cout << renderer.Width << ' ' << renderer.Height << ' ' << renderer.TotalThreads << '\n' <<
        "seconds\t" << std::chrono::duration_cast<std::chrono::seconds>(difference).count() << '\n' <<
        "milli\t" << std::chrono::duration_cast<std::chrono::milliseconds>(difference).count() << '\n' <<
        "micro\t" << std::chrono::duration_cast<std::chrono::microseconds>(difference).count() << '\n' <<
        "nano\t" << std::chrono::duration_cast<std::chrono::nanoseconds>(difference).count() << '\n';

    return 0;
}