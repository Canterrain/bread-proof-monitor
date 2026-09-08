#include "proof_state.h"

#include <math.h>

#include "outcome_feedback.h"
#include "recipe_presets.h"

namespace {
constexpr int8_t kNoEventIndex = -1;
constexpr unsigned long kDistanceStaleAfterMs = 15000;
// A single noisy sample shouldn't permanently raise the floor: require the reading to stay
// above the current peak for a few consecutive ticks before it's trusted. This is short
// enough to accept genuine rise (which persists for many minutes, not seconds) but long
// enough to reject a one-off VL53L0X jitter spike, which reverts within a tick or two.
constexpr unsigned long kPeakConfirmMs = 8000;
constexpr unsigned long kDefaultReferenceFloorSeconds = 20 * 60;
constexpr unsigned long kMinSegmentFloorSeconds = 8 * 60;
constexpr unsigned long kMaxSegmentFloorSeconds = 40 * 60;
constexpr float kReferenceTempF = 75.0f;
constexpr float kFermentationDoublingSpanF = 18.0f;

bool riseInputsReady(const AppState& state) {
  return proofHasEmptyCalibration(state) && proofHasStartingHeight(state) && state.smoothedDistanceMm > 0.0f;
}

bool loadSelectedPreset(const AppState& state, RecipePreset& preset) {
  return findRecipePreset(state.selectedRecipe, preset);
}

// The minimum time a segment must run before a fold/target crossing is trusted, scaled by
// measured temperature: warmer dough can legitimately move faster, colder dough should move
// slower, so an implausibly-fast reading is more suspicious in a cold kitchen, not less.
unsigned long requiredSegmentFloorSeconds(const AppState& state) {
  RecipePreset preset;
  const unsigned long referenceSeconds = loadSelectedPreset(state, preset)
                                              ? static_cast<unsigned long>(preset.referenceFloorMinutes) * 60UL
                                              : kDefaultReferenceFloorSeconds;

  if (!state.shtReady) return referenceSeconds;

  const float scale = powf(2.0f, (state.temperatureF - kReferenceTempF) / kFermentationDoublingSpanF);
  if (scale <= 0.0f) return referenceSeconds;

  const unsigned long scaledSeconds = static_cast<unsigned long>(referenceSeconds / scale);
  if (scaledSeconds < kMinSegmentFloorSeconds) return kMinSegmentFloorSeconds;
  if (scaledSeconds > kMaxSegmentFloorSeconds) return kMaxSegmentFloorSeconds;
  return scaledSeconds;
}

bool stageMatches(const FermentationEvent& event, const String& stage) {
  return event.stage != nullptr && stage == event.stage;
}

int8_t firstEventIndexForStage(const RecipePreset& preset, const String& stage) {
  for (uint8_t index = 0; index < preset.eventCount; ++index) {
    if (stageMatches(preset.events[index], stage)) return static_cast<int8_t>(index);
  }

  return kNoEventIndex;
}

int8_t nextEventIndexForStage(const RecipePreset& preset, const String& stage, int8_t currentIndex) {
  for (uint8_t index = static_cast<uint8_t>(currentIndex + 1); index < preset.eventCount; ++index) {
    if (stageMatches(preset.events[index], stage)) return static_cast<int8_t>(index);
  }

  return kNoEventIndex;
}

int8_t eventIndexFromCompletedCount(const RecipePreset& preset, const String& stage, uint8_t completedCount) {
  uint8_t seen = 0;

  for (uint8_t index = 0; index < preset.eventCount; ++index) {
    if (!stageMatches(preset.events[index], stage)) continue;
    if (seen == completedCount) return static_cast<int8_t>(index);
    ++seen;
  }

  return kNoEventIndex;
}

float doughHeightFromDistanceMm(const AppState& state, float distanceMm) {
  if (!proofHasEmptyCalibration(state) || distanceMm <= 0.0f) return 0.0f;
  return state.emptyDistanceMm - distanceMm;
}

float risePercentFromDistances(const AppState& state, float baselineDistanceMm, float currentDistanceMm) {
  const float baselineHeight = doughHeightFromDistanceMm(state, baselineDistanceMm);
  const float currentHeight = doughHeightFromDistanceMm(state, currentDistanceMm);

  if (baselineHeight <= 0.0f || currentHeight <= 0.0f) return 0.0f;

  const float risePercent = ((currentHeight - baselineHeight) / baselineHeight) * 100.0f;
  return risePercent > 0.0f ? risePercent : 0.0f;
}

float rawOverallProgressPercent(const AppState& state) {
  if (!proofHasStartingHeight(state)) return 0.0f;

  const float baselineDistanceMm = state.segmentBaselineDistanceMm > 0.0f
                                       ? state.segmentBaselineDistanceMm
                                       : state.startDistanceMm;
  const float segmentRise = risePercentFromDistances(state, baselineDistanceMm, state.smoothedDistanceMm);
  return state.completedProgressOffsetPercent + segmentRise;
}

int8_t resolvedPendingEventIndex(const AppState& state, const RecipePreset& preset) {
  if (state.pendingEventIndex >= 0 &&
      state.pendingEventIndex < static_cast<int8_t>(preset.eventCount) &&
      stageMatches(preset.events[state.pendingEventIndex], state.selectedStage)) {
    return state.pendingEventIndex;
  }

  return eventIndexFromCompletedCount(preset, state.selectedStage, state.completedEventCount);
}

void primeEventStateForSelection(AppState& state, const RecipePreset* preset = nullptr) {
  state.pendingEventActive = false;
  state.completedEventCount = 0;
  state.pendingEventNotified = false;
  state.segmentBaselineDistanceMm = 0.0f;
  state.completedProgressOffsetPercent = 0.0f;
  state.peakProgressPercent = 0.0f;
  state.peakCandidateSinceMillis = 0;
  state.pendingEventIndex = kNoEventIndex;
  state.segmentStartedAtMillis = 0;
  state.segmentElapsedOffsetSeconds = 0;

  if (preset != nullptr) {
    state.pendingEventIndex = firstEventIndexForStage(*preset, state.selectedStage);
  }
}

bool canPrepareFinalProof(const AppState& state, RecipePreset& preset) {
  return state.recipeConfigured &&
         state.selectedStage == "Bulk" &&
         findRecipePreset(state.selectedRecipe, preset) &&
         preset.hasFinal;
}

void clearActiveProofRun(AppState& state) {
  state.startDistanceMm = 0.0f;
  state.segmentBaselineDistanceMm = 0.0f;
  state.completedProgressOffsetPercent = 0.0f;
  state.peakProgressPercent = 0.0f;
  state.peakCandidateSinceMillis = 0;
  state.proofStartedAtMillis = 0;
  state.elapsedOffsetSeconds = 0;
  state.finalElapsedSeconds = 0;
  state.finalRisePercent = 0.0f;
  state.resumeIgnoreUntil = 0;
  state.targetNotified = false;
  state.pendingEventActive = false;
  state.pendingEventNotified = false;
  state.proofState = ProofState::NotStarted;
}

String nextMilestoneText(const AppState& state) {
  const ActiveFermentationEventStatus eventStatus = currentFermentationEvent(state);
  if (eventStatus.exists && !eventStatus.due) {
    return String("Next: ") + eventStatus.event.title + " at " + String(eventStatus.event.triggerRisePercent) + "%";
  }

  if (!state.recipeConfigured) return "Apply a proof profile target, then set empty setup and starting dough height.";
  if (state.selectedStage == "Bulk") {
    return String("Next: Ready to Shape at ") + String(state.targetRisePercent, 0) + "%";
  }

  return String("Next: Ready to Bake at ") + String(state.targetRisePercent, 0) + "%";
}
}  // namespace

