#pragma once

static const char kDashboardCss[] = R"rawliteral(
:root {
  --bg: #f3eadc;
  --bg-deep: #e1cfb1;
  --panel: rgba(255, 252, 247, 0.88);
  --panel-strong: rgba(255, 248, 240, 0.96);
  --ink: #332518;
  --muted: #786658;
  --line: rgba(84, 57, 34, 0.12);
  --accent: #9b5329;
  --accent-soft: #eed4bc;
  --danger: #6f3420;
  --shadow: 0 20px 56px rgba(69, 40, 19, 0.16);
}

* { box-sizing: border-box; }

body {
  margin: 0;
  min-height: 100vh;
  padding: 20px;
  background:
    radial-gradient(circle at top left, rgba(255, 255, 255, 0.85), transparent 38%),
    linear-gradient(160deg, var(--bg) 0%, var(--bg-deep) 100%);
  color: var(--ink);
  font-family: "Avenir Next", "Segoe UI", sans-serif;
}

.shell {
  width: 100%;
  max-width: 640px;
  margin: 0 auto;
}

.panel {
  background: var(--panel);
  border: 1px solid rgba(255, 255, 255, 0.55);
  border-radius: 32px;
  padding: 22px;
  box-shadow: var(--shadow);
  backdrop-filter: blur(14px);
}

.eyebrow,
.small,
.hint {
  color: var(--muted);
}

.eyebrow {
  text-transform: uppercase;
  letter-spacing: 0.18em;
  font-size: 12px;
}

.hero {
  background: linear-gradient(145deg, rgba(255, 248, 240, 0.94), rgba(246, 232, 213, 0.84));
  border: 1px solid rgba(145, 96, 57, 0.12);
  border-radius: 28px;
  padding: 20px;
}

.hero h1 {
  margin: 6px 0 0;
  font-family: "Iowan Old Style", "Palatino Linotype", ui-serif, Georgia, serif;
  font-size: 33px;
  font-weight: 700;
}

.rise-row {
  display: flex;
  align-items: flex-end;
  justify-content: space-between;
  gap: 16px;
  margin-top: 14px;
}

.rise-value {
  font-family: "Iowan Old Style", "Palatino Linotype", ui-serif, Georgia, serif;
  font-size: clamp(72px, 18vw, 112px);
  line-height: 0.9;
  font-weight: 700;
}

.status-pill {
  display: inline-flex;
  align-items: center;
  gap: 8px;
  border-radius: 999px;
  padding: 9px 14px;
  background: var(--accent);
  color: #fff8f1;
  font-size: 14px;
  font-weight: 700;
}

.hero-meta {
  display: grid;
  grid-template-columns: repeat(3, minmax(0, 1fr));
  gap: 12px;
  margin-top: 16px;
}

.meta-chip {
  background: rgba(255, 255, 255, 0.55);
  border: 1px solid var(--line);
  border-radius: 18px;
  padding: 12px;
}

.meta-label {
  display: block;
  color: var(--muted);
  font-size: 12px;
  text-transform: uppercase;
  letter-spacing: 0.08em;
}

.meta-value {
  display: block;
  margin-top: 4px;
  font-size: 20px;
  font-weight: 700;
}

.proof-card,
.callout,
.guidance-card {
  margin-top: 18px;
  border-radius: 22px;
  padding: 16px 18px;
  background: var(--panel-strong);
  border: 1px solid var(--line);
}

.proof-card {
  display: flex;
  justify-content: space-between;
  gap: 16px;
  align-items: center;
}

.proof-title {
  margin: 0;
  font-size: 22px;
}

.proof-meta {
  margin-top: 6px;
  color: var(--muted);
}

.target-badge {
  flex-shrink: 0;
  min-width: 98px;
  text-align: center;
  border-radius: 18px;
  padding: 12px 14px;
  background: var(--accent-soft);
}

.target-badge .small {
  display: block;
  font-size: 11px;
  text-transform: uppercase;
  letter-spacing: 0.08em;
}

.target-badge strong {
  display: block;
  margin-top: 4px;
  font-size: 26px;
}

.callout {
  color: var(--ink);
  line-height: 1.5;
}

.guidance-card {
  display: grid;
  gap: 16px;
}

.guidance-block {
  border-bottom: 1px solid var(--line);
  padding-bottom: 14px;
}

