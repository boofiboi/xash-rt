# Project Roadmap & TODO

Tasks for **Xash-RT Improved** and **RayTracedGL1**, ordered roughly by priority.

---

## Active & In-Progress

### Ray Tracing & Lighting Pipeline
* [ ] **Pre-Denoise Signal Cleanup:** Improve initial ray-tracing sampling so denoisers receive cleaner input data.
  * [x] ReSTIR GI
  * [x] Quasi-Monte Carlo (QMC) sampling
  * [ ] Linearly Transformed Cosines (LTC) for Half-Life 1 beam particles
* [ ] **Reduce Excessive Surface Light Bounce:** Fix default material light-bounce behavior where surfaces reflect far too much light.
  * **Automated PBR Estimation:** Generate roughness/normal maps automatically (e.g., via OpenCV script, GIMP, Laigter, or OpenBump). *(Note: Batch-baking BC7-compressed `.ktx2` files across all mods is slow—~20 minutes per 100 textures).*
  * **`texture.json` Overrides:** Introduce material presets (e.g., `Roughness`, `Concrete`) to govern light interaction without altering base albedo textures.
* [ ] **Quake/VRAD Polygon Light Estimation:** Find a reliable method to infer light sources from compiled BSP VRAD data (since BSP files strip original light entities) to automate gLTF scene generation for unmapped levels.

### Engine, Tooling & Scene Overhauls
* [ ] **Campaign Scene Rework:** Update base Half-Life 1 campaign scenes to take full advantage of the new renderer features.
* [ ] **RTGL1 Devmode Toggle / Build Target:** Replace the current devmode toggle with a proper cvar, launch argument, or separate non-dev build target to strip dev overhead and recover FPS.
* [ ] **Installation Manager & Launcher:** Build a lightweight .NET or Electron launcher to handle installation, updates, and compatibility notes.
* [ ] **Release Automation:** Create a one-click CI workflow that packages all required release files and generates user-friendly changelogs from recent commits.

---

## Deprioritized / Shelved

* [ ] **Platform-Specific ML Denoisers:**
  * ~~NVIDIA DLSS Ray Reconstruction~~
  * ~~AMD Ray Regeneration (RDNA 3+)~~
  * ~~Intel OIDN (Not worth maintaining for a tiny user base; also skipping [Snapdragon / Apple Silicon](https://youtu.be/h3lZXXO8qTk?t=15))~~
  * *Status:* The current NRD implementation already matches ML denoiser quality.

---

## Completed

### Upscaling & Frame Generation
* [x] **AMD FSR 4 Support:**
  * Implemented upscaling only (FSR 4 Frame Generation is locked to RX 9000 GPUs and skipped in favor of FSR 3.1.6 Frame Generation).
  * Linux builds currently fall back to FSR 3.1.6 until AMD releases native FSR 4 `.so` libraries.
* [x] **Unified Upscaler & Frame-Gen Stack:**
  * Consolidated cvars down to `rt_upscale_tech`, `rt_upscale_quality`, `rt_upscale_enabled`, and `rt_framegen` (removed `rt_framegen_tech`, `rt_upscale_fsr3`/`dlss`, and `_rt_fsr3_framegen_available`).
  * Standardized quality presets across FSR 3 and DLSS from a single code path, defaulting both upscaling and Frame Generation to OFF on first boot.
  * Restricted swapchain stack creation strictly to game boot (switching upscalers now prompts a restart instead of rebuilding mid-session).
* [x] **NVIDIA DLSS 4.5 Support:**
  * [x] Upscaling
  * [x] Frame Generation
  * [ ] ~~Ray Reconstruction~~ *(Moved to deprioritized)*

### Rendering, Assets & Platform Support
* [x] **Full Shader Refactor:** Resolved unstable/flickering bloom, broken lens flares, overpowering volumetrics, and inconsistent lighting.
* [x] **HD Model PBR Materials:** Added missing PBR textures for HD pack assets (Gauss Gun, Rocket Launcher, Revolver, MP5, Barney, HEV suit, etc.) to match LD model quality.
* [x] **Native Linux Builds:** Added first-class Linux build support.