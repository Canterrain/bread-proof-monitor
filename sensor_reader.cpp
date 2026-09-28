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
  // Per this library's own vl53l0x_api_strings.cpp: 0 = Range Valid, and every other code is a
  // named failure (1 Sigma, 2 Signal, 3 Min Range, 4 Phase, 5 Hardware). Only 4 (out of range)
  // was excluded before, which let the other four failure modes' garbage data through as long
  // as it happened to fall in a plausible-looking distance range.
  if (measure.RangeStatus == 0) return measure.RangeMilliMeter;
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

// Shared by every distance read, live tick or one-shot capture, so the two paths can't drift
// apart in reliability again: a dough surface is a harder target for this sensor than an empty
// container wall (uneven, inconsistent reflectivity), and 5 samples with retries rejects a bad
// read far more reliably than a quick, no-retry 3-sample pass would.
constexpr uint8_t kSamplesNeeded = 5;
constexpr uint8_t kMaxAttempts = 10;
constexpr unsigned long kSampleDelayMs = 35;
// Five valid samples aren't necessarily five *agreeing* samples - a surface still moving can
// produce a "successful" median that doesn't reflect anything real. This is a gross-instability
// check, not a noise filter: at this rig's ~235mm standoff the sensor's own jitter measured up
// to ~11mm across a batch on a perfectly still surface, so anything tighter rejects good reads.
constexpr float kMaxStableSpreadMm = 15.0f;

bool readMedianDistanceMm(float& distanceMm) {
  float samples[kSamplesNeeded] = {};
  uint8_t count = 0;

  for (uint8_t attempt = 0; attempt < kMaxAttempts && count < kSamplesNeeded; ++attempt) {
    float sample = 0.0f;
    if (readValidDistanceSample(sample)) {
      samples[count++] = sample;
    }
    delay(kSampleDelayMs);
  }

  if (count < kSamplesNeeded) return false;

  float minSample = samples[0];
  float maxSample = samples[0];
  for (uint8_t i = 1; i < count; ++i) {
    if (samples[i] < minSample) minSample = samples[i];
    if (samples[i] > maxSample) maxSample = samples[i];
  }
  if (maxSample - minSample > kMaxStableSpreadMm) return false;

  distanceMm = medianDistance(samples, count);
  return true;
}
}  // namespace

void beginSensors(AppState& state) {
  // Long-range mode: at this rig's standoff (~130-240mm to the dough) the default profile
  // returned every single reading as RangeStatus 2 (Signal Fail, return too weak), which the
  // strict status check below correctly rejects. Long-range mode gets valid status-0 reads.
  state.loxReady = gLox.begin(VL53L0X_I2C_ADDR, false, &Wire, Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE);
  state.shtReady = gSht31.begin(0x44);
}

void readSensors(AppState& state) {
  if (shouldReadDistance(state)) {
    float distance = 0.0f;
    if (readMedianDistanceMm(distance)) {
      state.lastValidDistanceAtMillis = millis();
      state.lastRawDistanceMm = distance;
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
  if (!readMedianDistanceMm(distanceMm)) return false;

  state.smoothedDistanceMm = distanceMm;
  state.lastRawDistanceMm = distanceMm;
  state.lastValidDistanceAtMillis = millis();
  return true;
}