bool proofHasEmptyCalibration(const AppState& state) {
  return state.emptyDistanceMm > 0.0f;
}

bool proofHasStartingHeight(const AppState& state) {
  return state.startDistanceMm > 0.0f;
}

bool proofDistanceReadingStale(const AppState& state) {
  if (!state.loxReady) return true;
  if (state.smoothedDistanceMm <= 0.0f) return true;
  if (state.lastValidDistanceAtMillis == 0) return true;
  return millis() - state.lastValidDistanceAtMillis > kDistanceStaleAfterMs;
}

bool proofCanSetEmpty(const AppState& state) {
  return state.proofState != ProofState::Paused && state.proofState != ProofState::Finished;
}

bool proofCanSetStart(const AppState& state) {
  return proofHasEmptyCalibration(state) &&
         state.proofState != ProofState::Paused &&
         state.proofState != ProofState::Finished;
}

bool proofCanPause(const AppState& state) {
  return state.proofState == ProofState::Running || state.proofState == ProofState::TargetReached;
}

bool proofCanResume(const AppState& state) {
  return state.proofState == ProofState::Paused;
}

bool proofCanFinish(const AppState& state) {
  return proofHasStartingHeight(state) && state.proofState != ProofState::Finished;
}

bool proofCanCompleteEvent(const AppState& state) {
  const ActiveFermentationEventStatus eventStatus = currentFermentationEvent(state);
  return eventStatus.exists &&
         eventStatus.due &&
         state.proofState != ProofState::Paused &&
         state.proofState != ProofState::Finished &&
         !proofDistanceReadingStale(state);
}

