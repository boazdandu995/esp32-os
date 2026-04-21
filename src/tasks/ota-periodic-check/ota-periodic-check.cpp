#include "ota-periodic-check.h"
#include "../../services/ota/ota_service.h"
#include "../../screen/OledService.h"
#include "../system/active_led.h"
#include <Arduino.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern OledService oledService;
extern OtaService otaService;

namespace
{

volatile bool sOtaUpdateInProgress = false;

void otaPeriodicTask(void *param)
{
  (void)param;
  const TickType_t interval = pdMS_TO_TICKS(300000);

  for (;;) {
    vTaskDelay(interval);

    if (WiFi.status() != WL_CONNECTED) {
      continue;
    }

    if (sOtaUpdateInProgress) {
      continue;
    }

    sOtaUpdateInProgress = true;
    const bool updateReady = otaService.isUpdateAvaiable();
    if (!updateReady) {
      sOtaUpdateInProgress = false;
      continue;
    }

    suspendActiveLedTask();

    oledService.showStatusProgress("Update found", 0);
    delay(600);

    otaService.performUpdate();

    sOtaUpdateInProgress = false;
    resumeActiveLedTask();
  }
}

}  // namespace

bool otaUpdateInProgress()
{
  return sOtaUpdateInProgress;
}

void startOtaPeriodicCheckTask()
{
  constexpr uint32_t kStack = 32768;
  constexpr UBaseType_t kPrio = 3;

  BaseType_t ok = xTaskCreatePinnedToCore(otaPeriodicTask, "ota_periodic", kStack, nullptr, kPrio, nullptr, 1);
  if (ok != pdPASS) {
    ok = xTaskCreatePinnedToCore(otaPeriodicTask, "ota_periodic", kStack, nullptr, kPrio, nullptr, 0);
  }
  if (ok != pdPASS) {
    ok = xTaskCreate(otaPeriodicTask, "ota_periodic", kStack, nullptr, kPrio, nullptr);
  }
  (void)ok;
}
