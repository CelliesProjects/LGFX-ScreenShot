#include <Arduino.h>
#include <LovyanGFX.h>

#define LGFX_M5STACK_CORE2 // for supported devices see https://github.com/lovyan03/LovyanGFX

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

    String error; // returns an error message or returns unchanged on success

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
