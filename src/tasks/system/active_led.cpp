#include "active_led.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define LED_PIN 2

namespace
{

TaskHandle_t sLedTaskHandle = nullptr;

void activeLedTask(void *param)
{
  (void)param;
  pinMode(LED_PIN, OUTPUT);

  for (;;) {
    // Turn ON (blink)
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(200));  // 200 ms ON

    // Turn OFF
    digitalWrite(LED_PIN, LOW);

    // Wait 20 seconds before next blink
    vTaskDelay(pdMS_TO_TICKS(20000));
  }
}

}  // namespace

void startLedTask()
{
  xTaskCreate(activeLedTask, "ActiveLedTask", 2048, nullptr, 1, &sLedTaskHandle);
}

void suspendActiveLedTask()
{
  if (sLedTaskHandle != nullptr) {
    vTaskSuspend(sLedTaskHandle);
  }
}

void resumeActiveLedTask()
{
  if (sLedTaskHandle != nullptr) {
    vTaskResume(sLedTaskHandle);
  }
}
