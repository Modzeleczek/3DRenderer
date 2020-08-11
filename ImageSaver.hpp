#ifndef FILE_HPP
#define FILE_HPP

#include "Vector.hpp"
#include <fstream>

class ImageSaver
{
protected:
    std::string Name;
    unsigned int Width, Height;
    Vec3f *Pixels;
public:
    ImageSaver(std::string name, unsigned int width, unsigned int height, Vec3f *pixels);
    virtual void Save() = 0;
};

class PPMSaver : public ImageSaver
{
public:
    PPMSaver(std::string name, unsigned int width, unsigned int height, Vec3f *pixels);
    virtual void Save();
};

class BMPSaver : public ImageSaver
{
private:
    std::ofstream OutputStream;
    void WriteBytes(unsigned int value, const unsigned char byteCount);
public:
    BMPSaver(std::string name, unsigned int width, unsigned int height, Vec3f *pixels);
    virtual void Save();
};
#endif //FILE_HPP
