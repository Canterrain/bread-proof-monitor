#pragma once

#include <Arduino.h>

#include "app_state.h"
#include "recipe_presets.h"

struct ActiveFermentationEventStatus {
  bool exists = false;
  bool due = false;
  bool notified = false;
  int8_t index = -1;
  FermentationEvent event = {};
};

bool proofHasEmptyCalibration(const AppState& state);
bool proofHasStartingHeight(const AppState& state);
bool proofDistanceReadingStale(const AppState& state);
bool proofCanSetEmpty(const AppState& state);
bool proofCanSetStart(const AppState& state);
bool proofCanPause(const AppState& state);
bool proofCanResume(const AppState& state);
bool proofCanFinish(const AppState& state);
bool proofCanCompleteEvent(const AppState& state);
bool proofCanRebaseline(const AppState& state);
// True exactly when currentStepInstruction() is about to tell the user to go mix or shape
// dough per the linked recipe (empty setup done, starting height not yet set, not finished,
// reading not stale) - used to surface a real Open Recipe link alongside that instruction.
bool proofAwaitingDoughPrep(const AppState& state);

float currentDoughHeightMm(const AppState& state);
float startingDoughHeightMm(const AppState& state);
float liveRisePercent(const AppState& state);
float overallProgressPercent(const AppState& state);
float displayRisePercent(const AppState& state);

unsigned long currentElapsedSeconds(const AppState& state);
unsigned long currentSegmentElapsedSeconds(const AppState& state);
String elapsedText(const AppState& state);

String proofStateApiValue(ProofState state);
String statusText(const AppState& state);
String nextStepText(const AppState& state);
String phoneAlertText(const AppState& state);
String currentStepTitle(const AppState& state);
String currentStepInstruction(const AppState& state);
String upcomingStepText(const AppState& state);
ActiveFermentationEventStatus currentFermentationEvent(const AppState& state);
void resetFermentationEventState(AppState& state);
void completeCurrentEvent(AppState& state);
void rebaselineProof(AppState& state);

void startProof(AppState& state);
void pauseProof(AppState& state);
void resumeProof(AppState& state);
void finishProof(AppState& state);
// Queues an outcome rating for (recipe, stage). If nothing is currently pending, it becomes
// the pending one immediately; otherwise it waits in the "Next" slot until the current one is
// rated (see handleRateOutcome()), so a Bulk rating queued when Final auto-starts isn't lost.
void queuePendingOutcome(AppState& state, const String& recipe, const String& stage);
// After the current pending outcome has been handled (rated or skipped), promotes the queued
// "Next" one (if any) into its place, or clears the slot entirely if there isn't one.
void advanceOutcomeQueue(AppState& state);
// Unconditionally discards the pending outcome and any queued "Next" one - used when a whole
// new proof begins, since a rating for a run you can no longer clearly remember isn't useful
// feedback, and the recipe/stage it would have nudged gets another chance from the new run.
void clearAllPendingOutcomes(AppState& state);
void startNewProof(AppState& state);
void updateProofStateFromRise(AppState& state);
void clearActiveProofRunForStage(AppState& state, bool clearEmptyCalibration);
bool stageChangeNeedsEmptyReset(const String& previousProfile,
                                const String& previousStage,
                                const String& nextProfile,
                                const String& nextStage);
