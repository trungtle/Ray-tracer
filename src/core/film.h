#ifndef FILM_H
#define FILM_H

#include <memory>
#include <string>

#include "spectrum/spectrum.h"

class Film
{
public:
    Film(unsigned int width, unsigned int height, const std::string& filename) :
        width(width), height(height), filename(filename)
    {
        pixels = std::unique_ptr<Pixel[]>(new Pixel[width * height]);
    }
    
    void SetPixel(const Spectrum& spectrum, int x, int y)
    {
        Pixel pixel;
        spectrum.ToRGB(pixel.rgb);
        pixels[x * width + y] = pixel;
    }
    
    void WriteImage()
    {
        // Output to file
        
        std::ofstream file;
        file.open("../images/" + filename);
        file << "P3\n" << width << " " << height << "\n255\n";
        
        for (int y = height - 1; y >= 0; y--)
        {
            for (int x = 0; x < width; x++)
            {
                Pixel pixel = pixels[x * width + y];
                int ir = int(pixel.rgb[0] * 255.99);
                int ig = int(pixel.rgb[1] * 255.99);
                int ib = int(pixel.rgb[2] * 255.99);
                file << ir << " " << ig << " " << ib << "\n";
            }
        }
        
        file.close();
    }
    
private:
    
    struct Pixel
    {
        float rgb[3] = {0, 0, 0};
    };
    
    std::unique_ptr<Pixel[]> pixels;
    unsigned int width;
    unsigned int height;
    std::string filename;
};

#endif
