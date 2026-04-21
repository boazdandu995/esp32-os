#include "StatusScreen.h"
#include "../display/DisplayDriver.h"
#include <stdio.h>

void StatusScreen::show(DisplayDriver &d, const String &status, int percent) {
  Adafruit_SSD1306 &g = d.gfx();

  if (percent < 0) {
    percent = 0;
  }
  if (percent > 100) {
    percent = 100;
  }

  d.clearBuffer();
  d.setTextColor(WHITE);

  d.drawCenteredLine(status.c_str(), 12, 1);

  constexpr int barWidth = 100;
  constexpr int barH = 10;
  const int barX = (DisplayDriver::kWidth - barWidth) / 2;
  constexpr int barY = 26;
  const int fill = (percent * barWidth) / 100;

  g.drawRect(barX, barY, barWidth, barH, WHITE);
  if (fill > 0) {
    g.fillRect(barX, barY, fill, barH, WHITE);
  }

  char pctBuf[8];
  snprintf(pctBuf, sizeof(pctBuf), "%d%%", percent);
  d.drawCenteredLine(pctBuf, barY + barH + 12, 1);

  d.flush();
}
