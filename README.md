# LGFX-ScreenShot

[![License](https://img.shields.io/github/license/CelliesProjects/lgfx-screenshot)](https://github.com/CelliesProjects/lgfx-screenshot/blob/main/LICENSE)
[![Release](https://img.shields.io/github/v/release/CelliesProjects/lgfx-screenshot)](https://github.com/CelliesProjects/lgfx-screenshot/releases/latest)
[![Issues](https://img.shields.io/github/issues/CelliesProjects/lgfx-screenshot)](https://github.com/CelliesProjects/lgfx-screenshot/issues)
[![Last Commit](https://img.shields.io/github/last-commit/CelliesProjects/lgfx-screenshot)](https://github.com/CelliesProjects/lgfx-screenshot/commits)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-green?logo=platformio)](https://registry.platformio.org/libraries/celliesprojects/lgfx-ScreenShot)
[![Arduino](https://img.shields.io/badge/Arduino-ESP32-blue?logo=arduino)](https://www.arduino.cc/)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/5c02977f0816457282ce90c3e4dc6153)](https://app.codacy.com/gh/CelliesProjects/LGFX-ScreenShot/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

This library allows you to take **screenshots** of a **16-bit RGB565 LGFX_Sprite** or **LGFX_Device** display and **save these as 24-bit RGB888 BMP files** on a mounted filesystem.  
I wrote this library because the **RGB565** format is almost unsupported on pc.

Designed for ESP32 devices using [LovyanGFX](https://github.com/lovyan03/LovyanGFX).

Psram and LovyanGFX are required to run this library.  
Currently the library only accepts **RGB565** sources.

## Features

- Saves **16-bit RGB565** sources as **24-bit RGB888**  
- Supports both **LGFX_Sprite** and **LGFX_Device** sources  

## Requirements

- ESP32 (any variant with psram)  
- [LovyanGFX](https://github.com/lovyan03/LovyanGFX)  
- A mounted filesystem 

### Example code - save a BMP to SD card

```c++
#include <Arduino.h>
#include <SD.h>
#include <LovyanGFX.h>
#include <LGFX_AUTODETECT.hpp>
#include <ScreenShot.hpp>

constexpr uint8_t SDCARD_SS = 4;

LGFX display;
ScreenShot screenShot;

void setup()
{
    Serial.begin(115200);

    display.begin();
    display.setRotation(1);
    display.setBrightness(110);
    display.clear(TFT_YELLOW);

    // mount SD card before using
    if (!SD.begin(SDCARD_SS))
        Serial.println("SD Card mount failed");

    // save a screenshot from the display
    auto result = screenShot.saveBMP("/screenshot.bmp", display, SD);
    if (!result.ok())
        Serial.println(screenShotErrorString(result.error));
    else
        Serial.printf("Saved screenshot: %u bytes\n", result.bytesWritten);

    // save a screenshot from a sprite
    LGFX_Sprite sprite;
    sprite.setPsram(true);
    sprite.createSprite(319, 213);
    sprite.setFont(&DejaVu24);
    sprite.drawCenterString("Sprite", sprite.width() / 2, sprite.height() / 2);

    result = screenShot.saveBMP("/spriteshot.bmp", sprite, SD);
    if (!result.ok())
        Serial.println(screenShotErrorString(result.error));
    else
        Serial.printf("Saved spriteshot: %u bytes\n", result.bytesWritten);
}

void loop()
{
    delay(1000);
}
```

## Known issues

- Some displays have no MISO pin connected or exposed, preventing them from reading pixeldata.  
On a call to `saveBMP()` these displays will return `false` and a `Display does not support readPixel()` error message.  