bool proofCanRebaseline(const AppState& state) {
  return proofHasStartingHeight(state) &&
         state.proofState != ProofState::Paused &&
         state.proofState != ProofState::Finished &&
         !proofDistanceReadingStale(state);
}

float currentDoughHeightMm(const AppState& state) {
  if (!proofHasEmptyCalibration(state) || state.smoothedDistanceMm <= 0.0f) return 0.0f;
  return state.emptyDistanceMm - state.smoothedDistanceMm;
}

float startingDoughHeightMm(const AppState& state) {
  if (!proofHasEmptyCalibration(state) || !proofHasStartingHeight(state)) return 0.0f;
  return state.emptyDistanceMm - state.startDistanceMm;
}

float liveRisePercent(const AppState& state) {
  const float startHeight = startingDoughHeightMm(state);
  const float currentHeight = currentDoughHeightMm(state);

  if (startHeight <= 0.0f || currentHeight <= 0.0f) return 0.0f;
  return ((currentHeight - startHeight) / startHeight) * 100.0f;
}

float overallProgressPercent(const AppState& state) {
  const float rawProgress = rawOverallProgressPercent(state);
  return rawProgress > state.peakProgressPercent ? rawProgress : state.peakProgressPercent;
}

float displayRisePercent(const AppState& state) {
  if (state.proofState == ProofState::Finished) return state.finalRisePercent;
  return overallProgressPercent(state);
}

unsigned long currentElapsedSeconds(const AppState& state) {
  switch (state.proofState) {
    case ProofState::Running:
    case ProofState::TargetReached:
      if (state.proofStartedAtMillis == 0) return state.elapsedOffsetSeconds;
      return state.elapsedOffsetSeconds + ((millis() - state.proofStartedAtMillis) / 1000);
    case ProofState::Paused:
      return state.elapsedOffsetSeconds;
    case ProofState::Finished:
      return state.finalElapsedSeconds;
    case ProofState::NotStarted:
    default:
      return 0;
  }
}

unsigned long currentSegmentElapsedSeconds(const AppState& state) {
  switch (state.proofState) {
    case ProofState::Running:
    case ProofState::TargetReached:
      if (state.segmentStartedAtMillis == 0) return state.segmentElapsedOffsetSeconds;
      return state.segmentElapsedOffsetSeconds + ((millis() - state.segmentStartedAtMillis) / 1000);
    case ProofState::Paused:
      return state.segmentElapsedOffsetSeconds;
    case ProofState::Finished:
    case ProofState::NotStarted:
    default:
      return 0;
  }
}

String elapsedText(const AppState& state) {
  if (!proofHasStartingHeight(state) && state.proofState == ProofState::NotStarted) return "Not started";

  const unsigned long elapsedSeconds = currentElapsedSeconds(state);
  const int hours = elapsedSeconds / 3600;
  const int minutes = (elapsedSeconds % 3600) / 60;
  return String(hours) + "h " + String(minutes) + "m";
}

