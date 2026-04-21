#include "ota_service.h"
#include "config/config.h"
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ctype.h>

#include "../../screen/OledService.h"

extern OledService oledService;

namespace
{

String normalizeRemoteVersion(String raw)
{
  raw.trim();
  if (raw.length() >= 3 && static_cast<uint8_t>(raw[0]) == 0xEF && static_cast<uint8_t>(raw[1]) == 0xBB &&
      static_cast<uint8_t>(raw[2]) == 0xBF) {
    raw.remove(0, 3);
    raw.trim();
  }
  const int nl = raw.indexOf('\n');
  if (nl >= 0) {
    raw = raw.substring(0, nl);
  }
  const int cr = raw.indexOf('\r');
  if (cr >= 0) {
    raw = raw.substring(0, cr);
  }
  raw.trim();
  return raw;
}

String buildVersionCheckUrl()
{
  String u = String(OTA_VERSION_URL);
  if (u.indexOf('?') >= 0) {
    u += "&ota_cb=";
  } else {
    u += "?ota_cb=";
  }
  u += String(millis());
  return u;
}

bool parseSemverCore(const String &s, int &outMajor, int &outMinor, int &outPatch)
{
  String t = s;
  t.trim();
  if (t.length() >= 1 && (t[0] == 'v' || t[0] == 'V')) {
    t.remove(0, 1);
  }
  const int p0 = t.indexOf('.');
  if (p0 <= 0) {
    return false;
  }
  const int p1 = t.indexOf('.', p0 + 1);
  if (p1 <= p0) {
    return false;
  }
  String pa = t.substring(p1 + 1);
  int end = 0;
  for (; end < pa.length(); end++) {
    if (!isdigit(static_cast<unsigned char>(pa[end]))) {
      break;
    }
  }
  pa = pa.substring(0, end);
  if (pa.length() == 0) {
    return false;
  }
  outMajor = t.substring(0, p0).toInt();
  outMinor = t.substring(p0 + 1, p1).toInt();
  outPatch = pa.toInt();
  return true;
}

// >0 remote newer, 0 same, <0 remote older (no OTA)
int compareRemoteVsLocalVersion(const String &remoteNorm, const String &localVer)
{
  int rMaj, rMin, rPat;
  int lMaj, lMin, lPat;
  const bool okR = parseSemverCore(remoteNorm, rMaj, rMin, rPat);
  const bool okL = parseSemverCore(localVer, lMaj, lMin, lPat);
  if (!okR || !okL) {
    if (remoteNorm == localVer) {
      return 0;
    }
    return 1;
  }
  if (rMaj != lMaj) {
    return rMaj - lMaj;
  }
  if (rMin != lMin) {
    return rMin - lMin;
  }
  return rPat - lPat;
}

// 0 = no update, 1 = update available, 2 = error / skip
int runVersionCheckOnce()
{
  if (WiFi.status() != WL_CONNECTED) {
    return 2;
  }
  if (OTA_VERSION_URL[0] == '\0') {
    return 2;
  }

  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.setTimeout(12000);
  http.setConnectTimeout(8000);
  http.setUserAgent("Esp32-OS/OTA (ESP32; version-check)");
  http.addHeader("Accept", "text/plain");
  http.addHeader("Cache-Control", "no-cache");

  const String versionUrl = buildVersionCheckUrl();
  WiFiClientSecure tlsClient;
  bool began;
  if (versionUrl.startsWith("https://")) {
    tlsClient.setInsecure();
    began = http.begin(tlsClient, versionUrl);
  } else {
    began = http.begin(versionUrl);
  }
  if (!began) {
    return 2;
  }

  const int code = http.GET();
  if (code != HTTP_CODE_OK) {
    http.end();
    return 2;
  }

  String contentType = http.header("Content-Type");
  if (contentType.length() > 0 && contentType.indexOf("text/plain") == -1) {
    if (contentType.indexOf("text/html") >= 0) {
      http.end();
      return 2;
    }
  }

  String v = normalizeRemoteVersion(http.getString());
  http.end();

  bool valid = true;
  for (int i = 0; i < v.length(); i++) {
    const char c = v[i];
    if (!(isdigit(static_cast<unsigned char>(c)) || c == '.' || c == 'v' || c == 'V')) {
      valid = false;
      break;
    }
  }

  if (!valid || v.length() < 5) {
    return 2;
  }

  if (v.startsWith("<") || v.indexOf("<!DOCTYPE") >= 0 || v.indexOf("<html") >= 0) {
    return 2;
  }

  const String localVer = String(FIRMWARE_VERSION);
  const int cmp = compareRemoteVsLocalVersion(v, localVer);
  if (cmp > 0) {
    return 1;
  }
  return 0;
}

bool downloadFirmwareWithProgress(const char *url)
{
  HTTPClient http;
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.setTimeout(60000);
  http.setConnectTimeout(15000);

  const String urlStr(url);
  WiFiClientSecure tlsClient;
  bool began;
  if (urlStr.startsWith("https://")) {
    tlsClient.setInsecure();
    began = http.begin(tlsClient, urlStr);
  } else {
    began = http.begin(urlStr);
  }
  if (!began) {
    oledService.clear();
    return false;
  }

  HTTPUpdate updater;
  updater.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  updater.rebootOnUpdate(true);

  updater.onStart([]() { oledService.showStatusProgress("Downloading...", 0); });

  updater.onProgress([](int cur, int total) {
    yield();
    if (total <= 0) {
      return;
    }
    int pct = static_cast<int>((static_cast<int64_t>(cur) * 100LL) / total);
    if (pct > 100) {
      pct = 100;
    }
    const char *msg = (pct < 88) ? "Downloading..." : "Updating...";
    oledService.showStatusProgress(msg, pct);
  });

  updater.onEnd([]() {
    oledService.showStatusProgress("Restarting...", 100);
    delay(500);
  });

  updater.onError([](int err) {
    (void)err;
    oledService.clear();
  });

  const t_httpUpdate_return ret = updater.update(http, String());

  http.end();

  if (ret == HTTP_UPDATE_OK) {
    return true;
  }

  if (ret == HTTP_UPDATE_NO_UPDATES) {
    oledService.clear();
    return false;
  }

  oledService.clear();
  return false;
}

}  // namespace

bool OtaService::isUpdateAvaiable()
{
  return runVersionCheckOnce() == 1;
}

void OtaService::performUpdate()
{
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  if (OTA_FIRMWARE_URL[0] == '\0') {
    return;
  }
  downloadFirmwareWithProgress(OTA_FIRMWARE_URL);
}

bool OtaService::tryUpdateFromUrl(const char *url)
{
  if (!url || url[0] == '\0') {
    return false;
  }
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  return downloadFirmwareWithProgress(url);
}

bool OtaService::tryConfiguredUpdate()
{
  return tryUpdateFromUrl(OTA_FIRMWARE_URL);
}
