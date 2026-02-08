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