.guidance-block:last-child {
  border-bottom: 0;
  padding-bottom: 0;
}

.guidance-step {
  margin-top: 6px;
  font-size: 24px;
  font-weight: 700;
}

.guidance-copy {
  margin-top: 6px;
  color: var(--ink);
  line-height: 1.55;
}

.guidance-block-primary {
  padding: 12px 14px;
  background: var(--accent-soft);
  border-radius: 16px;
  border-bottom: 0;
}

.guidance-block-primary .eyebrow {
  color: var(--accent);
}

.guidance-copy-primary {
  font-size: 17px;
  font-weight: 600;
}

.event-action {
  margin-top: 14px;
}

.rows {
  margin-top: 18px;
  border-top: 1px solid var(--line);
}

.row {
  display: flex;
  justify-content: space-between;
  gap: 16px;
  padding: 12px 0;
  border-bottom: 1px solid var(--line);
}

.label {
  color: var(--muted);
}

.value {
  text-align: right;
  font-weight: 700;
}

.actions {
  display: grid;
  grid-template-columns: repeat(2, minmax(0, 1fr));
  gap: 12px;
  margin-top: 18px;
}

button,
input,
select {
  width: 100%;
  border: 0;
  border-radius: 16px;
  font: inherit;
}

.button-like {
  display: block;
  box-sizing: border-box;
  width: 100%;
  border-radius: 16px;
  text-align: center;
  text-decoration: none;
}

button,
.button-like {
  padding: 14px 16px;
  background: var(--accent);
  color: #fff8f1;
  font-weight: 700;
  cursor: pointer;
  box-shadow: 0 6px 16px rgba(110, 63, 29, 0.18);
  transition: transform 0.08s ease, filter 0.08s ease, box-shadow 0.08s ease;
}

button:hover {
  filter: brightness(0.98);
}

button:active {
  transform: translateY(1px);
  box-shadow: 0 3px 10px rgba(110, 63, 29, 0.16);
}

button.secondary {
  background: #f4e8da;
  color: var(--ink);
}

button.secondary.ready {
  background: linear-gradient(145deg, #b96b39, var(--accent));
  color: #fff8f1;
  box-shadow: 0 10px 24px rgba(110, 63, 29, 0.24);
}

button.saved {
  background: #d7ead8;
  color: #24452d;
  box-shadow: 0 8px 20px rgba(70, 112, 76, 0.16);
}

button.ghost {
  background: rgba(255, 255, 255, 0.62);
  color: var(--ink);
}

button.danger {
  background: var(--danger);
}

button:disabled {
  cursor: not-allowed;
  opacity: 0.46;
  box-shadow: none;
}

details {
  margin-top: 18px;
  border-top: 1px solid var(--line);
  padding-top: 10px;
}

summary {
  cursor: pointer;
  list-style: none;
  font-weight: 700;
  padding: 10px 0;
}

summary::-webkit-details-marker {
  display: none;
}

.editor-details {
  margin-top: 18px;
  border-top: 0;
  padding-top: 0;
}

.editor-summary {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 16px;
  padding: 14px 16px;
  border-radius: 18px;
  background: rgba(255, 248, 240, 0.92);
  border: 1px solid var(--line);
  box-shadow: 0 8px 22px rgba(110, 63, 29, 0.08);
}

.editor-summary-copy {
  display: grid;
  gap: 2px;
}

.editor-summary-title {
  color: var(--ink);
  font-size: 16px;
  font-weight: 700;
}

.editor-summary-hint {
  color: var(--muted);
  font-size: 13px;
  line-height: 1.35;
}

.editor-summary-icon {
  flex-shrink: 0;
  color: var(--muted);
  font-size: 18px;
  transition: transform 0.12s ease;
}

.editor-details[open] .editor-summary-icon {
  transform: rotate(180deg);
}

.editor-details .stack {
  margin-top: 12px;
}

.stack {
  display: grid;
  gap: 12px;
}

label {
  display: block;
  color: var(--muted);
  font-size: 13px;
}

input,
select {
  margin-top: 6px;
  padding: 13px 14px;
  background: rgba(255, 255, 255, 0.7);
  color: var(--ink);
  border: 1px solid rgba(120, 102, 88, 0.16);
}

.hint {
  margin-top: 18px;
  font-size: 14px;
  line-height: 1.5;
}

.action-feedback {
  min-height: 20px;
  margin-top: 10px;
  padding-left: 2px;
  color: var(--muted);
  font-size: 13px;
  line-height: 1.4;
}

.action-feedback.success {
  color: #335c3a;
  font-weight: 600;
}

.topic {
  margin-top: 8px;
  padding: 14px;
  border-radius: 18px;
  background: rgba(255, 255, 255, 0.68);
  border: 1px solid var(--line);
  font-family: "SFMono-Regular", Consolas, monospace;
  word-break: break-all;
}

@media (max-width: 560px) {
  body { padding: 14px; }
  .panel { border-radius: 24px; padding: 16px; }
  .hero { border-radius: 22px; }
  .proof-card { flex-direction: column; align-items: flex-start; }
  .target-badge { width: 100%; text-align: left; }
  .hero-meta,
  .actions { grid-template-columns: 1fr; }
  .rise-row { flex-direction: column; align-items: flex-start; }
}
)rawliteral";

