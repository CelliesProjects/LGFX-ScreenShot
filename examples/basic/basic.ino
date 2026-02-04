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

    // Mount SD card before using
    if (!SD.begin(SDCARD_SS))
    {
        Serial.println("SD Card mount failed");
        while (true)
            delay(100);
    }

    String error; // returns an error message or unchanged on success

    const bool success = screenShot.saveBMP("/screenshot.bmp", display, SD, error);

    if (!success)
        Serial.println(error); // e.g. "file open failed"
    else
        Serial.println("Saved image");
}

void loop()
{
    delay(1000);
}
