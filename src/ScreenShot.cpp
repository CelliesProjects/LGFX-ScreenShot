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

bool ScreenShot::writeBMPHeader(lgfx::LGFXBase &gfx, File &file, size_t &headerSize)
{
    static constexpr size_t BMP_FILE_HEADER_SIZE = 14;
    static constexpr size_t BMP_INFO_HEADER_SIZE = 40;
    static constexpr size_t BMP_HEADER_SIZE =
        BMP_FILE_HEADER_SIZE + BMP_INFO_HEADER_SIZE;

    uint8_t header[BMP_HEADER_SIZE] = {0};

    // Signature
    header[0] = 'B';
    header[1] = 'M';

    const uint32_t biSizeImage = rowSize_ * gfx.height();
    const uint32_t bfOffBits = BMP_HEADER_SIZE;
    const uint32_t bfSize = bfOffBits + biSizeImage;

    // BMP info header fields
    const uint32_t biSize = BMP_INFO_HEADER_SIZE;
    const int32_t biWidth = gfx.width();
    const int32_t biHeight = -gfx.height(); // top-down bitmap
    const uint16_t biPlanes = 1;
    const uint16_t biBitCount = 24;   // RGB888
    const uint32_t biCompression = 0; // BI_RGB
    const int32_t biXPelsPerMeter = 0;
    const int32_t biYPelsPerMeter = 0;
    const uint32_t biClrUsed = 0;
    const uint32_t biClrImportant = 0;

    // Little-endian writer
    auto writeLE = [](uint8_t *buf, size_t offset, uint32_t val, uint8_t size)
    {
        for (uint8_t i = 0; i < size; ++i)
            buf[offset + i] = (val >> (8 * i)) & 0xFF;
    };

    // BITMAPFILEHEADER
    writeLE(header, 2, bfSize, 4);     // bfSize
    writeLE(header, 10, bfOffBits, 4); // bfOffBits

    // BITMAPINFOHEADER
    writeLE(header, 14, biSize, 4);
    writeLE(header, 18, biWidth, 4);
    writeLE(header, 22, biHeight, 4);
    writeLE(header, 26, biPlanes, 2);
    writeLE(header, 28, biBitCount, 2);
    writeLE(header, 30, biCompression, 4);
    writeLE(header, 34, biSizeImage, 4);
    writeLE(header, 38, biXPelsPerMeter, 4);
    writeLE(header, 42, biYPelsPerMeter, 4);
    writeLE(header, 46, biClrUsed, 4);
    writeLE(header, 50, biClrImportant, 4);

    headerSize = BMP_HEADER_SIZE;
    return file.write(header, headerSize) == headerSize;
}

bool ScreenShot::writeBMPPixelData(lgfx::LGFXBase &gfx, File &file, MemoryBuffer &buffer, size_t &bytesWritten)
{
    bytesWritten = 0;

    const int w = gfx.width();
    const int h = gfx.height();
    const size_t pixelBytes = static_cast<size_t>(w) * 3;
    uint8_t *buf = buffer.get();

    for (int y = 0; y < h; ++y)
    {
        uint8_t *p = buf;

        for (int x = 0; x < w; ++x)
        {
            const uint16_t color = gfx.readPixel(x, y);

            // RGB565 → RGB888 (BMP uses BGR order)
            *p++ = ((color >> 0) & 0x1F) * 255 / 31;  // B
            *p++ = ((color >> 5) & 0x3F) * 255 / 63;  // G
            *p++ = ((color >> 11) & 0x1F) * 255 / 31; // R
        }

        // Zero BMP padding bytes
        if (rowSize_ > pixelBytes)
            memset(buf + pixelBytes, 0, rowSize_ - pixelBytes);

        const size_t written = file.write(buf, rowSize_);
        if (written != rowSize_)
            return false;

        bytesWritten += written;
    }

    return true;
}

ScreenShotResult ScreenShot::saveBMP(const String &filename, lgfx::LGFXBase &gfx, FS &filesystem)
{
    return saveBMP(filename.c_str(), gfx, filesystem);
}

ScreenShotResult ScreenShot::saveBMP(const char *filename, lgfx::LGFXBase &gfx, FS &filesystem)
{
    if (gfx.getColorDepth() != 16)
        return {ScreenShotError::UnsupportedColorDepth, 0};

    if (gfx.width() <= 0 || gfx.height() <= 0)
        return {ScreenShotError::InvalidDimensions, 0};

    const lgfx::LGFX_Device *dev =
        static_cast<lgfx::LGFX_Device *>(&gfx);

    if (dev && dev->panel() && !dev->panel()->isReadable())
        return {ScreenShotError::DisplayNotReadable, 0};

    rowSize_ = (gfx.width() * 3 + 3) & ~3;
    MemoryBuffer pixelBuffer(rowSize_);
    if (!pixelBuffer.isAllocated())
        return {ScreenShotError::PixelBufferAllocFailed, 0};

    File file = filesystem.open(filename, FILE_WRITE);
    if (!file)
        return {ScreenShotError::FileOpenFailed, 0};

    size_t headerBytes = 0;
    if (!writeBMPHeader(gfx, file, headerBytes))
        return {ScreenShotError::WriteHeaderFailed, 0};

    size_t pixelBytes = 0;
    if (!writeBMPPixelData(gfx, file, pixelBuffer, pixelBytes))
        return {ScreenShotError::WritePixelDataFailed, 0};

    return {ScreenShotError::None, headerBytes + pixelBytes};
}

const char *screenShotErrorString(ScreenShotError err)
{
    switch (err)
    {
    case ScreenShotError::None:
        return "No error";
    case ScreenShotError::UnsupportedColorDepth:
        return "Only 16-bit color depth supported";
    case ScreenShotError::InvalidDimensions:
        return "Invalid display dimensions";
    case ScreenShotError::DisplayNotReadable:
        return "Display does not support readPixel()";
    case ScreenShotError::PixelBufferAllocFailed:
        return "Failed to allocate pixel buffer";
    case ScreenShotError::FileOpenFailed:
        return "Failed to open file";
    case ScreenShotError::WriteHeaderFailed:
        return "Failed to write BMP header";
    case ScreenShotError::WritePixelDataFailed:
        return "Failed to write pixel data";
    }
    return "Unknown error";
}