static const char kDashboardJs[] = R"rawliteral(
const BOOT = window.PROOF_MONITOR_BOOT || {};
const PROFILE_LIST = window.PROOF_MONITOR_PRESETS || [];
const PRESETS = {};

for (const preset of PROFILE_LIST) {
  if (!PRESETS[preset.category]) PRESETS[preset.category] = [];
  PRESETS[preset.category].push(preset);
}

let currentProfile = BOOT.profile || 'Custom';
let currentStage = BOOT.stage || 'Bulk';
let setEmptySavedUntil = 0;
let setStartSavedUntil = 0;
let rebaselineSavedUntil = 0;

function handleResponse(response) {
  if (response.ok) return response;
  return response.text().then((text) => {
    throw new Error(text || 'Request failed.');
  });
}

function setText(id, value) {
  const element = document.getElementById(id);
  if (element) element.textContent = value;
}

function setHtml(id, value) {
  const element = document.getElementById(id);
  if (element) element.innerHTML = value;
}

function toggleButton(id, enabled) {
  const button = document.getElementById(id);
  if (!button) return;
  button.disabled = !enabled;
}

function updateSetEmptyUi(data) {
  const button = document.getElementById('setEmptyButton');
  const feedback = document.getElementById('setEmptyFeedback');
  if (!button || !feedback) return;

  const now = Date.now();
  const showSaved = data.hasEmptySetup && now < setEmptySavedUntil;

  button.classList.remove('ready', 'saved');

  if (showSaved) {
    button.textContent = 'Empty Setup Saved';
    button.classList.add('saved');
    feedback.textContent = 'Empty setup captured. You can set starting dough height when ready.';
    feedback.classList.add('success');
    return;
  }

  button.textContent = 'Set Empty Setup';
  feedback.classList.remove('success');

  if (data.canSetEmpty) {
    button.classList.add('ready');
    if (data.hasEmptySetup) {
      feedback.textContent = 'Tap Set Empty Setup again if you moved to a new pan, bucket, tray, or monitor position.';
    } else {
      feedback.textContent = 'Ready to capture the empty setup before measuring dough rise.';
    }
    return;
  }

  if (data.hasEmptySetup) {
    feedback.textContent = 'Empty setup is already saved for this stage.';
  } else {
    feedback.textContent = 'Waiting for a live distance reading before empty setup can be saved.';
  }
}

function updateSetStartUi(data) {
  const button = document.getElementById('setStartButton');
  const feedback = document.getElementById('setStartFeedback');
  if (!button || !feedback) return;

  const hasStartingHeight = data.startingHeight > 0;
  const showSaved = hasStartingHeight && Date.now() < setStartSavedUntil;

  button.classList.remove('saved');
  feedback.classList.remove('success');

  if (showSaved) {
    button.textContent = 'Starting Height Saved';
    button.classList.add('saved');
    feedback.textContent = 'Starting dough height captured. Monitoring will track rise from here.';
    feedback.classList.add('success');
    return;
  }

  button.textContent = 'Set Starting Dough Height';
  feedback.textContent = hasStartingHeight
    ? 'Tap again only if you moved the dough or reset the container - each press restarts tracking from a new reading.'
    : '';
}

