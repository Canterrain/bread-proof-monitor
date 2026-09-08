#pragma once

#include <Arduino.h>

enum class OutcomeRating : uint8_t { Underproofed, JustRight, Overproofed };

// Returns the target rise percent to use for (recipeName, stage): the shipped preset value,
// nudged by whatever this device has locally learned from past Rate Outcome feedback.
float adjustedTarget(const String& recipeName, const String& stage, uint8_t shippedDefault);

// Records a baker's outcome rating for the most recently finished proof. Two consecutive
// same-direction ratings for the same (recipeName, stage) nudge that pair's learned target;
// a single rating just builds the streak. Nudges are clamped to +/-25 points from shippedDefault.
void recordOutcome(const String& recipeName, const String& stage, uint8_t shippedDefault, OutcomeRating rating);

// Clears all locally learned target adjustments, reverting every profile/stage to its shipped default.
void resetLearnedTargets();
