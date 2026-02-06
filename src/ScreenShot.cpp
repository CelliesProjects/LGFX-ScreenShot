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

#include "ScreenShot.hpp"

bool ScreenShot::writeBMPHeader(lgfx::LGFXBase &gfx, File &file)
{
    uint8_t header[54] = {0};

    // BMP file header
    header[0] = 'B';
    header[1] = 'M';

    uint32_t biSizeImage = rowSize_ * gfx.height();
    uint32_t bfSize = 54 + biSizeImage;
    uint32_t bfOffBits = 54;

    // BMP info header fields
    uint32_t biSize = 40;
    int32_t biWidth = gfx.width();
    int32_t biHeight = -gfx.height(); // top-down bitmap
    uint16_t biPlanes = 1;
    uint16_t biBitCount = 24;   // RGB888
    uint32_t biCompression = 0; // BI_RGB
    int32_t biXPelsPerMeter = 0;
    int32_t biYPelsPerMeter = 0;
    uint32_t biClrUsed = 0;
    uint32_t biClrImportant = 0;

    // Little-endian writer
    auto writeLE = [](uint8_t *buf, size_t offset, uint32_t val, uint8_t size)
    {
        for (uint8_t i = 0; i < size; ++i)
            buf[offset + i] = (val >> (8 * i)) & 0xFF;
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

bool ScreenShot::writeBMPPixelData(lgfx::LGFXBase &gfx, File &file, MemoryBuffer &buffer)
{
    const int w = gfx.width();
    const int h = gfx.height();
    const size_t pixelBytes = w * 3;
    uint8_t *buf = buffer.get();

    for (int y = 0; y < h; ++y)
    {
        uint8_t *p = buf;
        for (int x = 0; x < w; ++x)
        {
            uint16_t color = gfx.readPixel(x, y);
            // RGB565 → RGB888 (BMP uses BGR order)
            *p++ = ((color >> 0) & 0x1F) * 255 / 31;
            *p++ = ((color >> 5) & 0x3F) * 255 / 63;
            *p++ = ((color >> 11) & 0x1F) * 255 / 31;
        }

        // Zero BMP padding bytes
        if (rowSize_ > pixelBytes)
            memset(buf + pixelBytes, 0, rowSize_ - pixelBytes);

        if (file.write(buf, rowSize_) != rowSize_)
            return false;
    }

    return true;
}

bool ScreenShot::saveBMP(const String &filename, lgfx::LGFXBase &gfx, FS &filesystem, String &error)
{
    return saveBMP(filename.c_str(), gfx, filesystem, error);
}

bool ScreenShot::saveBMP(const char *filename, lgfx::LGFXBase &gfx, FS &filesystem, String &error)
{
    if (gfx.getColorDepth() != 16)
    {
        error = "Only 16-bit color depth supported";
        return false;
    }

    if (gfx.width() <= 0 || gfx.height() <= 0)
    {
        error = "Invalid display dimensions";
        return false;
    }

    const lgfx::LGFX_Device *dev = static_cast<lgfx::LGFX_Device *>(&gfx);
    if (dev && dev->panel() && !dev->panel()->isReadable())
    {
        error = "Display does not support readPixel()";
        return false;
    }

    // RGB888 row data needs to be 4 byte aligned and padded
    rowSize_ = (gfx.width() * 3 + 3) & ~3;
    MemoryBuffer pixelBuffer(rowSize_);
    if (!pixelBuffer.isAllocated())
    {
        error = "Failed to allocate pixel buffer";
        return false;
    }

    File file = filesystem.open(filename, FILE_WRITE);
    if (!file)
    {
        error = "Failed to open file";
        return false;
    }

    if (!writeBMPHeader(gfx, file))
    {
        error = "Failed to write bmp header";
        return false;
    }

    if (!writeBMPPixelData(gfx, file, pixelBuffer))
    {
        error = "Failed to write pixel data";
        return false;
    }

    return true;
}
