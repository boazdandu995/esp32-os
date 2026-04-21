#pragma once

class DisplayDriver;

class HomeScreen {
public:
  static void draw(DisplayDriver &display, const char *timeHHMM);
};