String proofStateApiValue(ProofState state) {
  switch (state) {
    case ProofState::Running:
      return "running";
    case ProofState::Paused:
      return "paused";
    case ProofState::TargetReached:
      return "target-reached";
    case ProofState::Finished:
      return "finished";
    case ProofState::NotStarted:
    default:
      return "not-started";
  }
}

String statusText(const AppState& state) {
  if (state.proofState == ProofState::Finished) return "Proof finished";
  if (state.proofState == ProofState::Paused) return "Paused";

  if (!state.loxReady && !state.shtReady) return "Sensors not found";
  if (!state.loxReady) return "Distance sensor not found";
  if (!state.shtReady) return "Temperature sensor not found";
  if (proofDistanceReadingStale(state)) {
    return state.pendingEventActive ? "Settling after fold" : "Distance reading unstable";
  }
  if (!state.recipeConfigured) return "Choose proof profile";
  if (state.awaitingFinalProofStart) return "Bulk proof complete";
  if (!proofHasEmptyCalibration(state)) return "Set empty setup";
  if (!proofHasStartingHeight(state)) return "Set starting dough height";
  const ActiveFermentationEventStatus eventStatus = currentFermentationEvent(state);
  if (eventStatus.exists && eventStatus.due) return eventStatus.event.title;
  if (state.proofState == ProofState::TargetReached) return "Target reached";

  const float rise = overallProgressPercent(state);
  if (rise >= state.targetRisePercent) return "Confirming target reached";
  if (rise < 10.0f) return "Just getting started";
  if (rise < state.targetRisePercent * 0.75f) return "Rising";
  return "Getting close";
}

String nextStepText(const AppState& state) {
  if (state.proofState != ProofState::TargetReached) return "";

  return "Check for slight jiggle, slow springback from a gentle poke, and visible bubbles along the side. If it looks right, finish this proof. If it needs more time, resume monitoring.";
}

String phoneAlertText(const AppState& state) {
  if (state.pendingEventActive) {
    return state.pendingEventNotified ? "Alert sent" : "Alert ready";
  }

  return state.targetNotified ? "Alert sent" : "Alert ready";
}

String currentStepTitle(const AppState& state) {
  if (!state.recipeConfigured) return "Choose proof profile";
  if (state.awaitingFinalProofStart) return "Bulk proof complete";
  if (!proofHasEmptyCalibration(state)) return "Set empty setup";
  if (!proofHasStartingHeight(state)) return "Set starting dough height";
  if (state.proofState == ProofState::Paused) return "Monitoring paused";
  const ActiveFermentationEventStatus eventStatus = currentFermentationEvent(state);
  if (eventStatus.exists && eventStatus.due) return eventStatus.event.title;
  if (state.proofState == ProofState::TargetReached) return "Target reached";
  if (state.proofState == ProofState::Finished) return "Proof finished";
  return "Monitor proof";
}

