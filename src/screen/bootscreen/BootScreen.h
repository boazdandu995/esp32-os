#pragma once

#include <Arduino.h>

class DisplayDriver;

class BootScreen {
public:
  static void show(DisplayDriver &display, const String &version);
};
