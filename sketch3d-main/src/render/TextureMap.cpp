#include "render/TextureMap.h"

#include "system/Logger.h"

#include <FreeImage.h>

namespace Sketch3D
{

TextureMap::TextureMap(unsigned char* data, size_t width, size_t height, size_t depth) : data_(data), width_(width), height_(height), depth_(depth)
{
}

TextureMap::~TextureMap()
{
    delete[] data_;
}

unsigned char* TextureMap::GetData() const
{
    return data_;
}

unsigned char*& TextureMap::GetData()
{
    return data_;
}

size_t TextureMap::GetWidth() const
{
    return width_;
}

size_t TextureMap::GetHeight() const
{
    return height_;
}

size_t TextureMap::GetDepth() const
{
    return depth_;
}

bool LoadTextureMapFromFile(const string& filename, shared_ptr<TextureMap>& textureMap)
{
    FREE_IMAGE_FORMAT format = FIF_UNKNOWN;

    format = FreeImage_GetFileType(filename.c_str());
    if (format == FIF_UNKNOWN)
    {
        format = FreeImage_GetFIFFromFilename(filename.c_str());
    }

    if ((format == FIF_UNKNOWN) || !FreeImage_FIFSupportsReading(format))
    {
        Logger::GetInstance()->Error("File format unsupported for image " + filename);
        return false;
    }

    FIBITMAP* dib = FreeImage_Load(format, filename.c_str());
    if (dib == nullptr)
    {
        Logger::GetInstance()->Error("Couldn't load image " + filename);
        return false;
    }

    size_t width = FreeImage_GetWidth(dib);
    size_t height = FreeImage_GetHeight(dib);
    size_t bpp = FreeImage_GetBPP(dib);
    size_t pitch = FreeImage_GetPitch(dib);

    size_t bytesPerPixel = bpp / 8;

    size_t size = height * pitch;
    unsigned char* textureData = (unsigned char*)malloc(size);

    size_t pad = (bytesPerPixel == 3) ? 1 : 0;
    size += (pad * width * height);
    
    unsigned char* data = (unsigned char*)malloc(size);
    FreeImage_ConvertToRawBits(textureData, dib, pitch, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, FALSE);

    // We store the texture data in rgba, where r is LSB and a is MSB
    size_t idx = 0;
    size_t textureIdx = 0;

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            if (bytesPerPixel == 4)
            {
                data[idx    ] = textureData[textureIdx + 3];
                data[idx + 1] = textureData[textureIdx + 2];
                data[idx + 2] = textureData[textureIdx + 1];
                data[idx + 3] = textureData[textureIdx    ];
            }
            else if (bytesPerPixel == 3)
            {
                data[idx    ] = textureData[textureIdx + 2];
                data[idx + 1] = textureData[textureIdx + 1];
                data[idx + 2] = textureData[textureIdx    ];
                data[idx + 3] = 255;
            }
            
            idx += bytesPerPixel + pad;
            textureIdx += bytesPerPixel;
        }
    }

    free(textureData);
    FreeImage_Unload(dib);
    
    textureMap.reset(new TextureMap(data, width, height));

    Logger::GetInstance()->Info("Successfully loaded texture map " + filename);

    return true;
}

}