function updateRebaselineUi() {
  const button = document.getElementById('rebaselineButton');
  const feedback = document.getElementById('rebaselineFeedback');
  if (!button || !feedback) return;

  const showSaved = Date.now() < rebaselineSavedUntil;

  button.classList.remove('saved');
  feedback.classList.remove('success');

  if (showSaved) {
    button.textContent = 'Re-baselined';
    button.classList.add('saved');
    feedback.textContent = 'New baseline captured. Rise will track from here.';
    feedback.classList.add('success');
    return;
  }

  button.textContent = 'Re-baseline After Lid Off';
  feedback.textContent = '';
}

function findPresetByName(name) {
  for (const category of Object.keys(PRESETS)) {
    for (const preset of PRESETS[category]) {
      if (preset.name === name) return { category, preset };
    }
  }

  const fallbackCategory = Object.keys(PRESETS)[0] || 'Custom';
  const fallbackList = PRESETS[fallbackCategory] || [];
  return { category: fallbackCategory, preset: fallbackList[0] };
}

function populateCategories() {
  const categorySelect = document.getElementById('categorySelect');
  const found = findPresetByName(currentProfile);
  categorySelect.innerHTML = '';

  for (const category of Object.keys(PRESETS)) {
    const option = document.createElement('option');
    option.value = category;
    option.textContent = category;
    if (category === found.category) option.selected = true;
    categorySelect.appendChild(option);
  }

  populateProfiles(false);
}

function populateProfiles(changeValue = true) {
  const category = document.getElementById('categorySelect').value;
  const profileSelect = document.getElementById('profileSelect');
  profileSelect.innerHTML = '';

  for (const preset of PRESETS[category] || []) {
    const option = document.createElement('option');
    option.value = preset.name;
    option.textContent = preset.name;
    if (preset.name === currentProfile) option.selected = true;
    profileSelect.appendChild(option);
  }

  if (!profileSelect.value && profileSelect.options.length > 0) profileSelect.selectedIndex = 0;

  document.getElementById('stageSelect').value = currentStage;
  updatePresetTarget(changeValue);
}

function updatePresetTarget(changeValue = true) {
  const category = document.getElementById('categorySelect').value;
  const profileName = document.getElementById('profileSelect').value;
  const stageSelect = document.getElementById('stageSelect');
  const preset = (PRESETS[category] || []).find((entry) => entry.name === profileName);
  if (!preset) return;

  const finalOption = stageSelect.querySelector('option[value="Final"]');
  finalOption.disabled = !preset.hasFinal;

  if (!preset.hasFinal && stageSelect.value === 'Final') {
    stageSelect.value = 'Bulk';
  }

  if (changeValue) {
    const target = stageSelect.value === 'Final' ? preset.final : preset.bulk;
    document.getElementById('targetInput').value = target;
  }
}

function updateRecipeLink(profileName) {
  const recipeLinkWrap = document.getElementById('recipeLinkWrap');
  const recipeLinkAnchor = document.getElementById('recipeLinkAnchor');
  if (!recipeLinkWrap || !recipeLinkAnchor) return;

  const found = findPresetByName(profileName);
  const preset = found.preset;

  if (preset && preset.sourceUrl) {
    recipeLinkAnchor.href = preset.sourceUrl;
    recipeLinkWrap.style.display = 'block';
    setText('recipeLinkCaption', recipeLinkCaptionFor(preset));
  } else {
    recipeLinkWrap.style.display = 'none';
  }
}

function updateOpenRecipeLink(profileName, needsDoughPrepLink) {
  const openRecipeWrap = document.getElementById('openRecipeWrap');
  const openRecipeLink = document.getElementById('openRecipeLink');
  if (!openRecipeWrap || !openRecipeLink) return;

  const found = findPresetByName(profileName);
  const preset = found.preset;

  if (needsDoughPrepLink && preset && preset.sourceUrl) {
    openRecipeLink.href = preset.sourceUrl;
    openRecipeWrap.style.display = 'block';
  } else {
    openRecipeWrap.style.display = 'none';
  }
}

function recipeLinkCaptionFor(preset) {
  if (preset.hasFinal) {
    return 'Follow the recipe for mixing and shaping. Use this monitor for both proofing stages.';
  }
  if (preset.hasUntrackedFinal) {
    return 'Follow the recipe for mixing and shaping. Use this monitor for the bulk proofing stage, then follow the recipe for the final proof.';
  }
  return 'Follow the recipe for mixing and shaping. Use this monitor for the proofing stage.';
}

