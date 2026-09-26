#include "web_dashboard.h"

#include "notifications.h"
#include "outcome_feedback.h"
#include "proof_state.h"
#include "recipe_presets.h"
#include "sensor_reader.h"
#include "text_utils.h"
#include "web_assets.h"
#include "wifi_setup.h"

namespace {
WebServer* gServer = nullptr;
Preferences* gPrefs = nullptr;
AppState* gState = nullptr;

const char* jsonBool(bool value) {
  return value ? "true" : "false";
}

void persistState() {
  saveAppState(*gPrefs, *gState);
  gState->lastProgressSaveMillis = millis();
}

void reconcileProofStateWithTarget() {
  updateProofStateFromRise(*gState);
}

String liveDataJson(const AppState& state) {
  const bool canSetDistance = state.smoothedDistanceMm > 0.0f;

  String json = "{";
  json += "\"rise\":" + String(displayRisePercent(state), 0) + ",";
  json += "\"status\":\"" + jsonEscape(statusText(state)) + "\",";
  json += "\"proofState\":\"" + proofStateApiValue(state.proofState) + "\",";
  json += "\"profile\":\"" + jsonEscape(state.selectedRecipe) + "\",";
  json += "\"stage\":\"" + jsonEscape(state.selectedStage) + "\",";
  json += "\"target\":" + String(state.targetRisePercent, 0) + ",";
  json += "\"distance\":" + String(state.smoothedDistanceMm, 1) + ",";
  json += "\"currentHeight\":" + String(currentDoughHeightMm(state), 1) + ",";
  json += "\"startingHeight\":" + String(startingDoughHeightMm(state), 1) + ",";
  json += "\"temperature\":" + String(state.temperatureF, 1) + ",";
  json += "\"humidity\":" + String(state.humidityPercent, 1) + ",";
  json += "\"elapsed\":\"" + jsonEscape(elapsedText(state)) + "\",";
  json += "\"phoneAlert\":\"" + phoneAlertText(state) + "\",";
  json += "\"currentStep\":\"" + jsonEscape(currentStepTitle(state)) + "\",";
  json += "\"currentInstruction\":\"" + jsonEscape(currentStepInstruction(state)) + "\",";
  json += "\"upcomingStep\":\"" + jsonEscape(upcomingStepText(state)) + "\",";
  json += "\"nextStep\":\"" + jsonEscape(nextStepText(state)) + "\",";
  json += "\"showProfilePicker\":" + String(jsonBool(!state.recipeConfigured)) + ",";
  json += "\"needsDoughPrepLink\":" + String(jsonBool(proofAwaitingDoughPrep(state))) + ",";
  json += "\"canCompleteStep\":" + String(jsonBool(proofCanCompleteEvent(state))) + ",";
  json += "\"hasEmptySetup\":" + String(jsonBool(proofHasEmptyCalibration(state))) + ",";
  json += "\"distanceReadingStale\":" + String(jsonBool(proofDistanceReadingStale(state))) + ",";
  json += "\"canSetEmpty\":" + String(jsonBool(proofCanSetEmpty(state) && canSetDistance)) + ",";
  json += "\"canSetStart\":" + String(jsonBool(proofCanSetStart(state) && canSetDistance)) + ",";
  json += "\"canPause\":" + String(jsonBool(proofCanPause(state))) + ",";
  json += "\"canResume\":" + String(jsonBool(proofCanResume(state))) + ",";
  json += "\"canFinish\":" + String(jsonBool(proofCanFinish(state))) + ",";
  json += "\"canRebaseline\":" + String(jsonBool(proofCanRebaseline(state) && canSetDistance)) + ",";
  json += "\"outcomeFeedbackPending\":" + String(jsonBool(state.outcomeFeedbackPending)) + ",";
  json += "\"outcomeRecipe\":\"" + jsonEscape(state.outcomeRecipe) + "\",";
  json += "\"outcomeStage\":\"" + jsonEscape(state.outcomeStage) + "\",";
  // Diagnostic fields, not used by the UI: let an external check reconstruct and verify
  // overallProgressPercent()/rawOverallProgressPercent() by hand, even after a fold has
  // shifted the segment baseline and the simple "current vs starting height" math no
  // longer applies.
  json += "\"segmentBaselineDistanceMm\":" + String(state.segmentBaselineDistanceMm, 1) + ",";
  json += "\"completedProgressOffsetPercent\":" + String(state.completedProgressOffsetPercent, 1) + ",";
  json += "\"peakProgressPercent\":" + String(state.peakProgressPercent, 1);
  json += "}";

  return json;
}

String dashboardPage(const AppState& state) {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Proof Monitor</title>
<style>)rawliteral";

