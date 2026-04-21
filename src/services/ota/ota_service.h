#pragma once

#include <Arduino.h>

class OtaService {
public:

  bool isUpdateAvaiable();
  void performUpdate();

  // Downloads firmware from url (http:// for LAN testing). On success the device reboots.
  // Returns false if skipped, failed, or no network; true only if reboot is about to occur.
  bool tryUpdateFromUrl(const char *url);

  bool tryConfiguredUpdate();


};
