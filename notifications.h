#pragma once

#include <Arduino.h>
#include <Preferences.h>

#include "app_state.h"

String generateNtfyTopic();
void ensureNtfyTopic(Preferences& prefs, AppState& state);
bool sendNtfyNotification(const AppState& state, const String& title, const String& body);
bool sendNtfyNotification(const AppState& state, const String& body);
void checkTargetNotification(Preferences& prefs, AppState& state);
