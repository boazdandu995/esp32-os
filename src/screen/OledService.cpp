#include "OledService.h"

#include "bootscreen/BootScreen.h"
#include "checkupdatescreen/CheckUpdateScreen.h"
#include "homescreen/HomeScreen.h"
#include "messagescreen/MessageScreen.h"
#include "statusscreen/StatusScreen.h"

namespace
{
constexpr TickType_t kDrawLockWait = pdMS_TO_TICKS(4000);
}  // namespace

bool OledService::takeDrawLock()
{
  if (!drawMutex_) {
    return true;
  }
  return xSemaphoreTake(drawMutex_, kDrawLockWait) == pdTRUE;
}

void OledService::giveDrawLock()
{
  if (drawMutex_) {
    xSemaphoreGive(drawMutex_);
  }
}

void OledService::init()
{
  if (!drawMutex_) {
    drawMutex_ = xSemaphoreCreateMutex();
  }
  driver_.init();
}

void OledService::showBootTitle(String version)
{
  if (!takeDrawLock()) {
    return;
  }
  BootScreen::show(driver_, version);
  giveDrawLock();
}

void OledService::showStatusProgress(String status, int percent)
{
  if (!takeDrawLock()) {
    return;
  }
  StatusScreen::show(driver_, status, percent);
  giveDrawLock();
}

void OledService::showCheckingForUpdates(uint8_t dotPhase)
{
  if (!takeDrawLock()) {
    return;
  }
  CheckUpdateScreen::show(driver_, dotPhase);
  giveDrawLock();
}

void OledService::showCenteredMessage(const char *text)
{
  if (!takeDrawLock()) {
    return;
  }
  MessageScreen::showCentered(driver_, text);
  giveDrawLock();
}

void OledService::clear()
{
  if (!takeDrawLock()) {
    return;
  }
  MessageScreen::clear(driver_);
  giveDrawLock();
}

void OledService::showHomeScreen(const char *timeHHMM)
{
  if (!takeDrawLock()) {
    return;
  }
  HomeScreen::draw(driver_, timeHHMM);
  giveDrawLock();
}
