#include "../include/ImageSaver.hpp"

void PPMSaver::Save(std::string name, unsigned int width, unsigned int height, Vec3f* pixels)
{
    std::ofstream ofs; // save the framebuffer to file
    ofs.open(name + ".ppm", std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    unsigned int i, j;
    float max;
    for (i = 0; i < height * width; ++i)
    {
        Vec3f &c = pixels[i];
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
void BMPSaver::Save(std::string name, unsigned int width, unsigned int height, Vec3f* pixels)
{
    const unsigned int extraBytes = ( 4 - ((width * 3) % 4) ) % 4;
    OutputStream = std::ofstream(name + ".bmp", std::ios::binary);

    //BMP header
    WriteBytes('B', 1);
    WriteBytes('M', 1);
    WriteBytes(54 + (width * 3 + extraBytes) * height, 4);
    WriteBytes(0, 2);
    WriteBytes(0, 2);
    WriteBytes(54, 4);

    //DIB header
    WriteBytes(40, 4);
    WriteBytes(width, 4);
    WriteBytes(height, 4);
    WriteBytes(1, 2);
    WriteBytes(24, 2);
    WriteBytes(0, 4);

    WriteBytes((width * 3 + extraBytes) * height, 4);
    //WriteValue(output, 0, 4);//też zadziała przy braku kompresji (czyli tak, jak jest standardowo)

    WriteBytes(0, 4);
    WriteBytes(0, 4);
    WriteBytes(0, 4);
    WriteBytes(0, 4);

    //pixel array (bitmap data)

    unsigned int x, y, i = width * (height - 1);
    int j;
    float max;
    for(y = 0; y < height; ++y)
    {
        for(x = 0; x < width; ++x)
        {
            Vec3f &c = pixels[i];
            max = std::max(c[0], std::max(c[1], c[2]));
            if (max > 1) c = c * (1. / max);
            for (j = 2; j >= 0; --j)
                OutputStream.put((char)(255 * std::max(0.f, std::min(1.f, c[j]))));
            ++i;
        }
        for(j = 0; j < extraBytes; ++j)
            OutputStream.put(0);
        i = i - 2 * width;
    }
    OutputStream.close();
}
