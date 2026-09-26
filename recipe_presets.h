#pragma once

#include <Arduino.h>
#include <stddef.h>

constexpr uint8_t kMaxProfileEvents = 4;

struct FermentationEvent {
  const char* stage;
  const char* type;
  const char* title;
  const char* instruction;
  uint8_t triggerRisePercent;
  const char* notificationTitle;
  const char* notificationBody;
};

struct RecipePreset {
  const char* category;
  const char* name;
  uint8_t bulk;
  uint8_t final;
  bool hasFinal;
  uint8_t eventCount;
  FermentationEvent events[kMaxProfileEvents];
  const char* sourceUrl = nullptr;
  // Shown when this profile's proof finishes without a monitored Final stage, for recipes
  // whose real final proof is untracked (usually because it's judged by time, not height).
  const char* untrackedFinalNote = nullptr;
  // Shown at "Set Starting Dough Height" for the Bulk stage, for recipes with a real premix
  // step (autolyse, mixing in salt/yeast, kneading) that must happen before monitoring starts.
  const char* premixNote = nullptr;
  uint16_t referenceFloorMinutes = 20;
};

const RecipePreset* allVisibleRecipePresets(size_t& count);
const RecipePreset& calibrationTestPreset();
String normalizeProfileName(const String& name);
bool findRecipePreset(const String& name, RecipePreset& preset);
String proofProfilesJson();
bool profileRequiresStageSetupReset(const String& name);
// True for starter-leavened recipes, where published bulk-fermentation guidance ties the right
// rise target to dough temperature. Commercial-yeast recipes are excluded even when their bulk
// stage runs warm, since their own instructions target a fixed rise (e.g. "until doubled") and
// expect only the *time* to get there to vary with temperature, not the target itself.
bool recipeUsesTemperatureScaledTarget(const String& name);
