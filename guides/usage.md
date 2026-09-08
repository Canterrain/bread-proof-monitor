# Using Bread Proof Monitor

[Back to the project](../README.md)

## Calibration and Starting a Proof

Apply a profile first. Calibrate with the sensor, container, and lid positioned exactly as they will be during monitoring:

1. With the container empty, press **Set Empty Setup**.
2. Add the dough, smooth the top, and replace the lid.
3. Let it settle, then press **Set Starting Dough Height**.

The monitor takes five valid distance samples for calibration and uses their median. If it cannot capture a reading, check the sensor and setup, then try again. Complete any preparation steps from the linked recipe before starting the monitored stage.

Changing to a different profile clears the active run and empty calibration, even when both recipes use the Bulk stage. Starting another proof with **Start New Proof** keeps the current profile, stage, target, and empty calibration. Set a fresh empty calibration yourself if the container or sensor position has changed.

## Reading the Dashboard

The dashboard refreshes every 15 seconds. Sensors are normally read about every two seconds, so phone alerts can arrive before the next dashboard refresh.

Rise is calculated from dough height relative to the calibrated container. For profiles with folds, displayed progress includes progress carried forward from earlier segments. It is a guide to the monitored process, not a direct measurement of dough volume.

The distance reading is filtered using up to three valid samples and smoothing. A higher progress reading must pass an additional confirmation period before it becomes the retained peak used for fold and target decisions.

The monitor also enforces a minimum time for each segment. At 75°F this is generally 20 minutes, or 10 minutes for Focaccia. With a working temperature sensor, it scales that time within an 8–40 minute range. The sensor measures **ambient** temperature; it isn't a probe inside the dough. Paused time is excluded.

You may see **Confirming target reached** while the displayed percentage is already at the target. The monitor is waiting for its confirmation conditions. These checks help with noise; you should still judge the dough before finishing.

## Folds and Other Handling Steps

When the current step calls for a fold:

1. Press **Pause Monitoring** before removing the lid.
2. Perform the fold and replace the lid.
3. Press **Resume Monitoring** and wait for readings to settle.
4. Press **Complete Step** when it becomes available.

Completing a step captures a new baseline while carrying forward retained progress. A new segment timer starts for the next milestone.

Distance readings stop during a pause, and there is a six-second settling delay after resuming. Leaving the lid off while running can cause a false reading.

## Re-baselining

Use **Re-baseline After Lid Off** after an unscheduled check or repositioning. It captures a new segment baseline and keeps the retained progress. It also restarts the segment's minimum waiting period.

Pause before handling the lid, resume when it is back, and allow readings to settle before re-baselining. Re-baselining preserves previously retained progress; it does not undo a false peak already accepted while the lid was off.

## Finishing and Final Proof

At the target, check how the dough looks and feels. **Finish Proof** records the finished result and queues feedback for that stage.

For a Bulk profile with a monitored Final stage, finishing prepares Final proof with its learned target. Most such profiles clear the empty calibration because the shaped dough uses a different setup. Follow the dashboard prompts to calibrate and capture a new starting height.

For a profile without monitored Final proof, follow the linked recipe for the remaining rests, shaping, and baking. Finishing stops distance readings and freezes the result for that stage unless the monitor moves on to preparing Final.

## Outcome Feedback

Rate the dough as it was when the stage ended, rather than judging the finished bake. Oven temperature and bake time can change the result without saying anything about the proof target.

Feedback is tracked separately for each recipe and stage:

- Two consecutive **Underproofed** ratings add five percentage points.
- Two consecutive **Overproofed** ratings subtract five percentage points.
- **Just Right** resets the streak without changing the learned adjustment.
- Switching from under to over, or over to under, starts a new streak.
- Adjustments are limited to ±25 points from the shipped suggestion, with overall target limits.

There are two pending-feedback slots, allowing Bulk and Final from one batch to wait together. Answer them before accumulating more finished stages across subsequent batches.

Submitting a rating or resetting learning reloads the dashboard so the profile list is refreshed. Your active proof keeps its applied target. To use another suggested target, select the profile/stage and apply it.

## Saved Settings and Restarts

The device saves calibration, selected profile and stage, targets, fold state, notification state, and pending feedback. Learned adjustments are stored separately on the same device.

During a proof, elapsed time and progress are saved periodically, normally once a minute. A reboot resumes from saved state; powered-off time is not counted, and the most recent unsaved progress may be lost.

**Start New Proof** clears the active run, timers, fold progress, final result, and alert state. It keeps calibration, profile, stage, target, Wi-Fi, ntfy topic, and learning.

A full flash erase clears all device settings, including the generated ntfy topic. Reconnect through **ProofMonitor-Setup**, complete setup, and subscribe to the new topic.

## Troubleshooting

- **No setup network:** a monitor with working saved Wi-Fi joins that network instead. Check your router or try [proofmonitor.local](http://proofmonitor.local).
- **The setup network has no internet:** that is expected. Stay connected long enough to open [192.168.4.1](http://192.168.4.1) and save your home Wi-Fi details.
- **The dashboard address does not open:** use the device's LAN IP from your router. Serial logs also show the IP after connection.
- **Distance readings are stale or unstable:** check the sensor connections, optical path, and lid position. Fresh readings are needed for step completion and re-baselining.
- **Rise jumps after moving the lid:** pause before handling it. A close, rigid mount and a stable dough surface make readings easier to interpret.
- **No phone alert:** check the internet connection, topic subscription, and the dashboard's test notification action.

The **Calibration Test** profile is available in Advanced for sensor checks. It uses the normal confirmation and timing behavior.
