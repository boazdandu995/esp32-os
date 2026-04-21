#include <Arduino.h>
#include "core/app.h"

App app;

void setup() {
  Serial.begin(115200);
  app.init();   // everything starts here
}


void loop() {
  app.loop();   // keep system running
}