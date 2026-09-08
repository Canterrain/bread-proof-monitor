#include "notifications.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "proof_state.h"

namespace {
String buildSimpleTargetTitle(const AppState& state) {
  if (state.selectedStage == "Bulk") return "Ready to Shape";
  return "Ready to Bake";
}

String buildSimpleTargetMessage(const AppState& state) {
  if (state.selectedStage == "Bulk") {
    return String("Shape your dough now. ") + state.selectedRecipe + " has reached its bulk target.";
  }

  return String("Bake your dough now. ") + state.selectedRecipe + " has reached its final proof target.";
}
}  // namespace

String generateNtfyTopic() {
  const char chars[] = "ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz23456789";
  String topic = "proofmonitor-";

  for (int i = 0; i < 16; ++i) {
    topic += chars[esp_random() % (sizeof(chars) - 1)];
  }

  return topic;
}

void ensureNtfyTopic(Preferences& prefs, AppState& state) {
  if (state.ntfyTopic.length() > 0) return;

  state.ntfyTopic = generateNtfyTopic();
  prefs.putString("ntfy", state.ntfyTopic);
}

bool sendNtfyNotification(const AppState& state, const String& title, const String& body) {
  if (state.ntfyTopic.length() < 10 || WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  const String url = String(NTFY_BASE_URL) + state.ntfyTopic;

  http.begin(client, url);
  http.addHeader("Content-Type", "text/plain");
  http.addHeader("Title", title.length() > 0 ? title : "Proof Monitor");
  http.addHeader("Tags", "bread");

  const int code = http.POST(body);
  http.end();

  Serial.print("ntfy response code: ");
  Serial.println(code);

  return code >= 200 && code < 300;
}

bool sendNtfyNotification(const AppState& state, const String& body) {
  return sendNtfyNotification(state, "Proof Monitor", body);
}

void checkTargetNotification(Preferences& prefs, AppState& state) {
  if (state.proofState == ProofState::Paused || state.proofState == ProofState::Finished) return;
  if (!proofHasEmptyCalibration(state) || !proofHasStartingHeight(state)) return;

  const ActiveFermentationEventStatus eventStatus = currentFermentationEvent(state);
  if (eventStatus.exists && eventStatus.due) {
    if (state.pendingEventNotified) return;

    const String body = String(eventStatus.event.notificationBody) + " Pause Monitoring before removing the lid.";
    if (sendNtfyNotification(state, eventStatus.event.notificationTitle, body)) {
      state.pendingEventNotified = true;
      saveAppState(prefs, state);
      state.lastProgressSaveMillis = millis();
    }
    return;
  }

  if (state.targetNotified) return;
  if (state.proofState != ProofState::TargetReached) return;

  if (sendNtfyNotification(state, buildSimpleTargetTitle(state), buildSimpleTargetMessage(state))) {
    state.targetNotified = true;
    saveAppState(prefs, state);
    state.lastProgressSaveMillis = millis();
  }
}
