#include "app_state.h"

#include <math.h>

#include "proof_state.h"
#include "recipe_presets.h"

namespace {
constexpr char kProofStateKey[] = "proofState";
constexpr char kRecipeReadyKey[] = "recipeReady";
constexpr char kElapsedSecondsKey[] = "elapsedSec";
constexpr char kSegmentElapsedKey[] = "segElapsed";
constexpr char kAwaitingFinalKey[] = "awaitFinal";
constexpr char kPendingEventKey[] = "eventIdx";
constexpr char kPendingEventActiveKey[] = "eventDue";
constexpr char kCompletedEventsKey[] = "eventDone";
constexpr char kPendingEventNotifiedKey[] = "eventSent";
constexpr char kSegmentBaselineKey[] = "segBase";
constexpr char kCompletedOffsetKey[] = "progOffset";
constexpr char kPeakProgressKey[] = "peakProg";
constexpr char kOutcomePendingKey[] = "outcomePend";
constexpr char kOutcomeRecipeKey[] = "outcomeRecipe";
constexpr char kOutcomeStageKey[] = "outcomeStage";
constexpr char kOutcomePendingNextKey[] = "outcomePendNxt";
constexpr char kOutcomeRecipeNextKey[] = "outcomeRecNxt";
constexpr char kOutcomeStageNextKey[] = "outcomeStgNxt";

bool inferRecipeConfigured(const AppState& state) {
  return state.selectedRecipe != "Custom" ||
         state.selectedStage != "Bulk" ||
         fabsf(state.targetRisePercent - 75.0f) > 0.05f;
}

unsigned long persistedElapsedSeconds(const AppState& state) {
  switch (state.proofState) {
    case ProofState::Running:
    case ProofState::Paused:
    case ProofState::TargetReached:
      return currentElapsedSeconds(state);
    case ProofState::Finished:
      return state.finalElapsedSeconds;
    case ProofState::NotStarted:
    default:
      return 0;
  }
}

unsigned long persistedSegmentElapsedSeconds(const AppState& state) {
  switch (state.proofState) {
    case ProofState::Running:
    case ProofState::Paused:
    case ProofState::TargetReached:
      return currentSegmentElapsedSeconds(state);
    case ProofState::Finished:
    default:
      return 0;
  }
}

void loadLegacyState(Preferences& prefs, AppState& state) {
  state.emptyDistanceMm = prefs.getFloat("empty", 0.0f);
  state.startDistanceMm = prefs.getFloat("start", 0.0f);
  state.targetRisePercent = prefs.getFloat("target", 75.0f);
  state.targetNotified = prefs.getBool("notified", false);
  state.finalRisePercent = prefs.getFloat("finalRise", 0.0f);

  state.selectedRecipe = prefs.getString("recipe", "Custom");
  state.selectedStage = prefs.getString("stage", "Bulk");
  state.wifiSsid = prefs.getString("ssid", "");
  state.wifiPassword = prefs.getString("pass", "");
  state.ntfyTopic = prefs.getString("ntfy", "");
  state.selectedRecipe = normalizeProfileName(state.selectedRecipe);
  state.awaitingFinalProofStart = false;
  state.pendingEventIndex = -1;
  state.pendingEventActive = false;
  state.completedEventCount = 0;
  state.pendingEventNotified = false;
  state.segmentBaselineDistanceMm = 0.0f;
  state.completedProgressOffsetPercent = 0.0f;
  state.peakProgressPercent = 0.0f;
  state.outcomeFeedbackPending = false;
  state.outcomeRecipe = "";
  state.outcomeStage = "";
  state.outcomeFeedbackPendingNext = false;
  state.outcomeRecipeNext = "";
  state.outcomeStageNext = "";

  const bool legacyPaused = prefs.getBool("paused", false);
  const bool legacyFinished = prefs.getBool("finished", false);

  if (legacyFinished) {
    state.proofState = ProofState::Finished;
  } else if (legacyPaused) {
    state.proofState = ProofState::Paused;
  } else if (state.startDistanceMm > 0.0f) {
    state.proofState = state.targetNotified ? ProofState::TargetReached : ProofState::Running;
  } else {
    state.proofState = ProofState::NotStarted;
  }

  const unsigned long legacyStarted = prefs.getULong("started", 0);
  const unsigned long legacyEnded = prefs.getULong("ended", 0);
  const unsigned long legacyPausedTotal = prefs.getULong("pausedTotal", 0);

  if (legacyFinished && legacyStarted > 0 && legacyEnded > (legacyStarted + legacyPausedTotal)) {
    state.finalElapsedSeconds = (legacyEnded - legacyStarted - legacyPausedTotal) / 1000;
  } else {
    state.finalElapsedSeconds = 0;
  }

  state.elapsedOffsetSeconds = state.proofState == ProofState::Finished ? state.finalElapsedSeconds : 0;
  state.proofStartedAtMillis = (state.proofState == ProofState::Running || state.proofState == ProofState::TargetReached)
                                   ? millis()
                                   : 0;
  state.segmentElapsedOffsetSeconds = 0;
  state.segmentStartedAtMillis = (state.proofState == ProofState::Running || state.proofState == ProofState::TargetReached)
                                      ? millis()
                                      : 0;
  state.recipeConfigured = prefs.getBool(kRecipeReadyKey, inferRecipeConfigured(state) || state.startDistanceMm > 0.0f);
}
}  // namespace

