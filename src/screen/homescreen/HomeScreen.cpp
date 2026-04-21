#include "HomeScreen.h"
#include "../display/DisplayDriver.h"

void HomeScreen::draw(DisplayDriver &d, const char *timeHHMM) {
  Adafruit_SSD1306 &g = d.gfx();
  d.clearBuffer();
  d.setTextColor(WHITE);
  d.setTextSize(1);

  if (timeHHMM != nullptr && timeHHMM[0] != '\0') {
    int16_t x1, y1;
    uint16_t w, h;
    g.getTextBounds(timeHHMM, 0, 0, &x1, &y1, &w, &h);
    g.setCursor(DisplayDriver::kWidth - static_cast<int>(w), 8);
    g.print(timeHHMM);
  }

  g.setCursor(0, 22);
  g.print(F("temp: 0.00, hum 0.00"));
  g.setCursor(0, 34);
  g.print(F("dht11: ok"));
  g.setCursor(0, 46);
  g.print(F("system: ok"));

  d.flush();
}
