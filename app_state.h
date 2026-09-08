#pragma once

#include <Arduino.h>
#include <Preferences.h>

constexpr uint8_t SDA_PIN = 22;
constexpr uint8_t SCL_PIN = 21;

constexpr char SETUP_AP_NAME[] = "ProofMonitor-Setup";
constexpr char SETUP_AP_PASSWORD[] = "sourdough";
constexpr char MDNS_NAME[] = "proofmonitor";
constexpr char NTFY_BASE_URL[] = "https://ntfy.sh/";

enum class ProofState : uint8_t {
  NotStarted,
  Running,
  Paused,
  TargetReached,
  Finished
};

struct AppState {
  bool loxReady = false;
  bool shtReady = false;
  bool setupMode = false;
  bool targetNotified = false;
  bool recipeConfigured = false;
  bool awaitingFinalProofStart = false;
  bool pendingEventActive = false;
  bool pendingEventNotified = false;
  bool outcomeFeedbackPending = false;
  // A second outcome can already be waiting when the first hasn't been rated yet - e.g. Bulk
  // finishes (queuing a Bulk rating) and then Final finishes too before the Bulk rating was
  // answered. Held here and promoted into the fields above once the current one is rated,
  // rather than overwriting it and losing that feedback.
  bool outcomeFeedbackPendingNext = false;

  String wifiSsid = "";
  String wifiPassword = "";
  String ntfyTopic = "";
  String selectedRecipe = "Custom";
  String selectedStage = "Bulk";
  String outcomeRecipe = "";
  String outcomeStage = "";
  String outcomeRecipeNext = "";
  String outcomeStageNext = "";

  int8_t pendingEventIndex = -1;
  uint8_t completedEventCount = 0;

  float emptyDistanceMm = 0.0f;
  float startDistanceMm = 0.0f;
  float segmentBaselineDistanceMm = 0.0f;
  float completedProgressOffsetPercent = 0.0f;
  float peakProgressPercent = 0.0f;
  float targetRisePercent = 75.0f;
  float smoothedDistanceMm = 0.0f;
  float temperatureF = 0.0f;
  float humidityPercent = 0.0f;
  float finalRisePercent = 0.0f;

  ProofState proofState = ProofState::NotStarted;

  unsigned long proofStartedAtMillis = 0;
  unsigned long elapsedOffsetSeconds = 0;
  unsigned long finalElapsedSeconds = 0;
  unsigned long segmentStartedAtMillis = 0;
  unsigned long segmentElapsedOffsetSeconds = 0;
  unsigned long resumeIgnoreUntil = 0;
  unsigned long peakCandidateSinceMillis = 0;
  unsigned long lastSensorRead = 0;
  unsigned long lastValidDistanceAtMillis = 0;
  unsigned long lastProgressSaveMillis = 0;

  bool restartRequested = false;
  unsigned long restartAt = 0;
};

ProofState parseProofState(const String& value);
String proofStateStorageValue(ProofState state);

void loadAppState(Preferences& prefs, AppState& state);
void saveAppState(Preferences& prefs, const AppState& state);
void maybeSaveProgress(Preferences& prefs, AppState& state, unsigned long intervalMs = 60000);
void scheduleRestart(AppState& state, unsigned long delayMs = 1000);
