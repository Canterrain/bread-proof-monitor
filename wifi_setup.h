#pragma once

#include <Arduino.h>
#include <Preferences.h>

#include "app_state.h"

bool connectHomeWifi(AppState& state);
void startSetupMode(AppState& state);
String wifiOptionsHtml(const AppState& state);
void saveWifiConfig(Preferences& prefs, AppState& state, const String& ssid, const String& password);
void resetWifiConfig(Preferences& prefs, AppState& state);
