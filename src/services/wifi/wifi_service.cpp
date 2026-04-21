#include "wifi_service.h"
#include <WiFi.h>
#include "config/config.h"
#include "../../screen/OledService.h"

extern OledService oledService;

namespace {

volatile uint8_t wifiBootMilestone = 0;

void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_START:
      wifiBootMilestone = 1;
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      wifiBootMilestone = 2;
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      wifiBootMilestone = 3;
      break;
    default:
      break;
  }
}

void showWifiBootProgress(uint8_t milestone) {
  switch (milestone) {
    case 0:
      oledService.showStatusProgress("WiFi setup...", 12);
      break;
    case 1:
      oledService.showStatusProgress("Station started", 25);
      break;
    case 2:
      oledService.showStatusProgress("Link up", 37);
      break;
    case 3:
      oledService.showStatusProgress("Online", 50);
      break;
    default:
      break;
  }
}

void runFakeBootTail() {
  constexpr unsigned long kHoldMs = 3000;
  struct {
    const char *msg;
    int pct;
  } const steps[] = {
      {"Starting services...", 62},
      {"Initializing...", 75},
      {"Almost ready...", 88},
      {"Ready", 100},
  };
  for (const auto &s : steps) {
    oledService.showStatusProgress(s.msg, s.pct);
    delay(kHoldMs);
  }
}

}  // namespace

void WifiService::init() {
  connect();
}

void WifiService::connect() {
  wifiBootMilestone = 0;
  WiFi.onEvent(onWiFiEvent);

  WiFi.mode(WIFI_STA);
  showWifiBootProgress(0);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  uint8_t lastShown = 255;
  const uint32_t t0 = millis();
  constexpr uint32_t kTimeoutMs = 45000;

  while (wifiBootMilestone < 3) {
    const uint8_t m = wifiBootMilestone;
    if (m != lastShown) {
      lastShown = m;
      showWifiBootProgress(m);
    }

    const wl_status_t st = WiFi.status();
    if (st == WL_NO_SSID_AVAIL || st == WL_CONNECT_FAILED) {
      oledService.showStatusProgress("WiFi failed", 0);
      delay(2000);
      return;
    }

    if (millis() - t0 > kTimeoutMs) {
      oledService.showStatusProgress("WiFi timeout", 0);
      delay(2000);
      return;
    }

    delay(50);
  }

  showWifiBootProgress(3);

  runFakeBootTail();
}