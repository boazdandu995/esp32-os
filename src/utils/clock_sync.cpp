#include "clock_sync.h"
#include "config/config.h"
#include <Arduino.h>
#include <time.h>

void clockSyncBegin() {
  configTime(NTP_GMT_OFFSET_SEC, NTP_DAYLIGHT_OFFSET_SEC, NTP_SERVER_1, NTP_SERVER_2);

  struct tm timeinfo;
  for (int i = 0; i < 40; i++) {
    if (getLocalTime(&timeinfo, 100)) {
      return;
    }
    delay(200);
  }
}

void clockFormatHHMM(char *buf, size_t len) {
  if (!buf || len < 6) {
    return;
  }

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 5)) {
    snprintf(buf, len, "--:--");
    return;
  }

  strftime(buf, len, "%H:%M", &timeinfo);
}
