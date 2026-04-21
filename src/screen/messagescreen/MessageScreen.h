#pragma once

class DisplayDriver;

class MessageScreen {
public:
  static void showCentered(DisplayDriver &display, const char *text);
  static void clear(DisplayDriver &display);
};