function applyLiveData(data) {
  currentProfile = data.profile || currentProfile;
  currentStage = data.stage || currentStage;
  setText('riseValue', data.rise + '%');
  setText('statusValue', data.status);
  setText('elapsedValue', data.elapsed);
  setText('temperatureValue', data.temperature + ' \u00B0F');
  setText('humidityValue', data.humidity + '%');
  setText('profileValue', data.profile);
  setText('stageValue', data.stage);
  updateRecipeLink(data.profile);
  updateOpenRecipeLink(data.profile, data.needsDoughPrepLink);
  setText('targetValue', data.target + '%');
  setText('distanceValue', data.distance + ' mm');
  setText('currentHeightValue', data.currentHeight + ' mm');
  setText('startingHeightValue', data.startingHeight + ' mm');
  setText('phoneAlertValue', data.phoneAlert);
  setText('currentStepValue', data.currentStep);
  setText('instructionValue', data.currentInstruction);
  setText('nextStepValue', data.upcomingStep || 'No further proof steps.');
  const completeStepWrap = document.getElementById('completeStepWrap');
  if (completeStepWrap) completeStepWrap.style.display = data.canCompleteStep ? 'block' : 'none';
  const chooseProfileWrap = document.getElementById('chooseProfileWrap');
  if (chooseProfileWrap) chooseProfileWrap.style.display = data.showProfilePicker ? 'block' : 'none';

  const profileMeta = data.showProfilePicker
    ? 'Choose your bake or adjust the target before you start.'
    : data.stage + ' / Target ' + data.target + '%';
  setText('proofMetaValue', profileMeta);
  setText('proofCardEyebrow', data.showProfilePicker ? 'Last Bake' : 'Current Proof');
  const targetBadge = document.getElementById('targetBadge');
  if (targetBadge) targetBadge.style.display = data.showProfilePicker ? 'none' : 'block';

  updateSetEmptyUi(data);
  updateSetStartUi(data);
  toggleButton('setEmptyButton', data.canSetEmpty);
  toggleButton('setStartButton', data.canSetStart);
  toggleButton('pauseButton', data.canPause);
  toggleButton('resumeButton', data.canResume);
  toggleButton('finishButton', data.canFinish);
  toggleButton('rebaselineButton', data.canRebaseline);
  updateRebaselineUi();

  const outcomeCard = document.getElementById('outcomeCard');
  if (outcomeCard) {
    outcomeCard.style.display = data.outcomeFeedbackPending ? 'block' : 'none';
    if (data.outcomeFeedbackPending) {
      setText('outcomeText', 'Rate the dough itself, not the bake: when you finished your last ' +
        data.outcomeRecipe + ' (' + data.outcomeStage + ' proof), was it under, just right, or over? ' +
        'This can wait until you check on it, but rate what you saw then, not how it later baked.');
    }
  }
}

function rateOutcome(rating) {
  // A rating can adjust this recipe's learned target. Reload rather than just refetching
  // live data, so the profile picker (built once from the page's initial load) doesn't keep
  // showing a now-stale target if it's opened later without a manual page refresh.
  fetch('/rate-outcome?rating=' + rating, { method: 'POST' })
    .then(handleResponse)
    .then(() => { window.location.reload(); })
    .catch((error) => alert(error.message || 'Could not reach Proof Monitor.'));
}

function skipOutcome() {
  // Unlike a rating, skipping never touches a learned target, so a plain AJAX refresh is
  // enough here - no need for rateOutcome()'s full reload.
  fetch('/skip-outcome', { method: 'POST' })
    .then(handleResponse)
    .then(() => updateLiveData())
    .catch((error) => alert(error.message || 'Could not reach Proof Monitor.'));
}

function openProfilePicker() {
  const details = document.getElementById('profileDetails');
  if (!details) return;
  details.setAttribute('open', '');
  details.scrollIntoView({ behavior: 'smooth', block: 'start' });
}

function applyProfileTarget() {
  const profile = document.getElementById('profileSelect').value;
  const stage = document.getElementById('stageSelect').value;
  const target = document.getElementById('targetInput').value;

  fetch('/set-profile-target?profile=' + encodeURIComponent(profile) +
        '&stage=' + encodeURIComponent(stage) +
        '&target=' + encodeURIComponent(target), { method: 'POST' })
    .then(handleResponse)
    .then(() => {
      currentProfile = profile;
      currentStage = stage;
      document.getElementById('profileDetails').removeAttribute('open');
      return updateLiveData();
    })
    .catch((error) => alert(error.message || 'Could not reach Proof Monitor.'));
}