  html += kDashboardCss;

  html += R"rawliteral(</style>
</head>
<body>
<div class="shell">
  <div class="panel">
    <div class="hero">
      <div class="eyebrow">Current Rise</div>
      <h1>Proof Monitor</h1>
      <div class="rise-row">
        <div class="rise-value" id="riseValue">)rawliteral";

  html += String(displayRisePercent(state), 0);

  html += R"rawliteral(%</div>
        <div class="status-pill" id="statusValue">)rawliteral";

  html += htmlEscape(statusText(state));

  html += R"rawliteral(</div>
      </div>
      <div class="hero-meta">
        <div class="meta-chip">
          <span class="meta-label">Elapsed</span>
          <span class="meta-value" id="elapsedValue">)rawliteral";

  html += htmlEscape(elapsedText(state));

  html += R"rawliteral(</span>
        </div>
        <div class="meta-chip">
          <span class="meta-label">Temperature</span>
          <span class="meta-value" id="temperatureValue">)rawliteral";

  html += String(state.temperatureF, 1);

  html += R"rawliteral( &deg;F</span>
        </div>
        <div class="meta-chip">
          <span class="meta-label">Humidity</span>
          <span class="meta-value" id="humidityValue">)rawliteral";

  html += String(state.humidityPercent, 1);

  html += R"rawliteral(%</span>
        </div>
      </div>
    </div>

    <div class="proof-card">
      <div>
        <div class="eyebrow">Current Proof</div>
        <h2 class="proof-title" id="profileValue">)rawliteral";

  html += htmlEscape(state.selectedRecipe);

  html += R"rawliteral(</h2>
        <div class="proof-meta" id="proofMetaValue">)rawliteral";

  if (state.recipeConfigured) {
    html += htmlEscape(state.selectedStage);
    html += " / Target ";
    html += String(state.targetRisePercent, 0);
    html += "%";
  } else {
    html += "Choose your bake or adjust the target before you start.";
  }

  html += R"rawliteral(</div>
        <div class="small" id="recipeLinkWrap")rawliteral";

  RecipePreset selectedPreset;
  const bool hasSelectedPreset = findRecipePreset(state.selectedRecipe, selectedPreset);
  const bool hasSourceUrl = hasSelectedPreset && selectedPreset.sourceUrl != nullptr;

  if (!hasSourceUrl) {
    html += " style=\"display:none\"";
  }

  html += R"rawliteral(>
          <a id="recipeLinkAnchor" href=")rawliteral";

  html += hasSourceUrl ? htmlEscape(selectedPreset.sourceUrl) : "#";

  html += R"rawliteral(" target="_blank" rel="noopener">Ingredients &amp; Mixing: King Arthur Recipe &#8599;</a><br>
          <span id="recipeLinkCaption">)rawliteral";

  if (!hasSelectedPreset || selectedPreset.hasFinal) {
    html += "Follow the recipe for mixing and shaping. Use this monitor for both proofing stages.";
  } else if (selectedPreset.untrackedFinalNote != nullptr) {
    html += "Follow the recipe for mixing and shaping. Use this monitor for the bulk proofing stage, then follow the recipe for the final proof.";
  } else {
    html += "Follow the recipe for mixing and shaping. Use this monitor for the proofing stage.";
  }

  html += R"rawliteral(</span>
        </div>
      </div>
      <div class="target-badge">
        <span class="small" id="stageValue">)rawliteral";

  html += htmlEscape(state.selectedStage);

  html += R"rawliteral(</span>
        <strong id="targetValue">)rawliteral";

  html += String(state.targetRisePercent, 0);

  html += R"rawliteral(%</strong>
      </div>
    </div>

    <div class="guidance-card">
      <div class="guidance-block">
        <div class="eyebrow">Current Step</div>
        <div class="guidance-step" id="currentStepValue">)rawliteral";

  html += htmlEscape(currentStepTitle(state));

  html += R"rawliteral(</div>
      </div>
      <div class="guidance-block guidance-block-primary">
        <div class="eyebrow">What to do now</div>
        <div class="guidance-copy guidance-copy-primary" id="instructionValue">)rawliteral";

  html += htmlEscape(currentStepInstruction(state));

  html += R"rawliteral(</div>
        <div class="event-action" id="openRecipeWrap")rawliteral";

  if (!(proofAwaitingDoughPrep(state) && hasSourceUrl)) {
    html += " style=\"display:none\"";
  }

  html += R"rawliteral(>
          <a id="openRecipeLink" class="button-like" href=")rawliteral";

  html += hasSourceUrl ? htmlEscape(selectedPreset.sourceUrl) : "#";

  html += R"rawliteral(" target="_blank" rel="noopener">Open Recipe &#8599;</a>
        </div>
        <div class="event-action" id="completeStepWrap")rawliteral";

  if (!proofCanCompleteEvent(state)) {
    html += " style=\"display:none\"";
  }

  html += R"rawliteral(>
          <button id="completeStepButton" onclick="sendAction('/complete-event')">Complete Step</button>
        </div>
      </div>
      <div class="guidance-block">
        <div class="eyebrow">Next Step</div>
        <div class="guidance-copy" id="nextStepValue">)rawliteral";

  const String upcomingStep = upcomingStepText(state);
  html += htmlEscape(upcomingStep.length() > 0 ? upcomingStep : "No further proof steps.");

  html += R"rawliteral(</div>
        <div class="event-action" id="chooseProfileWrap")rawliteral";

  if (state.recipeConfigured) {
    html += " style=\"display:none\"";
  }

  html += R"rawliteral(>
          <button id="chooseProfileButton" onclick="openProfilePicker()">Choose Your Bake</button>
        </div>
      </div>
    </div>

    <div class="callout" id="outcomeCard" )rawliteral";

  if (!state.outcomeFeedbackPending) {
    html += "style=\"display:none\"";
  }

  html += R"rawliteral(>
      <div class="eyebrow">Quick Feedback</div>
      <div id="outcomeText" style="margin-top:6px;"></div>
      <div class="actions" style="margin-top:12px;">
        <button class="secondary" onclick="rateOutcome('under')">Underproofed</button>
        <button onclick="rateOutcome('good')">Just Right</button>
        <button class="secondary" onclick="rateOutcome('over')">Overproofed</button>
        <button class="ghost" onclick="skipOutcome()">Skip</button>
      </div>
    </div>

    <details id="profileDetails" class="editor-details">
      <summary class="editor-summary">
        <div class="editor-summary-copy">
          <span class="editor-summary-title">Choose Your Bake</span>
          <span class="editor-summary-hint">Change bake, stage, or target</span>
        </div>
        <span class="editor-summary-icon" aria-hidden="true">▾</span>
      </summary>
      <div class="stack">
        <div class="small">These are suggested starting points. Adjust based on your dough and results.</div>
        <label>Category
          <select id="categorySelect" onchange="populateProfiles()"></select>
        </label>
        <label>Bake Type
          <select id="profileSelect" onchange="updatePresetTarget()"></select>
        </label>
        <label>Proof Stage
          <select id="stageSelect" onchange="updatePresetTarget()">
            <option value="Bulk">Bulk rise</option>
            <option value="Final">Final rise</option>
          </select>
        </label>
        <label>Target Rise (%)
          <input id="targetInput" type="number" min="10" max="200" value=")rawliteral";

  html += String(state.targetRisePercent, 0);

  html += R"rawliteral(">
        </label>
        <button onclick="applyProfileTarget()">Set This Bake</button>
      </div>
    </details>

    <div class="actions">
      <button id="setEmptyButton" class="secondary" onclick="sendAction('/set-empty')">Set Empty Setup</button>
      <button id="setStartButton" onclick="sendAction('/set-start')">Set Starting Dough Height</button>
      <button id="pauseButton" class="secondary" onclick="sendAction('/pause-proof')">Pause Monitoring</button>
      <button id="resumeButton" class="ghost" onclick="sendAction('/resume-proof')">Resume Monitoring</button>
      <button id="finishButton" onclick="sendAction('/finish-proof')">Finish Proof</button>
      <button id="rebaselineButton" class="secondary" onclick="sendAction('/rebaseline')">Re-baseline After Lid Off</button>
      <button onclick="sendAction('/test-ntfy')">Send Test Phone Alert</button>
      <button class="danger" onclick="sendAction('/reset-proof')">Start New Proof</button>
    </div>
    <div class="action-feedback" id="setEmptyFeedback">Set this before measuring dough rise in a new setup.</div>
    <div class="action-feedback" id="setStartFeedback"></div>
    <div class="action-feedback">Pause Monitoring before lifting the lid for any reason. Use Re-baseline afterward if it wasn't a scripted fold step, so a shift on set-down isn't read as rise.</div>
    <div class="action-feedback" id="rebaselineFeedback"></div>

    <details>
      <summary>Details</summary>
      <div class="rows">
        <div class="row"><span class="label">Distance</span><span class="value" id="distanceValue">)rawliteral";

  html += String(state.smoothedDistanceMm, 1);

  html += R"rawliteral( mm</span></div>
        <div class="row"><span class="label">Current dough height</span><span class="value" id="currentHeightValue">)rawliteral";

  html += String(currentDoughHeightMm(state), 1);

  html += R"rawliteral( mm</span></div>
        <div class="row"><span class="label">Starting dough height</span><span class="value" id="startingHeightValue">)rawliteral";

  html += String(startingDoughHeightMm(state), 1);

  html += R"rawliteral( mm</span></div>
        <div class="row"><span class="label">Phone alert</span><span class="value" id="phoneAlertValue">)rawliteral";

  html += phoneAlertText(state);

  html += R"rawliteral(</span></div>
      </div>
    </details>

    <details>
      <summary>Phone Alerts</summary>
      <div class="small">Subscribe to this topic in the ntfy app to get fold reminders and Ready to Shape or Ready to Bake alerts on your phone.</div>
      <div class="topic" id="topicText">)rawliteral";

  html += htmlEscape(state.ntfyTopic);

  html += R"rawliteral(</div>
      <button onclick="copyTopic()">Copy Alert Topic</button>
      <div class="hint" id="copyNote">
        Install the <strong>ntfy</strong> app, then subscribe to the topic above.<br>
        You can also open <a href=")rawliteral";

  html += R"rawliteral(https://ntfy.sh/)rawliteral";
  html += htmlEscape(state.ntfyTopic);

  html += R"rawliteral(" target="_blank">this topic page</a>.<br>
        The topic is randomly generated and stored only on this ESP32.
      </div>
      <button class="secondary" onclick="sendAction('/new-ntfy-topic')">Generate New Alert Topic</button>
    </details>

    <details>
      <summary>Advanced</summary>
      <div class="small">Developer / testing</div>
      <button class="secondary" onclick="sendAction('/use-calibration-profile')">Use Calibration Test Profile</button>
      <button class="secondary" onclick="sendAction('/reset-learned-targets')">Reset Learned Targets</button>
      <button class="secondary" onclick="sendAction('/reset-setup')">Reset Wi-Fi Setup</button>
    </details>

    <div class="hint">
      Open this page at <strong>http://proofmonitor.local</strong>.<br>
      Live readings update every 15 seconds without refreshing the page.
    </div>
  </div>
