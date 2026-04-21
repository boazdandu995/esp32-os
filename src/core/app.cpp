#include "app.h"
#include "../boot/boot_service.h"
#include "../screen/OledService.h"
#include "../services/wifi/wifi_service.h"
#include "../services/ota/ota_service.h"
#include "../utils/clock_sync.h"
#include "../tasks/system/active_led.h"
#include "../tasks/ota-periodic-check/ota-periodic-check.h"

// Create global services
OledService oledService;
WifiService wifiService;
BootService bootService;
OtaService otaService;

void App::init() {

    startLedTask();

    oledService.init();

    // STEP 1: Boot screen FIRST
    bootService.run();

    // STEP 2: System init AFTER boot
    wifiService.connect();

    // NTP before HTTPS OTA: cert validity checks need correct wall time.
    clockSyncBegin();

    char timeBuf[8];
    clockFormatHHMM(timeBuf, sizeof(timeBuf));
    oledService.showHomeScreen(timeBuf);
}

void App::loop() {
    if (otaUpdateInProgress()) {
        // Must yield: otherwise loop() busy-spins core 1, the idle task never runs, and the
        // task watchdog / WiFi stack can reset the chip during long HTTPS OTA downloads.
        delay(5);
        return;
    }

    static uint32_t lastHomeMs = 0;
    const uint32_t now = millis();
    if (now - lastHomeMs < 1000) {
        return;
    }
    lastHomeMs = now;

    char timeBuf[8];
    clockFormatHHMM(timeBuf, sizeof(timeBuf));
    oledService.showHomeScreen(timeBuf);

    static bool otaPeriodicStarted = false;
    if (!otaPeriodicStarted) {
        otaPeriodicStarted = true;
        startOtaPeriodicCheckTask();
    }
}