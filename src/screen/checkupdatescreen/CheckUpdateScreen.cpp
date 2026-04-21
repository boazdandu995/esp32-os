#include "CheckUpdateScreen.h"
#include "../display/DisplayDriver.h"
#include <stdio.h>

void CheckUpdateScreen::show(DisplayDriver &d, uint8_t dotPhase) {
  d.clearBuffer();
  d.setTextColor(WHITE);

  d.drawCenteredLine("Checking for", 18, 1);

  static const char *const dotSuffix[] = {".", "..", "..."};
  const char *dots = dotSuffix[dotPhase % 3];
  char line[20];
  snprintf(line, sizeof(line), "updates%s", dots);
  d.drawCenteredLine(line, 34, 1);

  d.flush();
}
