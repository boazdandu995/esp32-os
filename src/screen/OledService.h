#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "display/DisplayDriver.h"

// Facade over screen/* components — same API as before for boot / WiFi / OTA / app.
class OledService {
public:
  void init();

  void showBootTitle(String version);
  void showStatusProgress(String status, int percent);
  void showCheckingForUpdates(uint8_t dotPhase);
  void showCenteredMessage(const char *text);
  void clear();
  void showHomeScreen(const char *timeHHMM);

private:
  bool takeDrawLock();
  void giveDrawLock();

  DisplayDriver driver_;
  SemaphoreHandle_t drawMutex_{nullptr};
};
