#include "ImageSaver.hpp"

ImageSaver::ImageSaver(std::string name, unsigned int width, unsigned int height, Vec3f* pixels)
    : Name(name), Width(width), Height(height), Pixels(pixels) { }

PPMSaver::PPMSaver(std::string name, unsigned int width, unsigned int height, Vec3f* pixels)
    : ImageSaver(name, width, height, pixels) { }
void PPMSaver::Save()
{
    std::ofstream ofs; // save the framebuffer to file
    ofs.open(Name + ".ppm", std::ios::binary);
    ofs << "P6\n" << Width << " " << Height << "\n255\n";
    unsigned int i, j;
    float max;
    for (i = 0; i < Height * Width; ++i)
    {
        Vec3f &c = Pixels[i];
        max = std::max(c[0], std::max(c[1], c[2]));
        if (max > 1) c = c * (1. / max);
        for (j = 0; j < 3; ++j)
        {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, c[j])));
        }
    }
    ofs.close();
}

void BMPSaver::WriteBytes(unsigned int value, const unsigned char byteCount)
{
    for(unsigned int i = 0; i < byteCount; ++i)
    {
        OutputStream.put(value % 256);
        value /= 256;
    }
}
BMPSaver::BMPSaver(std::string name, unsigned int width, unsigned int height, Vec3f* pixels)
    : ImageSaver(name, width, height, pixels) { }
void BMPSaver::Save()
{
    const unsigned int extraBytes = ( 4 - ((Width * 3) % 4) ) % 4;
    OutputStream = std::ofstream(Name + ".bmp", std::ios::binary);

    //BMP header
    WriteBytes('B', 1);
    WriteBytes('M', 1);
    WriteBytes(54 + (Width * 3 + extraBytes) * Height, 4);
    WriteBytes(0, 2);
    WriteBytes(0, 2);
    WriteBytes(54, 4);

    //DIB header
    WriteBytes(40, 4);
    WriteBytes(Width, 4);
    WriteBytes(Height, 4);
    WriteBytes(1, 2);
    WriteBytes(24, 2);
    WriteBytes(0, 4);

    WriteBytes((Width * 3 + extraBytes) * Height, 4);
    //WriteValue(output, 0, 4);//też zadziała przy braku kompresji (czyli tak, jak jest standardowo)

    WriteBytes(0, 4);
    WriteBytes(0, 4);
    WriteBytes(0, 4);
    WriteBytes(0, 4);

    //pixel array (bitmap data)

    unsigned int x, y, i = Width * (Height - 1);
    int j;
    float max;
    for(y = 0; y < Height; ++y)
    {
        for(x = 0; x < Width; ++x)
        {
            Vec3f &c = Pixels[i];
            max = std::max(c[0], std::max(c[1], c[2]));
            if (max > 1) c = c * (1. / max);
            for (j = 2; j >= 0; --j)
                OutputStream.put((char)(255 * std::max(0.f, std::min(1.f, c[j]))));
            ++i;
        }
        for(j = 0; j < extraBytes; ++j)
            OutputStream.put(0);
        i = i - 2 * Width;
    }
    OutputStream.close();
}
