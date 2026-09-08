# Publishing on GitHub

[Back to the project](../README.md)

The repository contains the static installer and its matching firmware package. This guide covers publishing and maintaining them together.

## Project Addresses

- Repository: `Canterrain/bread-proof-monitor`
- Installer: `https://canterrain.github.io/bread-proof-monitor/`

When publishing a fork under another name, update the README and installer links. For source builds, clone into a folder named `bread_proof_monitor` or use the packaging script, which stages the sketch with that name.

## Release Checks

- Keep the README and installer links aligned with the repository name.
- Licensing follows the clock projects: CC BY-NC 4.0, © 2026 Josh Hendrickson.
- Run a USB browser installation on the intended ESP32, check both sensors, complete Wi-Fi setup, and verify an alert.
- Check an update with Erase device unchecked retains settings, and a fresh erased install starts setup correctly.

The local firmware build and package validation are useful checks, but they do not establish that USB installation has been tested on hardware.

## Repository Contents

Publish the firmware sources, tools, README, guides, `docs/` installer, and intended hardware files. The `.gitignore` excludes local build products, OS metadata, editor backups, and KiCad session/backup files. The intentional firmware under `docs/firmware/` is included.

Use the compiled release files, never a dump read from a personal device. The hardware directory includes design history alongside the finalized Rev A files; start with `hardware/kicad/rev-a/README.md` for the current design.

## Enable GitHub Pages

After the repository and its reviewed files are on GitHub:

1. Open **Settings → Pages**.
2. Under Build and deployment, choose **Deploy from a branch**.
3. Select **main** and **/docs**, then save.
4. Wait for the Pages deployment to complete and open the address GitHub provides.
5. Confirm the install page, `firmware/manifest.json`, and all four `.bin` files are served, then test the installer over USB.

The site is static HTML, CSS, and JavaScript, matching the approach used by Round Weather Display. `docs/.nojekyll` keeps Jekyll processing out of the deployment. A separate website framework or hosting service is not needed.

See [GitHub's publishing-source instructions](https://docs.github.com/en/pages/getting-started-with-github-pages/configuring-a-publishing-source-for-your-github-pages-site).

## Publish an Update

1. Prepare firmware with an explicit new version: `python3 tools/prepare_installer.py --version YOUR_VERSION`.
2. Test the firmware on hardware and inspect `docs/firmware/build-info.json` and `SHA256SUMS`.
3. Commit the source and its corresponding generated firmware together.
4. Push the reviewed change. Pages republishes the installer from `main` and `/docs`.

Do not package from a stale build directory. The preparation script always compiles first and refuses unexpected chip, partition, or flash-header settings. If a future version changes the partition layout, update the installer instructions to require an erase instead of promising saved-setting retention.

## Installer References

- [Round Weather Display installer source](https://github.com/Canterrain/round-weather-display/blob/main/docs/index.html)
- [ESP Web Tools documentation](https://esphome.github.io/esp-web-tools/)

Bread Proof Monitor uses its existing Wi-Fi access-point setup after installation. It does not implement Improv Serial provisioning, so the installer skips that optional step.