</div>
<script>
window.PROOF_MONITOR_BOOT = )rawliteral";

  html += liveDataJson(state);

  html += R"rawliteral(;
window.PROOF_MONITOR_PRESETS = )rawliteral";

  html += proofProfilesJson();

  html += R"rawliteral(;
</script>
<script>)rawliteral";

  html += kDashboardJs;

  html += R"rawliteral(</script>
</body>
</html>
)rawliteral";

  return html;
}

String setupPage(const AppState& state) {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Proof Monitor Setup</title>
<style>)rawliteral";

  html += kSetupCss;

  html += R"rawliteral(</style>
</head>
<body>
<div class="shell">
  <div class="panel">
    <div class="eyebrow">First-Time Setup</div>
    <h1>Connect Your Proof Monitor</h1>
    <p class="note">Join your home Wi-Fi, then subscribe to this monitor's ntfy topic for phone alerts.</p>

    <label>Choose home Wi-Fi
      <select id="ssidSelect" onchange="document.getElementById('ssidManual').value=this.value">)rawliteral";

  html += wifiOptionsHtml(state);

  html += R"rawliteral(</select>
    </label>

    <label>Or type Wi-Fi name manually
      <input id="ssidManual" value=")rawliteral";

  html += htmlEscape(state.wifiSsid);

  html += R"rawliteral(">
    </label>

    <label>Home Wi-Fi password
      <input id="pass" type="password" placeholder="Leave blank to keep existing password">
    </label>

    <label>Phone alert topic</label>
    <div class="topic" id="topicText">)rawliteral";

  html += htmlEscape(state.ntfyTopic);

  html += R"rawliteral(</div>

    <button onclick="copyTopic()">Copy Alert Topic</button>

    <div class="note" id="copyNote">
      Install the <strong>ntfy</strong> app, then subscribe to the topic above.<br>
      You can also open <a href="https://ntfy.sh/)rawliteral";

  html += htmlEscape(state.ntfyTopic);

  html += R"rawliteral(" target="_blank">this topic page</a>.<br>
      The topic is randomly generated and stored only on this ESP32.
    </div>

    <button onclick="saveConfig()">Save and Restart</button>
    <button class="secondary" onclick="sendAction('/new-ntfy-topic')">Generate New Alert Topic</button>
    <button class="secondary" onclick="location.reload()">Rescan Wi-Fi Networks</button>
  </div>
