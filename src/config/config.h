#pragma once

#define WIFI_SSID "Dandu"
#define WIFI_PASS "7702426322"

#define FIRMWARE_VERSION "1.1.9"

// ---- OTA ----
// version.txt: plain text, one line (e.g. 1.0.5). Use raw.githubusercontent.com/.../version.txt not /blob/...
// URLs may be http:// or https://. HTTPS uses Arduino HTTPClient (TLS encrypted; no custom CA verification).
#define OTA_VERSION_URL "https://raw.githubusercontent.com/boazdandu995/esp32-os/main/version.txt"
#define OTA_FIRMWARE_URL "https://github.com/boazdandu995/esp32-os/releases/download/ota-check/firmware.bin"

// ---- NTP (home screen clock) ----
#define NTP_SERVER_1 "pool.ntp.org"
#define NTP_SERVER_2 "time.nist.gov"
#define NTP_GMT_OFFSET_SEC 0
#define NTP_DAYLIGHT_OFFSET_SEC 0