String currentStepInstruction(const AppState& state) {
  if (!state.recipeConfigured) {
    return "Choose a Proof Profile Target or adjust a custom target before you start monitoring.";
  }

  // Finished stops sensor reads entirely (like Paused), so a stale reading at this point is
  // permanent, not transient. Show the actual finished-proof guidance instead of getting
  // stuck forever on the generic staleness message below.
  if (state.proofState == ProofState::Finished) {
    RecipePreset preset;
    if (loadSelectedPreset(state, preset) && preset.untrackedFinalNote != nullptr) {
      return String(preset.untrackedFinalNote);
    }
    return "This proof is complete. Start a new proof when you are ready to monitor another batch.";
  }

  if (proofDistanceReadingStale(state)) {
    if (state.pendingEventActive) {
      return "Give the reading a few seconds to settle now that the lid is back on. Complete Step "
             "will become available once it does, no action needed.";
    }
    return "Distance readings look stale or unstable right now. Check sensor position and lid placement, then wait for live readings to settle.";
  }

  if (!proofHasEmptyCalibration(state)) {
    if (state.awaitingFinalProofStart || state.selectedStage == "Final") {
      return "Place the empty final-proof setup under the sensor, then press Set Empty Setup.";
    }
    return "Place the empty proof setup under the sensor, then press Set Empty Setup.";
  }

  if (state.awaitingFinalProofStart) {
    return "Bulk proof complete. Shape your dough, place the monitor over the shaped dough, then press Set Starting Dough Height to begin final proof.";
  }

  if (!proofHasStartingHeight(state)) {
    if (state.selectedStage == "Final") {
      return "Place the monitor over the shaped dough, let it settle for a moment, then "
             "press Set Starting Dough Height.";
    }
    RecipePreset preset;
    if (loadSelectedPreset(state, preset) && preset.premixNote != nullptr) {
      return String(preset.premixNote) +
             " Then, for bulk rise, place the dough in the proof container, smooth the "
             "top, let the lid settle for a moment, and press Set Starting Dough Height.";
    }
    return "Place the dough in the proof container, smooth the top, let the lid settle "
           "for a moment, then press Set Starting Dough Height.";
  }

  if (state.proofState == ProofState::Paused) {
    return "Resume monitoring when the dough is back in place and you want the proof timer to continue.";
  }

  const ActiveFermentationEventStatus eventStatus = currentFermentationEvent(state);
  if (eventStatus.exists && eventStatus.due) {
    return "Pause Monitoring before you remove the lid. " + String(eventStatus.event.instruction) +
           " Resume Monitoring once the lid is back on. Complete Step won't be "
           "available until you do.";
  }

  if (state.proofState == ProofState::TargetReached) {
    return nextStepText(state);
  }

  if (overallProgressPercent(state) >= state.targetRisePercent) {
    return "The reading has hit your target already, but the app waits a bit longer at "
           "this temperature before trusting it, in case it was a brief spike rather than "
           "real rise. No action needed, this resolves on its own shortly.";
  }

  return "Keep monitoring until the dough reaches the selected target rise.";
}

String upcomingStepText(const AppState& state) {
  if (!state.recipeConfigured) return nextMilestoneText(state);
  if (state.proofState == ProofState::Finished) return "";
  if (proofDistanceReadingStale(state)) {
    return state.pendingEventActive ? "Wait a few seconds, then press Complete Step."
                                     : "Wait for stable live distance readings.";
  }
  if (!proofHasEmptyCalibration(state)) return "Set starting dough height.";
  if (state.awaitingFinalProofStart) return String("Next: Ready to Bake at ") + String(state.targetRisePercent, 0) + "%";
  if (!proofHasStartingHeight(state)) return nextMilestoneText(state);
  const ActiveFermentationEventStatus eventStatus = currentFermentationEvent(state);
  if (eventStatus.exists && eventStatus.due) {
    RecipePreset preset;
    if (loadSelectedPreset(state, preset)) {
      const int8_t nextIndex = nextEventIndexForStage(preset, state.selectedStage, eventStatus.index);
      if (nextIndex >= 0) {
        return String("Next: ") + preset.events[nextIndex].title + " at " + String(preset.events[nextIndex].triggerRisePercent) + "%";
      }
    }
    return state.selectedStage == "Bulk"
               ? String("Next: Ready to Shape at ") + String(state.targetRisePercent, 0) + "%"
               : String("Next: Ready to Bake at ") + String(state.targetRisePercent, 0) + "%";
  }
  if (state.proofState == ProofState::TargetReached) return "Finish Proof when the dough looks ready.";
  return nextMilestoneText(state);
}

ActiveFermentationEventStatus currentFermentationEvent(const AppState& state) {
  ActiveFermentationEventStatus status;
  RecipePreset preset;
  if (!loadSelectedPreset(state, preset)) return status;

  const int8_t eventIndex = resolvedPendingEventIndex(state, preset);
  if (eventIndex < 0) return status;

  status.exists = true;
  status.index = eventIndex;
  status.notified = state.pendingEventNotified;
  status.event = preset.events[eventIndex];
  // Use the debounced peak here, not overallProgressPercent() (which is max(raw, peak)) -
  // a fold must go through the same 8-second confirm window as a target-reached decision
  // does, or a single noisy sample can mark it due and fire a phone alert with no confirmation
  // at all, even though the peak itself never actually moved.
  const bool crossedTrigger = proofHasEmptyCalibration(state) &&
                              proofHasStartingHeight(state) &&
                              !state.awaitingFinalProofStart &&
                              state.proofState != ProofState::Finished &&
                              currentSegmentElapsedSeconds(state) >= requiredSegmentFloorSeconds(state) &&
                              state.peakProgressPercent >= status.event.triggerRisePercent;
  status.due = state.pendingEventActive || crossedTrigger;
  return status;
}