</div>
<script>)rawliteral";

  html += kSetupJs;

  html += R"rawliteral(</script>
</body>
</html>
)rawliteral";

  return html;
}

void handleRoot() {
  if (gState->setupMode) {
    gServer->send(200, "text/html; charset=utf-8", setupPage(*gState));
  } else {
    gServer->send(200, "text/html; charset=utf-8", dashboardPage(*gState));
  }
}

void handleData() {
  gServer->send(200, "application/json", liveDataJson(*gState));
}

void handleSaveConfig() {
  saveWifiConfig(*gPrefs, *gState, gServer->arg("ssid"), gServer->arg("pass"));
  ensureNtfyTopic(*gPrefs, *gState);
  gServer->send(200, "text/plain", "OK");
  scheduleRestart(*gState);
}

void handleNewNtfyTopic() {
  gState->ntfyTopic = generateNtfyTopic();
  gPrefs->putString("ntfy", gState->ntfyTopic);
  gState->targetNotified = false;
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handleSetProfileTarget() {
  const String previousProfile = gState->selectedRecipe;
  const String previousStage = gState->selectedStage;

  if (gServer->hasArg("profile")) gState->selectedRecipe = normalizeProfileName(gServer->arg("profile"));
  else if (gServer->hasArg("recipe")) gState->selectedRecipe = normalizeProfileName(gServer->arg("recipe"));
  if (gServer->hasArg("stage")) {
    const String stageArg = gServer->arg("stage");
    if (stageArg == "Bulk" || stageArg == "Final") gState->selectedStage = stageArg;
  }

  if (gServer->hasArg("target")) {
    gState->targetRisePercent = gServer->arg("target").toFloat();
    if (gState->targetRisePercent < 10.0f) gState->targetRisePercent = 10.0f;
    if (gState->targetRisePercent > 200.0f) gState->targetRisePercent = 200.0f;
  }

  RecipePreset preset;
  if (!findRecipePreset(gState->selectedRecipe, preset)) {
    gState->selectedRecipe = "Custom";
  }

  gState->recipeConfigured = true;
  gState->targetNotified = false;
  gState->awaitingFinalProofStart = false;
  if (stageChangeNeedsEmptyReset(previousProfile, previousStage, gState->selectedRecipe, gState->selectedStage)) {
    clearActiveProofRunForStage(*gState, true);
  }
  resetFermentationEventState(*gState);
  // A rating for whatever was pending before this bake was chosen isn't useful feedback this
  // far removed from it, and the recipe/stage it would have nudged gets another chance later.
  clearAllPendingOutcomes(*gState);
  reconcileProofStateWithTarget();
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handleUseCalibrationProfile() {
  const RecipePreset& preset = calibrationTestPreset();
  gState->selectedRecipe = preset.name;
  gState->selectedStage = "Bulk";
  gState->targetRisePercent = preset.bulk;
  gState->recipeConfigured = true;
  gState->targetNotified = false;
  gState->awaitingFinalProofStart = false;
  resetFermentationEventState(*gState);
  reconcileProofStateWithTarget();
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handleSetEmpty() {
  if (!proofCanSetEmpty(*gState)) {
    gServer->send(409, "text/plain", "Proof is paused or finished.");
    return;
  }

  float capturedDistanceMm = 0.0f;
  if (!captureStableDistanceMm(*gState, capturedDistanceMm)) {
    gServer->send(409, "text/plain", "Could not capture a stable distance reading.");
    return;
  }

  gState->emptyDistanceMm = capturedDistanceMm;
  persistState();
  sendNtfyNotification(*gState, "Empty Setup Captured",
                        "Add your dough, replace the lid, and let it settle a moment "
                        "before pressing Set Starting Dough Height.");
  gServer->send(200, "text/plain", "OK");
}

void handleSetStart() {
  if (!proofCanSetStart(*gState)) {
    const char* message = proofHasEmptyCalibration(*gState) ? "Proof is paused or finished." : "Set empty setup first.";
    gServer->send(409, "text/plain", message);
    return;
  }

  float capturedDistanceMm = 0.0f;
  if (!captureStableDistanceMm(*gState, capturedDistanceMm)) {
    gServer->send(409, "text/plain", "Could not capture a stable distance reading.");
    return;
  }

  startProof(*gState);
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handlePauseProof() {
  pauseProof(*gState);
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handleResumeProof() {
  resumeProof(*gState);
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handleFinishProof() {
  finishProof(*gState);
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handleCompleteEvent() {
  if (!proofCanCompleteEvent(*gState)) {
    gServer->send(409, "text/plain", "No handling step is ready.");
    return;
  }

  float capturedDistanceMm = 0.0f;
  if (!captureStableDistanceMm(*gState, capturedDistanceMm)) {
    gServer->send(409, "text/plain", "Could not capture a stable distance reading.");
    return;
  }

  completeCurrentEvent(*gState);
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handleRebaseline() {
  if (!proofCanRebaseline(*gState)) {
    gServer->send(409, "text/plain", "Nothing to re-baseline right now.");
    return;
  }

  float capturedDistanceMm = 0.0f;
  if (!captureStableDistanceMm(*gState, capturedDistanceMm)) {
    gServer->send(409, "text/plain", "Could not capture a stable distance reading.");
    return;
  }

  rebaselineProof(*gState);
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handleRateOutcome() {
  if (!gState->outcomeFeedbackPending) {
    gServer->send(409, "text/plain", "No outcome is pending.");
    return;
  }

  const String rating = gServer->arg("rating");
  OutcomeRating parsed;
  if (rating == "under") {
    parsed = OutcomeRating::Underproofed;
  } else if (rating == "good") {
    parsed = OutcomeRating::JustRight;
  } else if (rating == "over") {
    parsed = OutcomeRating::Overproofed;
  } else {
    gServer->send(400, "text/plain", "Unknown rating.");
    return;
  }

  RecipePreset preset;
  const bool isBulk = gState->outcomeStage == "Bulk";
  uint8_t shippedDefault = 75;
  if (findRecipePreset(gState->outcomeRecipe, preset)) {
    shippedDefault = isBulk ? preset.bulk : preset.final;
  }

  recordOutcome(gState->outcomeRecipe, gState->outcomeStage, shippedDefault, parsed);

  advanceOutcomeQueue(*gState);
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handleSkipOutcome() {
  if (!gState->outcomeFeedbackPending) {
    gServer->send(409, "text/plain", "No outcome is pending.");
    return;
  }

  advanceOutcomeQueue(*gState);
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handleResetLearnedTargets() {
  resetLearnedTargets();
  gServer->send(200, "text/plain", "OK");
}

void handleResetProof() {
  startNewProof(*gState);
  persistState();
  gServer->send(200, "text/plain", "OK");
}

void handleResetSetup() {
  resetWifiConfig(*gPrefs, *gState);
  gServer->send(200, "text/plain", "OK");
  scheduleRestart(*gState);
}

void handleTestNtfy() {
  const bool sent = sendNtfyNotification(*gState, "Proof Monitor test alert.");
  gServer->send(sent ? 200 : 500, "text/plain", sent ? "OK" : "FAILED");
}
}  // namespace

void configureWebServer(WebServer& server, Preferences& prefs, AppState& state) {
  gServer = &server;
  gPrefs = &prefs;
  gState = &state;

  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData);
  server.on("/save-config", HTTP_POST, handleSaveConfig);
  server.on("/new-ntfy-topic", HTTP_POST, handleNewNtfyTopic);
  server.on("/set-profile-target", HTTP_POST, handleSetProfileTarget);
  server.on("/set-recipe-target", HTTP_POST, handleSetProfileTarget);
  server.on("/set-empty", HTTP_POST, handleSetEmpty);
  server.on("/set-start", HTTP_POST, handleSetStart);
  server.on("/pause-proof", HTTP_POST, handlePauseProof);
  server.on("/resume-proof", HTTP_POST, handleResumeProof);
  server.on("/finish-proof", HTTP_POST, handleFinishProof);
  server.on("/complete-event", HTTP_POST, handleCompleteEvent);
  server.on("/rebaseline", HTTP_POST, handleRebaseline);
  server.on("/rate-outcome", HTTP_POST, handleRateOutcome);
  server.on("/skip-outcome", HTTP_POST, handleSkipOutcome);
  server.on("/reset-learned-targets", HTTP_POST, handleResetLearnedTargets);
  server.on("/reset-proof", HTTP_POST, handleResetProof);
  server.on("/use-calibration-profile", HTTP_POST, handleUseCalibrationProfile);
  server.on("/reset-setup", HTTP_POST, handleResetSetup);
  server.on("/test-ntfy", HTTP_POST, handleTestNtfy);
}
