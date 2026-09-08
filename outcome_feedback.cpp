#include "outcome_feedback.h"

#include <Preferences.h>

namespace {
constexpr int kNudgeStepPercent = 5;
constexpr int kMaxDriftPercent = 25;
constexpr int kMinTargetPercent = 5;
constexpr int kMaxTargetPercent = 200;

Preferences gOutcomePrefs;
bool gOutcomeStoreReady = false;

void ensureOutcomeStore() {
  if (gOutcomeStoreReady) return;
  gOutcomePrefs.begin("outcomes", false);
  gOutcomeStoreReady = true;
}

// Short, deterministic, collision-resistant-enough key derived from (recipeName, stage) so
// learned adjustments stay tied to a recipe's name rather than its position in the preset table.
String hashKey(const String& recipeName, const String& stage) {
  uint32_t hash = 2166136261u;
  const String combined = recipeName + "|" + stage;

  for (size_t i = 0; i < combined.length(); ++i) {
    hash ^= static_cast<uint8_t>(combined[i]);
    hash *= 16777619u;
  }

  char buf[9];
  snprintf(buf, sizeof(buf), "%08lx", static_cast<unsigned long>(hash));
  return String(buf);
}

int clampInt(int value, int minValue, int maxValue) {
  if (value < minValue) return minValue;
  if (value > maxValue) return maxValue;
  return value;
}
}  // namespace

float adjustedTarget(const String& recipeName, const String& stage, uint8_t shippedDefault) {
  ensureOutcomeStore();

  const String deltaKey = "ov" + hashKey(recipeName, stage);
  const int8_t delta = gOutcomePrefs.getChar(deltaKey.c_str(), 0);
  const int adjusted = clampInt(static_cast<int>(shippedDefault) + delta, kMinTargetPercent, kMaxTargetPercent);
  return static_cast<float>(adjusted);
}

void recordOutcome(const String& recipeName, const String& stage, uint8_t shippedDefault, OutcomeRating rating) {
  ensureOutcomeStore();

  const String key = hashKey(recipeName, stage);
  const String streakKey = "st" + key;
  const String deltaKey = "ov" + key;

  if (rating == OutcomeRating::JustRight) {
    gOutcomePrefs.putChar(streakKey.c_str(), 0);
    return;
  }

  int8_t streak = gOutcomePrefs.getChar(streakKey.c_str(), 0);
  const int8_t direction = (rating == OutcomeRating::Underproofed) ? 1 : -1;

  if ((streak > 0 && direction < 0) || (streak < 0 && direction > 0)) {
    streak = 0;
  }
  streak += direction;

  if (abs(streak) >= 2) {
    const int8_t delta = gOutcomePrefs.getChar(deltaKey.c_str(), 0);
    const int newDelta = clampInt(static_cast<int>(delta) + (direction * kNudgeStepPercent),
                                   -kMaxDriftPercent, kMaxDriftPercent);
    gOutcomePrefs.putChar(deltaKey.c_str(), static_cast<int8_t>(newDelta));
    streak = 0;
  }

  gOutcomePrefs.putChar(streakKey.c_str(), streak);
}

void resetLearnedTargets() {
  ensureOutcomeStore();
  gOutcomePrefs.clear();
}