ProofState parseProofState(const String& value) {
  if (value == "running") return ProofState::Running;
  if (value == "paused") return ProofState::Paused;
  if (value == "target_reached") return ProofState::TargetReached;
  if (value == "finished") return ProofState::Finished;
  return ProofState::NotStarted;
}

String proofStateStorageValue(ProofState state) {
  switch (state) {
    case ProofState::Running:
      return "running";
    case ProofState::Paused:
      return "paused";
    case ProofState::TargetReached:
      return "target_reached";
    case ProofState::Finished:
      return "finished";
    case ProofState::NotStarted:
    default:
      return "not_started";
  }
}

void loadAppState(Preferences& prefs, AppState& state) {
  const String savedProofState = prefs.getString(kProofStateKey, "");

  if (savedProofState.length() == 0) {
    loadLegacyState(prefs, state);
    state.lastProgressSaveMillis = millis();
    return;
  }

  state.emptyDistanceMm = prefs.getFloat("empty", 0.0f);
  state.startDistanceMm = prefs.getFloat("start", 0.0f);
  state.targetRisePercent = prefs.getFloat("target", 75.0f);
  state.targetNotified = prefs.getBool("notified", false);
  state.finalRisePercent = prefs.getFloat("finalRise", 0.0f);

  state.selectedRecipe = prefs.getString("recipe", "Custom");
  state.selectedStage = prefs.getString("stage", "Bulk");
  state.selectedRecipe = normalizeProfileName(state.selectedRecipe);
  state.recipeConfigured = prefs.getBool(kRecipeReadyKey, inferRecipeConfigured(state));
  state.awaitingFinalProofStart = prefs.getBool(kAwaitingFinalKey, false);
  state.pendingEventIndex = static_cast<int8_t>(prefs.getInt(kPendingEventKey, -1));
  state.pendingEventActive = prefs.getBool(kPendingEventActiveKey, false);
  state.completedEventCount = static_cast<uint8_t>(prefs.getUInt(kCompletedEventsKey, 0));
  state.pendingEventNotified = prefs.getBool(kPendingEventNotifiedKey, false);
  state.segmentBaselineDistanceMm = prefs.getFloat(kSegmentBaselineKey, 0.0f);
  state.completedProgressOffsetPercent = prefs.getFloat(kCompletedOffsetKey, 0.0f);
  state.peakProgressPercent = prefs.getFloat(kPeakProgressKey, 0.0f);
  state.outcomeFeedbackPending = prefs.getBool(kOutcomePendingKey, false);
  state.outcomeRecipe = prefs.getString(kOutcomeRecipeKey, "");
  state.outcomeStage = prefs.getString(kOutcomeStageKey, "");
  state.outcomeFeedbackPendingNext = prefs.getBool(kOutcomePendingNextKey, false);
  state.outcomeRecipeNext = prefs.getString(kOutcomeRecipeNextKey, "");
  state.outcomeStageNext = prefs.getString(kOutcomeStageNextKey, "");

  state.wifiSsid = prefs.getString("ssid", "");
  state.wifiPassword = prefs.getString("pass", "");
  state.ntfyTopic = prefs.getString("ntfy", "");

  state.proofState = parseProofState(savedProofState);

  const unsigned long storedElapsedSeconds = prefs.getULong(kElapsedSecondsKey, 0);
  const unsigned long storedSegmentElapsedSeconds = prefs.getULong(kSegmentElapsedKey, 0);
  state.proofStartedAtMillis = 0;
  state.elapsedOffsetSeconds = 0;
  state.finalElapsedSeconds = 0;
  state.segmentStartedAtMillis = 0;
  state.segmentElapsedOffsetSeconds = 0;

  switch (state.proofState) {
    case ProofState::Running:
    case ProofState::TargetReached:
      state.elapsedOffsetSeconds = storedElapsedSeconds;
      state.proofStartedAtMillis = millis();
      state.segmentElapsedOffsetSeconds = storedSegmentElapsedSeconds;
      state.segmentStartedAtMillis = millis();
      break;
    case ProofState::Paused:
      state.elapsedOffsetSeconds = storedElapsedSeconds;
      state.segmentElapsedOffsetSeconds = storedSegmentElapsedSeconds;
      break;
    case ProofState::Finished:
      state.finalElapsedSeconds = storedElapsedSeconds;
      break;
    case ProofState::NotStarted:
    default:
      break;
  }

  state.lastProgressSaveMillis = millis();
}

