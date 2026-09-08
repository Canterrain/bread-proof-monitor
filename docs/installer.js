const status = document.getElementById("installer-status");
const button = document.getElementById("install-button");
const version = document.getElementById("version");

async function prepareInstaller() {
  if (!window.isSecureContext) {
    status.textContent = "Open the HTTPS version of this installer. For a local preview, use localhost.";
    return;
  }
  if (!("serial" in navigator)) {
    status.textContent = "USB installation needs Chrome, Edge, or a Firefox version with Web Serial support on a desktop or laptop. Safari and mobile browsers aren't supported.";
    return;
  }

  try {
    const response = await fetch("firmware/manifest.json", { cache: "no-store" });
    if (!response.ok) throw new Error("Firmware manifest unavailable");
    const manifest = await response.json();
    if (manifest.name !== "Bread Proof Monitor" || !manifest.version ||
        !manifest.builds?.some((build) => build.chipFamily === "ESP32" && build.parts?.length === 4)) {
      throw new Error("Firmware manifest is incomplete");
    }
    await import("https://unpkg.com/esp-web-tools@10.4.0/dist/web/install-button.js?module");
    await customElements.whenDefined("esp-web-install-button");
    version.textContent = "Firmware " + manifest.version;
    version.hidden = false;
    status.hidden = true;
    button.hidden = false;
  } catch (error) {
    status.textContent = "The installer couldn't load. Check your internet connection and reload this page. If it continues, the published firmware may be unavailable.";
    console.error(error);
  }
}

prepareInstaller();
