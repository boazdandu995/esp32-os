#include "MessageScreen.h"
#include "../display/DisplayDriver.h"

void MessageScreen::showCentered(DisplayDriver &display, const char *text) {
  display.clearBuffer();
  display.setTextColor(WHITE);
  display.drawCenteredLine(text, 30, 1);
  display.flush();
}

void MessageScreen::clear(DisplayDriver &display) {
  display.clearBuffer();
  display.flush();
}