void saveAppState(Preferences& prefs, const AppState& state) {
  prefs.putFloat("empty", state.emptyDistanceMm);
  prefs.putFloat("start", state.startDistanceMm);
  prefs.putFloat("target", state.targetRisePercent);
  prefs.putString(kProofStateKey, proofStateStorageValue(state.proofState));
  prefs.putULong(kElapsedSecondsKey, persistedElapsedSeconds(state));
  prefs.putULong(kSegmentElapsedKey, persistedSegmentElapsedSeconds(state));
  prefs.putBool("notified", state.targetNotified);
  prefs.putFloat("finalRise", state.finalRisePercent);
  prefs.putString("recipe", state.selectedRecipe);
  prefs.putString("stage", state.selectedStage);
  prefs.putBool(kRecipeReadyKey, state.recipeConfigured);
  prefs.putBool(kAwaitingFinalKey, state.awaitingFinalProofStart);
  prefs.putInt(kPendingEventKey, state.pendingEventIndex);
  prefs.putBool(kPendingEventActiveKey, state.pendingEventActive);
  prefs.putUInt(kCompletedEventsKey, state.completedEventCount);
  prefs.putBool(kPendingEventNotifiedKey, state.pendingEventNotified);
  prefs.putFloat(kSegmentBaselineKey, state.segmentBaselineDistanceMm);
  prefs.putFloat(kCompletedOffsetKey, state.completedProgressOffsetPercent);
  prefs.putFloat(kPeakProgressKey, state.peakProgressPercent);
  prefs.putBool(kOutcomePendingKey, state.outcomeFeedbackPending);
  prefs.putString(kOutcomeRecipeKey, state.outcomeRecipe);
  prefs.putString(kOutcomeStageKey, state.outcomeStage);
  prefs.putBool(kOutcomePendingNextKey, state.outcomeFeedbackPendingNext);
  prefs.putString(kOutcomeRecipeNextKey, state.outcomeRecipeNext);
  prefs.putString(kOutcomeStageNextKey, state.outcomeStageNext);
}

void maybeSaveProgress(Preferences& prefs, AppState& state, unsigned long intervalMs) {
  const bool shouldPersist = state.proofState == ProofState::Running ||
                             state.proofState == ProofState::Paused ||
                             state.proofState == ProofState::TargetReached;

  if (!shouldPersist) return;
  if (millis() - state.lastProgressSaveMillis < intervalMs) return;

  saveAppState(prefs, state);
  state.lastProgressSaveMillis = millis();
}

void scheduleRestart(AppState& state, unsigned long delayMs) {
  state.restartRequested = true;
  state.restartAt = millis() + delayMs;
}