void clearActiveProofRunForStage(AppState& state, bool clearEmptyCalibration) {
  clearActiveProofRun(state);
  if (clearEmptyCalibration) {
    state.emptyDistanceMm = 0.0f;
  }
}

bool stageChangeNeedsEmptyReset(const String& previousProfile,
                                const String& previousStage,
                                const String& nextProfile,
                                const String& nextStage) {
  // A different recipe should never inherit a leftover run's state, even if both happen to
  // default to the same stage name (e.g. "Bulk" to "Bulk") - that's a different container
  // and a different bake, not a continuation of the old one.
  if (previousProfile != nextProfile) return true;
  if (previousStage == nextStage) return false;
  return profileRequiresStageSetupReset(previousProfile) || profileRequiresStageSetupReset(nextProfile);
}

void resetFermentationEventState(AppState& state) {
  RecipePreset preset;
  if (loadSelectedPreset(state, preset)) {
    primeEventStateForSelection(state, &preset);
  } else {
    primeEventStateForSelection(state, nullptr);
  }
}

void completeCurrentEvent(AppState& state) {
  const ActiveFermentationEventStatus eventStatus = currentFermentationEvent(state);
  if (!eventStatus.exists || !eventStatus.due || state.smoothedDistanceMm <= 0.0f) return;

  RecipePreset preset;
  if (!loadSelectedPreset(state, preset)) return;

  // Use the already-latched peak, not a fresh recompute: captureStableDistanceMm() has
  // already overwritten smoothedDistanceMm with the post-fold reading, so comparing it
  // against the still-stale pre-fold segmentBaselineDistanceMm here would mix
  // measurements from before and after the physical disturbance.
  state.completedProgressOffsetPercent = state.peakProgressPercent;
  state.segmentBaselineDistanceMm = state.smoothedDistanceMm;
  state.peakCandidateSinceMillis = 0;
  state.pendingEventActive = false;
  state.completedEventCount += 1;
  state.pendingEventNotified = false;
  state.pendingEventIndex = nextEventIndexForStage(preset, state.selectedStage, eventStatus.index);
  state.targetNotified = false;
  state.segmentStartedAtMillis = millis();
  state.segmentElapsedOffsetSeconds = 0;
  updateProofStateFromRise(state);
}

void rebaselineProof(AppState& state) {
  if (!proofCanRebaseline(state) || state.smoothedDistanceMm <= 0.0f) return;

  // Same reasoning as completeCurrentEvent(): use the already-latched peak rather than
  // recomputing against the freshly-captured (post-disturbance) distance.
  state.completedProgressOffsetPercent = state.peakProgressPercent;
  state.segmentBaselineDistanceMm = state.smoothedDistanceMm;
  state.peakCandidateSinceMillis = 0;
  state.segmentStartedAtMillis = millis();
  state.segmentElapsedOffsetSeconds = 0;
  updateProofStateFromRise(state);
}

void startProof(AppState& state) {
  RecipePreset preset;
  const RecipePreset* presetPtr = loadSelectedPreset(state, preset) ? &preset : nullptr;
  primeEventStateForSelection(state, presetPtr);
  state.startDistanceMm = state.smoothedDistanceMm;
  state.segmentBaselineDistanceMm = state.smoothedDistanceMm;
  state.peakProgressPercent = 0.0f;
  state.proofStartedAtMillis = millis();
  state.elapsedOffsetSeconds = 0;
  state.finalElapsedSeconds = 0;
  state.finalRisePercent = 0.0f;
  state.targetNotified = false;
  state.awaitingFinalProofStart = false;
  state.segmentStartedAtMillis = millis();
  state.segmentElapsedOffsetSeconds = 0;
  state.proofState = ProofState::Running;
}

