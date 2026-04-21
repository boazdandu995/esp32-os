#pragma once

#include <stddef.h>

void clockSyncBegin();

// Writes local time as HH:MM into buf. If time not set yet, writes "--:--".
void clockFormatHHMM(char *buf, size_t len);
