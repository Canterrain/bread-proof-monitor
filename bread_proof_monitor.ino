#include <Preferences.h>
#include <WebServer.h>
#include <Wire.h>

#include "app_state.h"
#include "notifications.h"
#include "proof_state.h"
#include "sensor_reader.h"
#include "web_dashboard.h"
#include "wifi_setup.h"

WebServer server(80);
Preferences prefs;
AppState appState;

namespace {
void logReadings(const AppState& state) {
  Serial.print("Distance: ");
  Serial.print(state.smoothedDistanceMm);
  Serial.print(" mm | Rise: ");
  Serial.print(displayRisePercent(state));
  Serial.print("% | Target: ");
  Serial.print(state.targetRisePercent);
  Serial.print("% | Profile: ");
  Serial.print(state.selectedRecipe);
  Serial.print(" | Stage: ");
  Serial.print(state.selectedStage);
  Serial.print(" | Step: ");
  Serial.print(currentStepTitle(state));
  Serial.print(" | State: ");
  Serial.print(proofStateApiValue(state.proofState));
  Serial.print(" | Status: ");
  Serial.print(statusText(state));
  Serial.print(" | Temp: ");
  Serial.print(state.temperatureF);
  Serial.print(" F | Humidity: ");
  Serial.print(state.humidityPercent);
  Serial.println(" %");
}
}  // namespace

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Starting Proof Monitor...");

  // Preserve the existing clean wiring exactly: GPIO22 = SDA, GPIO21 = SCL.
  Wire.begin(SDA_PIN, SCL_PIN);

  prefs.begin("proof", false);
  loadAppState(prefs, appState);
  ensureNtfyTopic(prefs, appState);

  beginSensors(appState);

  Serial.println(appState.loxReady ? "VL53L0X found" : "VL53L0X NOT found");
  Serial.println(appState.shtReady ? "SHT31 found" : "SHT31 NOT found");

  readSensors(appState);

  const bool wifiConnected = connectHomeWifi(appState);
  if (!wifiConnected) startSetupMode(appState);

  configureWebServer(server, prefs, appState);
  server.begin();

  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();

  if (appState.restartRequested && millis() >= appState.restartAt) {
    ESP.restart();
  }

  if (millis() - appState.lastSensorRead < 2000) return;

  appState.lastSensorRead = millis();

  const ProofState previousState = appState.proofState;
  const bool previousPendingEventActive = appState.pendingEventActive;
  const bool previousPendingEventNotified = appState.pendingEventNotified;
  readSensors(appState);

  if (appState.proofState != previousState ||
      appState.pendingEventActive != previousPendingEventActive ||
      appState.pendingEventNotified != previousPendingEventNotified) {
    saveAppState(prefs, appState);
    appState.lastProgressSaveMillis = millis();
  }

  checkTargetNotification(prefs, appState);
  maybeSaveProgress(prefs, appState);
  logReadings(appState);
}
