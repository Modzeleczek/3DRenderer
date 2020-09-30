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
    const uint32_t delay = 5;
	GifBegin(&writer, "output.gif", renderer.Width, renderer.Height, delay);

    // predefined materials
    Material      ivory(1.0, Vec4f(0.6,  0.3, 0.1, 0.0), Vec3f(0.4, 0.4, 0.3),   50.);
    Material      glass(1.5, Vec4f(0.0,  0.5, 0.1, 0.8), Vec3f(0.6, 0.7, 0.8),  125.);
    Material red_rubber(1.0, Vec4f(0.9,  0.1, 0.0, 0.0), Vec3f(0.3, 0.1, 0.1),   10.);
    Material     mirror(1.0, Vec4f(0.0, 10.0, 0.8, 0.0), Vec3f(1.0, 1.0, 1.0), 1425.);

    srand(time(0));
    // walls
    renderer.Shapes.push_back(new Plane(Vec3f(-6,0,-20), Vec3f(1,0,0).Normalize(), 
        red_rubber));
    renderer.Shapes.push_back(new Plane(Vec3f(5,0,-15), Vec3f(0,0,1).Normalize(), 
        red_rubber));
    renderer.Shapes.push_back(new Plane(Vec3f(0,-4,0), Vec3f(0,1,0).Normalize(), 
        red_rubber));

    // shapes
    renderer.Shapes.push_back(new Circle(Vec3f(-3,0,-10), 2, Vec3f(0,1,1).Normalize(), 
        ivory));
    renderer.Shapes.push_back(new Rectangle(Vec3f(3,0,-5), 4, 4, Vec3f(0,0,1).Normalize(), 
        glass));
    renderer.Shapes.push_back(new Sphere(Vec3f(3,0,-10), 2, 
        mirror));
    // renderer.Shapes.push_back(new Ellipse(Vec3f(6,0,-10), Vec3f(6,0,-10), 1, Vec3f(0,0,1).Normalize(), 
    //    ivory));

    renderer.Lights.push_back(new Light(Vec3f(-5, 10,  -1), 1.5));
    renderer.Lights.push_back(new Light(Vec3f( 5, 10, -1), 1.8));
    // renderer.Lights.push_back(new Light(Vec3f( 30, 20,  -1), 1.7));

    // Negative angle rotates clockwise.
    // renderer.Eye.RotateY(-M_PI / 2);

    const uint32_t totalFrames = 32;
    // const float rotationVelocity = M_PI * 1.f / (float) totalFrames;
    // float targetX = -5.f / 2.f;
    const float velocity = 5.f / totalFrames;
    for(uint32_t frameCounter = 0; frameCounter < totalFrames; ++frameCounter)
    {
        // renderer.Eye.SetDirection(Vec3f(targetX,0,-10).Normalize());
        renderer.RenderFrame();
        GifWriteFrame(&writer, renderer.FrameBuffer, 
            renderer.Width, renderer.Height, delay);
        // renderer.Eye.RotateY(rotationVelocity);
        // targetX += velocity;
        // dynamic_cast<Circle*>(renderer.Shapes[3])->SetDirection
        renderer.Shapes[3]->Center.X += velocity;
    }
    GifEnd(&writer);

    return 0;
}