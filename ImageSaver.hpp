#ifndef IMAGESAVER_HPP
#define IMAGESAVER_HPP

#include "Vector.hpp"
#include <fstream>

class ImageSaver
{
public:
    virtual void Save() = 0;
};

class PPMSaver : public ImageSaver
{
public:
    virtual void Save(std::string name, unsigned int width, unsigned int height, Vec3f *pixels);
};

class BMPSaver : public ImageSaver
{
private:
    std::ofstream OutputStream;
    void WriteBytes(unsigned int value, const unsigned char byteCount);
public:
    virtual void Save(std::string name, unsigned int width, unsigned int height, Vec3f *pixels);
};
#endif //IMAGESAVER_HPP