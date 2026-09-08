#pragma once

#include <Preferences.h>
#include <WebServer.h>

#include "app_state.h"

void configureWebServer(WebServer& server, Preferences& prefs, AppState& state);
