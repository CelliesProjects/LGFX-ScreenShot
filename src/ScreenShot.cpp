/*
MIT License

Copyright (c) 2025 Cellie https://github.com/CelliesProjects/LGFX-ScreenShot

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ScreenShot.h"

bool ScreenShot::writeHeader(const lgfx::LGFXBase &gfx, File &file)
{
    uint8_t header[54] = {0}; // BMP header (54 bytes)
    // BMP File Header (14 bytes)
    header[0] = 'B';
    header[1] = 'M';
    uint32_t biSizeImage = gfx.width() * gfx.height() * 3; // 3 bytes per pixel (RGB888)
    uint32_t bfSize = 54 + biSizeImage;                    // Total file size
    uint32_t bfOffBits = 54;                               // Offset to pixel data

    // BMP Info Header (40 bytes)
    uint32_t biSize = 40; // Info header size
    int32_t biWidth = gfx.width();
    int32_t biHeight = -gfx.height(); // Negative for top-down order
    uint16_t biPlanes = 1;
    uint16_t biBitCount = 24; // RGB888 format
    uint32_t biCompression = 0;
    int32_t biXPelsPerMeter = 0;
    int32_t biYPelsPerMeter = 0;
    uint32_t biClrUsed = 0;
    uint32_t biClrImportant = 0;

    // Write little endian
    auto writeLE = [](uint8_t *buffer, size_t offset, uint32_t value, uint8_t size)
    {
        for (uint8_t i = 0; i < size; i++)
            buffer[offset + i] = static_cast<uint8_t>((value >> (8 * i)) & 0xFF);
    };

    // Populate the header array with the correct offsets
    writeLE(header, 2, bfSize, 4);           // File size
    writeLE(header, 10, bfOffBits, 4);       // Pixel data offset
    writeLE(header, 14, biSize, 4);          // Info header size
    writeLE(header, 18, biWidth, 4);         // Image width
    writeLE(header, 22, biHeight, 4);        // Image height (negative for top-down)
    writeLE(header, 26, biPlanes, 2);        // Number of planes (always 1)
    writeLE(header, 28, biBitCount, 2);      // Bits per pixel (24-bit RGB)
    writeLE(header, 30, biCompression, 4);   // Compression (0 = none)
    writeLE(header, 34, biSizeImage, 4);     // Image size in bytes
    writeLE(header, 38, biXPelsPerMeter, 4); // Horizontal resolution (not used)
    writeLE(header, 42, biYPelsPerMeter, 4); // Vertical resolution (not used)
    writeLE(header, 46, biClrUsed, 4);       // Colors in palette (not used)
    writeLE(header, 50, biClrImportant, 4);  // Important colors (not used)

    return file.write(header, sizeof(header)) == sizeof(header);
}

bool ScreenShot::writePixelData(lgfx::LGFXBase &gfx, File &file, MemoryBuffer &buffer)
{
    int w = gfx.width();
    int h = gfx.height();
    uint8_t *buf = buffer.get();

    for (int y = 0; y < h; ++y)
    {
        uint8_t *p = buf;
        for (int x = 0; x < w; ++x)
        {
            uint16_t color = gfx.readPixel(x, y);
            // Convert RGB565 → RGB888
            *p++ = ((color >> 0) & 0x1F) * 255 / 31;  // B
            *p++ = ((color >> 5) & 0x3F) * 255 / 63;  // G
            *p++ = ((color >> 11) & 0x1F) * 255 / 31; // R
        }
        if (file.write(buffer.get(), buffer.size()) != buffer.size())
            return false;
    }
    return true;
}

bool ScreenShot::saveBMP(const String &filename, lgfx::LGFXBase &gfx, FS &filesystem, String &result)
{
    return saveBMP(filename.c_str(), gfx, filesystem, result);
}

bool ScreenShot::saveBMP(const char *filename, lgfx::LGFXBase &gfx, FS &filesystem, String &result)
{
    if (gfx.getColorDepth() != 16)
    {
        result = "Only 16-bit color depth supported";
        return false;
    }

    const lgfx::LGFX_Device *dev = static_cast<lgfx::LGFX_Device *>(&gfx);
    if (dev && dev->panel() && !dev->panel()->isReadable())
    {
        result = "Display does not support readPixel()";
        return false;
    }

    MemoryBuffer rowBuffer(gfx.width() * 3);
    if (!rowBuffer.isAllocated())
    {
        result = "Row buffer allocation failed";
        return false;
    }

    ScopedFile scopedFile(filename);
    if (!scopedFile.isValid())
    {
        result = "SD Card mount or file open failed";
        return false;
    }

    File &file = scopedFile.get();

    if (!writeHeader(gfx, file))
    {
        result = "Failed to write bmp header";
        return false;
    }

    if (!writePixelData(gfx, file, rowBuffer))
    {
        result = "Failed to write pixel data";
        return false;
    }

    return true;
}
