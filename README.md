# LGFX-ScreenShot

[![License](https://img.shields.io/github/license/CelliesProjects/lgfx-screenshot)](https://github.com/CelliesProjects/lgfx-screenshot/blob/main/LICENSE)
[![Release](https://img.shields.io/github/v/release/CelliesProjects/lgfx-screenshot)](https://github.com/CelliesProjects/lgfx-screenshot/releases/latest)
[![Issues](https://img.shields.io/github/issues/CelliesProjects/lgfx-screenshot)](https://github.com/CelliesProjects/lgfx-screenshot/issues)
[![Last Commit](https://img.shields.io/github/last-commit/CelliesProjects/lgfx-screenshot)](https://github.com/CelliesProjects/lgfx-screenshot/commits)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-green?logo=platformio)](https://registry.platformio.org/libraries/celliesprojects/lgfx-ScreenShot)
[![Arduino](https://img.shields.io/badge/Arduino-ESP32-blue?logo=arduino)](https://www.arduino.cc/)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/5c02977f0816457282ce90c3e4dc6153)](https://app.codacy.com/gh/CelliesProjects/LGFX-ScreenShot/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)

This library allows you to take **screenshots** of a **16-bit RGB565** **`LGFX_Sprite`** or **`LGFX_Device`** display and **save these as 24-bit RGB888 BMP files** on the SD card.  

Designed for ESP32 devices using [LovyanGFX](https://github.com/lovyan03/LovyanGFX).

Psram and LovyanGFX are required to run this library.  
Currently the library only accepts **RGB565** sources.

## Features

- Saves **16-bit RGB565** sources as **24-bit RGB888**  
- Supports both **`LGFX_Sprite`** and **`LGFX_Device`** sources  

## Requirements

- ESP32 (any variant with psram)  
- [LovyanGFX](https://github.com/lovyan03/LovyanGFX)  
- A mounted filesystem 

### Example code - save a screenshot

```c++
#include <Arduino.h>
#include <SD.h>
#include <LovyanGFX.h>
#include <LGFX_AUTODETECT.hpp>
#include <ScreenShot.hpp>

#define SDCARD_SS = 4;

LGFX display;
ScreenShot screenShot;

void setup()
{
    Serial.begin(115200);

    display.begin();
    display.setRotation(1);
    display.setBrightness(110);
    display.clear(TFT_YELLOW);

    // Mount SD card before using
    if (!SD.begin(SDCARD_SS))    
        Serial.println("SD Card mount failed");    

    String error; // returns an error message or returns unchanged on success

    const bool success = screenShot.saveBMP("/screenshot.bmp", display, SD, error);
    if (!success)
        Serial.println(error); // e.g. "Display does not support readPixel()"
    else
        Serial.println("Saved image");

    LGFX_Sprite sprite;

    sprite.setPsram(true);
    sprite.createSprite(320, 240);
    sprite.setFont(&DejaVu24);
    sprite.drawCenterString("Sprite", sprite.width() / 2, sprite.height() / 2);        

    const bool success = screenShot.saveBMP("/spriteshot.bmp", sprite, SD, error);
    if (!success)
        Serial.println(error); // e.g. "Failed to open file"
    else
        Serial.println("Saved image");  
}

void loop()
{
    delay(1000);
}
```

## Known issues

- Some displays have hardware issues that prevent them from reading pixeldata.  
These will return `false` and a `Display does not support readPixel()` error message.  
- Currently the library only accepts **RGB565** sources.

