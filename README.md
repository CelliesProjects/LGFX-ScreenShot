# LGFX-ScreenShot

This library allows you to take **screenshots** of a **16 bit RGB565** **`LGFX_Sprite`** or **`LGFXBase`** display and **save them as 24-bit BMP files** on the SD card.  
No more need to deal with the poorly supported RGB565 image format any more.

Designed for ESP32 devices using [LovyanGFX](https://github.com/lovyan03/LovyanGFX).

Psram and LovyanGFX are needed to run this library.  
Currently the library only accepts RGB565 sources.

## Features

-  Saves **16bit RGB565** sources as **24bit RGB888**
-  Supports both **`LGFX_Sprite`** and **`LGFX_Device`** sources

## Requirements

-  ESP32 (any variant)
-  LovyanGFX
-  SD card (connected via SPI)

## Installation

Clone into your `lib/` folder or install via PlatformIO:

```ini
lib_deps =
    lovyan03/LovyanGFX
    https://github.com/CelliesProjects/ScreenShot.git
```

## How to use

-  Do **not** mount the sd card in advance, mounting and unmounting is handled by the library.  
-  A `String` has to be passed in with `saveBMP` which returns an error message on error or is unchanged on success.  

## Known issues

-  Some displays are not capable of reading pixeldata.  
These will return `false` and a `Display does not support readPixel()` error message.

## Example code

```c++
#include <Arduino.h>
#include <LovyanGFX.h>
#include <LGFX_AUTODETECT.hpp>
#include <ScreenShot.hpp>

LGFX display;
ScreenShot screenShot;

void setup()
{
    display.begin();
    display.setRotation(1);
    display.setBrightness(110);
    display.clear(TFT_YELLOW);

    String error; // returns unchanged on success or returns an error message

    const bool success = screenShot.saveBMP("/screenshot.bmp", display, error);
    
    if (!success)
        Serial.println(error); // describes the error for example `SD Card mount or file open failed`
    else
        Serial.println("Saved image");
}

void loop()
{
    delay(1000);
}

```