void pauseProof(AppState& state) {
  if (!proofCanPause(state)) return;

  state.elapsedOffsetSeconds = currentElapsedSeconds(state);
  state.proofStartedAtMillis = 0;
  state.segmentElapsedOffsetSeconds = currentSegmentElapsedSeconds(state);
  state.segmentStartedAtMillis = 0;
  state.proofState = ProofState::Paused;
}

void resumeProof(AppState& state) {
  if (!proofCanResume(state)) return;

  state.proofStartedAtMillis = millis();
  state.segmentStartedAtMillis = millis();
  state.resumeIgnoreUntil = millis() + 6000;
  state.proofState = ProofState::Running;
  updateProofStateFromRise(state);
}

void queuePendingOutcome(AppState& state, const String& recipe, const String& stage) {
  if (!state.outcomeFeedbackPending) {
    state.outcomeFeedbackPending = true;
    state.outcomeRecipe = recipe;
    state.outcomeStage = stage;
    return;
  }

  // Something's already waiting to be rated (e.g. a Bulk rating from earlier in this same
  // proof) - hold this one rather than clobbering it, and promote it once the current one is
  // answered. See handleRateOutcome() in web_dashboard.cpp.
  state.outcomeFeedbackPendingNext = true;
  state.outcomeRecipeNext = recipe;
  state.outcomeStageNext = stage;
}

void finishProof(AppState& state) {
  if (!proofCanFinish(state)) return;

  state.finalElapsedSeconds = currentElapsedSeconds(state);
  state.proofStartedAtMillis = 0;
  state.elapsedOffsetSeconds = state.finalElapsedSeconds;
  state.finalRisePercent = overallProgressPercent(state);
  state.awaitingFinalProofStart = false;
  state.proofState = ProofState::Finished;

  RecipePreset preset;
  if (!canPrepareFinalProof(state, preset)) {
    queuePendingOutcome(state, state.selectedRecipe, state.selectedStage);
    return;
  }

  // Bulk is done and we're about to auto-start Final - queue a rating for the Bulk stage now,
  // rather than only ever asking about whichever stage finishes last. Otherwise Bulk targets
  // for two-stage recipes could never learn from feedback at all.
  queuePendingOutcome(state, state.selectedRecipe, state.selectedStage);

  state.selectedStage = "Final";
  state.targetRisePercent = adjustedTarget(preset.name, "Final", preset.final);
  clearActiveProofRunForStage(state, profileRequiresStageSetupReset(state.selectedRecipe));
  primeEventStateForSelection(state, &preset);
  state.awaitingFinalProofStart = true;
}

void startNewProof(AppState& state) {
  clearActiveProofRunForStage(state, false);
  resetFermentationEventState(state);
  state.awaitingFinalProofStart = false;
}

void updateProofStateFromRise(AppState& state) {
  if (state.proofState != ProofState::Running && state.proofState != ProofState::TargetReached) return;
  if (!riseInputsReady(state)) return;
  if (proofDistanceReadingStale(state)) return;
  const float rawProgress = rawOverallProgressPercent(state);
  if (rawProgress > state.peakProgressPercent) {
    if (state.peakCandidateSinceMillis == 0) {
      state.peakCandidateSinceMillis = millis();
    } else if (millis() - state.peakCandidateSinceMillis >= kPeakConfirmMs) {
      state.peakProgressPercent = rawProgress;
    }
  } else {
    state.peakCandidateSinceMillis = 0;
  }
  const ActiveFermentationEventStatus eventStatus = currentFermentationEvent(state);
  if (eventStatus.exists && eventStatus.due) {
    state.pendingEventActive = true;
    state.proofState = ProofState::Running;
    return;
  }

  state.pendingEventActive = false;
  if (currentSegmentElapsedSeconds(state) >= requiredSegmentFloorSeconds(state) &&
      state.peakProgressPercent >= state.targetRisePercent) {
    state.proofState = ProofState::TargetReached;
    return;
  }

  state.proofState = ProofState::Running;
}
