#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class DisplayDriver {
public:
  static constexpr int kWidth = 128;
  static constexpr int kHeight = 64;

  void init();

  void clearBuffer();
  void flush();

  void setTextColor(uint16_t color);
  void setTextSize(uint8_t size);

  void drawCenteredLine(const char *text, int baselineY, uint8_t textSize);

  Adafruit_SSD1306 &gfx() { return display_; }

private:
  Adafruit_SSD1306 display_{kWidth, kHeight, &Wire, -1};
};
