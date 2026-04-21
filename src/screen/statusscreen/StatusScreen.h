#pragma once

#include <Arduino.h>

class DisplayDriver;

class StatusScreen {
public:
  static void show(DisplayDriver &display, const String &status, int percent);
};
