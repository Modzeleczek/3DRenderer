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
    Renderer renderer;
    GifWriter writer;
    const uint32_t delay = 5;
	GifBegin(&writer, "output.gif", renderer.Width, renderer.Height, delay);

    srand(time(0));
    renderer.Shapes.push_back(new Circle(Vec3f(-2,0,-10), 0.5, Vec3f(0,1,1).Normalize(), 
        Vec3b(255,255,255))); // white
    renderer.Shapes.push_back(new Plane(Vec3f(-6,0,-20), Vec3f(1,0,0).Normalize(), 
        Vec3b(255,0,0))); // red
    renderer.Shapes.push_back(new Plane(Vec3f(5,0,-15), Vec3f(0,0,1).Normalize(), 
        Vec3b(0,255,0))); // green
    renderer.Shapes.push_back(new Plane(Vec3f(0,-4,0), Vec3f(0,1,0).Normalize(), 
        Vec3b(0,0,255))); // blue
    renderer.Shapes.push_back(new Rectangle(Vec3f(0,0,-10), 0.5, 0.5, Vec3f(1,1,1).Normalize(), 
        Vec3b(128,128,128))); // gray
    renderer.Shapes.push_back(new Sphere(Vec3f(2,0,-10), 0.5, 
        Vec3b(255,0,255))); // pink
    renderer.Shapes.push_back(new Ellipse(Vec3f(4,0,-10), Vec3f(6,0,-10), 1, Vec3f(0,0,1).Normalize(), 
        Vec3b(0,255,255))); // aqua

    // Negative angle rotates clockwise.
    //renderer.Eye.RotateY(-M_PI / 2);

    const uint32_t totalFrames = 1;
    /*const float rotationVelocity = M_PI * 1.f / (float) totalFrames;
    float targetX = -5.f / 2.f;
    const float velocity = 5.f / totalFrames;*/
    for(uint32_t frameCounter = 0; frameCounter < totalFrames; ++frameCounter)
    {
        //renderer.Eye.SetDirection(Vec3f(targetX,0,-10).Normalize());
        renderer.RenderFrame();
        GifWriteFrame(&writer, renderer.FrameBuffer, 
            renderer.Width, renderer.Height, delay);
        //renderer.Eye.RotateY(rotationVelocity);
        //targetX += velocity;
    }
    GifEnd(&writer);

    return 0;
}