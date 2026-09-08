#pragma once

#include "app_state.h"

void beginSensors(AppState& state);
void readSensors(AppState& state);
bool captureStableDistanceMm(AppState& state, float& distanceMm);