function sendAction(path) {
  fetch(path, { method: 'POST' })
    .then(handleResponse)
    .then(() => {
      if (path === '/set-empty') {
        setEmptySavedUntil = Date.now() + 4000;
      }
      if (path === '/set-start') {
        setStartSavedUntil = Date.now() + 4000;
      }
      if (path === '/rebaseline') {
        rebaselineSavedUntil = Date.now() + 4000;
      }
      if (path === '/reset-learned-targets') {
        // Clearing learned adjustments changes every recipe's target; reload so the profile
        // picker (built once from the page's initial load) reflects the reset values too.
        window.location.reload();
        return null;
      }
      if (path === '/new-ntfy-topic') {
        // The new topic isn't part of /data, so an AJAX refresh wouldn't show it; reload
        // to pick up the freshly rendered topic text from the server.
        window.location.reload();
        return null;
      }
      return updateLiveData();
    })
    .catch((error) => alert(error.message || 'Could not reach Proof Monitor.'));
}

function legacyCopyTopic(topic, noteEl) {
  // navigator.clipboard requires a secure context (HTTPS or localhost), which this
  // page is not served over. This older technique still works over plain HTTP in
  // most browsers: put the text in a temporary textarea, select it, and use the
  // deprecated but still-functional execCommand('copy').
  const textarea = document.createElement('textarea');
  textarea.value = topic;
  textarea.setAttribute('readonly', '');
  textarea.style.position = 'fixed';
  textarea.style.left = '-9999px';
  document.body.appendChild(textarea);
  textarea.select();
  textarea.setSelectionRange(0, textarea.value.length);

  let copied = false;
  try {
    copied = document.execCommand('copy');
  } catch (err) {
    copied = false;
  }
  document.body.removeChild(textarea);

  if (noteEl) {
    noteEl.textContent = copied
      ? 'Copied. Paste this topic into the ntfy app.'
      : 'Automatic copy is not supported on this device. Press and hold the topic above to copy it manually.';
  }
}

function copyTopic() {
  const topicEl = document.getElementById('topicText');
  const noteEl = document.getElementById('copyNote');
  const topic = topicEl.textContent;

  if (!navigator.clipboard || !navigator.clipboard.writeText) {
    legacyCopyTopic(topic, noteEl);
    return;
  }

  navigator.clipboard.writeText(topic).then(() => {
    if (noteEl) noteEl.textContent = 'Copied. Paste this topic into the ntfy app.';
  }).catch(() => {
    legacyCopyTopic(topic, noteEl);
  });
}

function updateLiveData() {
  return fetch('/data')
    .then(handleResponse)
    .then((response) => response.json())
    .then((data) => applyLiveData(data))
    .catch(() => console.log('Live update failed'));
}

populateCategories();
applyLiveData(BOOT);
updateLiveData();
setInterval(updateLiveData, 15000);
)rawliteral";

static const char kSetupCss[] = R"rawliteral(
:root {
  --bg: #f3eadc;
  --bg-deep: #e1cfb1;
  --panel: rgba(255, 252, 247, 0.9);
  --ink: #332518;
  --muted: #786658;
  --line: rgba(84, 57, 34, 0.12);
  --accent: #9b5329;
  --shadow: 0 20px 56px rgba(69, 40, 19, 0.16);
}

* { box-sizing: border-box; }

body {
  margin: 0;
  min-height: 100vh;
  padding: 20px;
  background:
    radial-gradient(circle at top left, rgba(255, 255, 255, 0.85), transparent 38%),
    linear-gradient(160deg, var(--bg) 0%, var(--bg-deep) 100%);
  color: var(--ink);
  font-family: "Avenir Next", "Segoe UI", sans-serif;
}

.shell {
  width: 100%;
  max-width: 640px;
  margin: 0 auto;
}

.panel {
  background: var(--panel);
  border-radius: 32px;
  padding: 22px;
  border: 1px solid rgba(255, 255, 255, 0.55);
  box-shadow: var(--shadow);
  backdrop-filter: blur(14px);
}

