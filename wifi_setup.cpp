#include "wifi_setup.h"

#include <ESPmDNS.h>
#include <WiFi.h>

#include "text_utils.h"

bool connectHomeWifi(AppState& state) {
  if (state.wifiSsid.length() == 0) return false;

  state.setupMode = false;

  WiFi.mode(WIFI_STA);
  WiFi.begin(state.wifiSsid.c_str(), state.wifiPassword.c_str());

  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(state.wifiSsid);

  const unsigned long startAttempt = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 20000) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi connection failed.");
    return false;
  }

  Serial.println("Connected to home Wi-Fi.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin(MDNS_NAME)) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("Open: http://proofmonitor.local");
  }

  return true;
}

void startSetupMode(AppState& state) {
  state.setupMode = true;

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(SETUP_AP_NAME, SETUP_AP_PASSWORD);

  Serial.println("Setup mode started.");
  Serial.print("Wi-Fi network: ");
  Serial.println(SETUP_AP_NAME);
  Serial.print("Password: ");
  Serial.println(SETUP_AP_PASSWORD);
  Serial.println("Open: http://192.168.4.1");
}

String wifiOptionsHtml(const AppState& state) {
  String options;
  const int networkCount = WiFi.scanNetworks();

  if (networkCount <= 0) {
    options += "<option value=\"\">No networks found</option>";
  } else {
    options += "<option value=\"\">Choose a Wi-Fi network</option>";

    for (int index = 0; index < networkCount; ++index) {
      const String ssid = WiFi.SSID(index);
      const String safeSsid = htmlEscape(ssid);

      options += "<option value=\"";
      options += safeSsid;
      options += "\"";

      if (ssid == state.wifiSsid) options += " selected";

      options += ">";
      options += safeSsid;
      options += " (";
      options += String(WiFi.RSSI(index));
      options += " dBm)</option>";
    }
  }

  WiFi.scanDelete();
  return options;
}

void saveWifiConfig(Preferences& prefs, AppState& state, const String& ssid, const String& password) {
  if (ssid.length() > 0) {
    state.wifiSsid = ssid;
    prefs.putString("ssid", state.wifiSsid);
  }

  if (password.length() > 0) {
    state.wifiPassword = password;
    prefs.putString("pass", state.wifiPassword);
  }
}

void resetWifiConfig(Preferences& prefs, AppState& state) {
  prefs.remove("ssid");
  prefs.remove("pass");
  state.wifiSsid = "";
  state.wifiPassword = "";
}
