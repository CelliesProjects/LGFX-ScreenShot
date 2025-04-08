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
#include <SD.h>
#include <LovyanGFX.h>

#include "MemoryBuffer.h"
#include "ScopedFile.h"

/**
 * @class ScreenShot
 * @brief A utility class for saving LGFX screen or sprite data as BMP files to an SD card.
 */
class ScreenShot
{
public:
    /**
     * @brief Saves a given LGFX object as a 24-bit BMP file to the SD card.
     *
     * @param filename The path and filename to save the screenshot as (e.g., "/screenshot.bmp").
     * @param gfx The LGFXBase object to capture.
     * @param result A reference to a String to store the error message.
     *
     * @param sdPin \b `Optional` The GPIO pin connected to the SD card CS line.
     * @param frequency \b `Optional` SPI frequency used for SD communication.
     *
     * @return true if successful, false if an error occurred
     *
     * -  \b `result` will contain error message.
     *
     * -  \b `result` will be empty on success.
     */
    bool saveBMP(const char *filename, lgfx::LGFXBase &gfx, String &result, uint8_t sdPin = SS, uint32_t frequency = 4000000);

    /**
     * @brief Overload of saveBMP accepting a String for the filename.
     *
     * @param filename The path and filename to save the screenshot as (e.g., "/screenshot.bmp").
     * @param gfx The LGFXBase object to capture.
     * @param result A reference to a String to store the error message.
     *
     * @param sdPin \b `Optional` The GPIO pin connected to the SD card CS line.
     * @param frequency \b `Optional` SPI frequency used for SD communication.
     *
     * @return true if successful, false if an error occurred
     *
     * -  \b `result` will contain error message.
     *
     * -  \b `result` will be empty on success.
     */
    bool saveBMP(const String &filename, lgfx::LGFXBase &gfx, String &result, uint8_t sdPin = SS, uint32_t frequency = 4000000);

private:
    /**
     * @brief Writes the BMP file header to the provided file.
     *
     * @param gfx The LGFXBase object for width and height information.
     * @param file The File object to write the header to.
     *
     * @return true if the header was written successfully, false otherwise.
     */
    static bool writeHeader(const lgfx::LGFXBase &gfx, File &file);

    /**
     * @brief Writes the pixel data to the provided file.
     *
     * @param gfx The LGFXBase object to read pixel data from.
     * @param file The File object to write the pixel data to.
     * @param buffer The MemoryBuffer used for row data.
     *
     * @return true if the pixel data was written successfully, false otherwise.
     */
    static bool writePixelData(lgfx::LGFXBase &gfx, File &file, MemoryBuffer &buffer);
};

#endif // SCREENSHOT_H