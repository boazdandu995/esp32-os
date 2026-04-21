#include "boot_service.h"
#include "../screen/OledService.h"
#include "../config/config.h"

// Access global OLED instance
extern OledService oledService;

void BootService::run() {
  oledService.showBootTitle(FIRMWARE_VERSION);
  delay(2000);
}