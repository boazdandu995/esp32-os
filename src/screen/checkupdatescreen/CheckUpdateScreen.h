#pragma once

#include <stdint.h>

class DisplayDriver;

class CheckUpdateScreen {
public:
  static void show(DisplayDriver &display, uint8_t dotPhase);
};
