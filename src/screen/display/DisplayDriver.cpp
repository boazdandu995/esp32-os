#include "DisplayDriver.h"
#include <Wire.h>

void DisplayDriver::init() {
  Wire.begin(21, 22);
  display_.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  clearBuffer();
  flush();
}

void DisplayDriver::clearBuffer() {
  display_.clearDisplay();
}

void DisplayDriver::flush() {
  display_.display();
}

void DisplayDriver::setTextColor(uint16_t color) {
  display_.setTextColor(color);
}

void DisplayDriver::setTextSize(uint8_t size) {
  display_.setTextSize(size);
}

void DisplayDriver::drawCenteredLine(const char *text, int baselineY, uint8_t textSize) {
  display_.setTextSize(textSize);
  int16_t x1, y1;
  uint16_t w, h;
  display_.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  display_.setCursor((kWidth - static_cast<int>(w)) / 2, baselineY);
  display_.print(text);
}
