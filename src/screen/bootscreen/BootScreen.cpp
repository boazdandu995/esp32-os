#include "BootScreen.h"
#include "../display/DisplayDriver.h"

void BootScreen::show(DisplayDriver &d, const String &version) {
  d.clearBuffer();
  d.setTextColor(WHITE);

  d.drawCenteredLine("ESP32", 16, 2);
  d.drawCenteredLine("- IOT", 36, 2);

  const String verLine = "V: " + version;
  d.drawCenteredLine(verLine.c_str(), 52, 1);

  d.flush();
}
