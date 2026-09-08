#include "sensor_reader.h"

#include <Adafruit_SHT31.h>
#include <Adafruit_VL53L0X.h>

#include "proof_state.h"

namespace {
Adafruit_VL53L0X gLox;
Adafruit_SHT31 gSht31 = Adafruit_SHT31();
constexpr unsigned long kDistanceStaleAfterMs = 15000;

float readDistanceMm() {
  VL53L0X_RangingMeasurementData_t measure;
  gLox.rangingTest(&measure, false);
  if (measure.RangeStatus != 4) return measure.RangeMilliMeter;
  return -1.0f;
}

bool shouldReadDistance(const AppState& state) {
  return state.loxReady &&
         state.proofState != ProofState::Paused &&
         state.proofState != ProofState::Finished &&
         millis() > state.resumeIgnoreUntil;
}

bool readValidDistanceSample(float& distanceMm) {
  const float distance = readDistanceMm();
  if (distance > 10.0f && distance < 2000.0f) {
    distanceMm = distance;
    return true;
  }

  return false;
}

float medianDistance(float* values, uint8_t count) {
  for (uint8_t index = 1; index < count; ++index) {
    const float value = values[index];
    int8_t insertIndex = index - 1;
    while (insertIndex >= 0 && values[insertIndex] > value) {
      values[insertIndex + 1] = values[insertIndex];
      --insertIndex;
    }
    values[insertIndex + 1] = value;
  }

  return values[count / 2];
}
}  // namespace

void beginSensors(AppState& state) {
  state.loxReady = gLox.begin();
  state.shtReady = gSht31.begin(0x44);
}

void readSensors(AppState& state) {
  if (shouldReadDistance(state)) {
    // Take a few raw reads and use the median before smoothing. At this sensor's typical
    // standoff distance (well over a foot, long range for a VL53L0X) individual reads are
    // noisier and lower-confidence, so a single bad sample can still slip past the loose
    // RangeStatus filter below. Requiring at least two of three reads to agree rejects a
    // lone outlier outright, instead of just partially blending it into the average.
    constexpr uint8_t kSamplesPerTick = 3;
    float samples[kSamplesPerTick] = {};
    uint8_t count = 0;
    for (uint8_t i = 0; i < kSamplesPerTick; ++i) {
      float sample = 0.0f;
      if (readValidDistanceSample(sample)) {
        samples[count++] = sample;
      }
    }

    if (count > 0) {
      const float distance = medianDistance(samples, count);
      state.lastValidDistanceAtMillis = millis();
      if (state.smoothedDistanceMm == 0.0f) {
        state.smoothedDistanceMm = distance;
      } else {
        state.smoothedDistanceMm = (state.smoothedDistanceMm * 0.85f) + (distance * 0.15f);
      }
    }
  }

  if (state.shtReady) {
    const float tempC = gSht31.readTemperature();
    const float humidity = gSht31.readHumidity();

    if (!isnan(tempC)) state.temperatureF = (tempC * 9.0f / 5.0f) + 32.0f;
    if (!isnan(humidity)) state.humidityPercent = humidity;
  }

  updateProofStateFromRise(state);
}

bool captureStableDistanceMm(AppState& state, float& distanceMm) {
  if (!state.loxReady) return false;

  constexpr uint8_t kSamplesNeeded = 5;
  constexpr uint8_t kMaxAttempts = 10;
  float samples[kSamplesNeeded] = {};
  uint8_t count = 0;

  for (uint8_t attempt = 0; attempt < kMaxAttempts && count < kSamplesNeeded; ++attempt) {
    float sample = 0.0f;
    if (readValidDistanceSample(sample)) {
      samples[count++] = sample;
    }
    delay(35);
  }

  if (count < kSamplesNeeded) return false;

  distanceMm = medianDistance(samples, count);
  state.smoothedDistanceMm = distanceMm;
  state.lastValidDistanceAtMillis = millis();
  return true;
}