h1 {
  margin: 8px 0 10px;
  font-family: "Iowan Old Style", "Palatino Linotype", ui-serif, Georgia, serif;
  font-size: 34px;
}

.eyebrow,
.note,
label {
  color: var(--muted);
}

.eyebrow {
  text-transform: uppercase;
  letter-spacing: 0.18em;
  font-size: 12px;
}

label {
  display: block;
  margin-top: 14px;
  font-size: 13px;
}

input,
select,
button {
  width: 100%;
  border: 0;
  border-radius: 16px;
  font: inherit;
}

input,
select {
  margin-top: 6px;
  padding: 13px 14px;
  background: rgba(255, 255, 255, 0.7);
  color: var(--ink);
  border: 1px solid rgba(120, 102, 88, 0.16);
}

button {
  margin-top: 14px;
  padding: 14px 16px;
  background: var(--accent);
  color: #fff8f1;
  font-weight: 700;
  cursor: pointer;
  box-shadow: 0 6px 16px rgba(110, 63, 29, 0.18);
}

button.secondary {
  background: rgba(255, 255, 255, 0.62);
  color: var(--ink);
}

.topic {
  margin-top: 8px;
  padding: 14px;
  border-radius: 18px;
  background: rgba(255, 255, 255, 0.68);
  border: 1px solid var(--line);
  font-family: "SFMono-Regular", Consolas, monospace;
  word-break: break-all;
}

.note {
  margin-top: 16px;
  line-height: 1.55;
  font-size: 14px;
}

a { color: inherit; }

@media (max-width: 560px) {
  body { padding: 14px; }
  .panel { border-radius: 24px; padding: 16px; }
}
)rawliteral";

static const char kSetupJs[] = R"rawliteral(
function handleResponse(response) {
  if (response.ok) return response;
  return response.text().then((text) => {
    throw new Error(text || 'Request failed.');
  });
}

function legacyCopyTopic(topic, noteEl) {
  // navigator.clipboard requires a secure context (HTTPS or localhost), which this
  // page is not served over (the setup AP is plain HTTP). This older technique
  // still works over plain HTTP in most browsers: put the text in a temporary
  // textarea, select it, and use the deprecated but still-functional
  // execCommand('copy').
  const textarea = document.createElement('textarea');
  textarea.value = topic;
  textarea.setAttribute('readonly', '');
  textarea.style.position = 'fixed';
  textarea.style.left = '-9999px';
  document.body.appendChild(textarea);
  textarea.select();
  textarea.setSelectionRange(0, textarea.value.length);

  let copied = false;
  try {
    copied = document.execCommand('copy');
  } catch (err) {
    copied = false;
  }
  document.body.removeChild(textarea);

  if (noteEl) {
    noteEl.textContent = copied
      ? 'Copied. Paste this topic into the ntfy app.'
      : 'Automatic copy is not supported on this device. Press and hold the topic above to copy it manually.';
  }
}

function copyTopic() {
  const topicEl = document.getElementById('topicText');
  const noteEl = document.getElementById('copyNote');
  const topic = topicEl.textContent;

  if (!navigator.clipboard || !navigator.clipboard.writeText) {
    legacyCopyTopic(topic, noteEl);
    return;
  }

  navigator.clipboard.writeText(topic).then(() => {
    if (noteEl) noteEl.textContent = 'Copied. Paste this topic into the ntfy app.';
  }).catch(() => {
    legacyCopyTopic(topic, noteEl);
  });
}

function saveConfig() {
  const ssid = document.getElementById('ssidManual').value.trim();
  if (!ssid) {
    alert('Choose or type your Wi-Fi network name.');
    return;
  }

  const data = new URLSearchParams();
  data.append('ssid', ssid);
  data.append('pass', document.getElementById('pass').value);

  fetch('/save-config', {
    method: 'POST',
    headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: data.toString()
  })
    .then(handleResponse)
    .then(() => {
      document.body.innerHTML = '<div style="font-family:Avenir Next,Segoe UI,sans-serif;padding:24px;color:#332518;">Saved. Proof Monitor is restarting...</div>';
    })
    .catch((error) => alert(error.message || 'Could not save settings.'));
}

function sendAction(path) {
  fetch(path, { method: 'POST' })
    .then(handleResponse)
    .then(() => location.reload())
    .catch((error) => alert(error.message || 'Could not reach Proof Monitor.'));
}
)rawliteral";
