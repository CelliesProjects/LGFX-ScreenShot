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

/**
 * @file ScreenShot.h
 * @brief Provides functionality to save LGFX sprites or screen content to a BMP file on SD card.
 */

#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <Arduino.h>
#include <LovyanGFX.h>
#include "MemoryBuffer.h"
#include "ScopedFile.hpp"

/**
 * @class ScreenShot
 * @brief A utility class for saving LGFX screen or sprite data as BMP files to an SD card.
 */
class ScreenShot
{
public:
    bool saveBMP(const char *filename, lgfx::LGFXBase &gfx, FS &filesystem, String &result);
    bool saveBMP(const String &filename, lgfx::LGFXBase &gfx, FS &filesystem, String &error);

private:
    bool writeHeader(lgfx::LGFXBase &gfx, File &file);
    bool writePixelData(lgfx::LGFXBase &gfx, File &file, MemoryBuffer &buffer);
};

#endif // SCREENSHOT_H
