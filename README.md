<div align="center">

# Xash-RT Improved

[![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/boofiboi/xash-rt/rt.yml?style=for-the-badge&logo=github)](https://github.com/boofiboi/xash-rt/actions/workflows/rt.yml)
[![Discord](https://img.shields.io/discord/1530690470194446395?color=5865F2&label=Discord&logo=discord&logoColor=white&style=for-the-badge)](https://discord.com/invite/6ueFjD6UTy)
[![GitHub Release](https://img.shields.io/github/v/release/boofiboi/xash-rt?style=for-the-badge&logo=github)](https://github.com/boofiboi/xash-rt/releases)
[![Downloads](https://img.shields.io/github/downloads/boofiboi/xash-rt/total?color=brightgreen&logo=github&style=for-the-badge)](https://github.com/boofiboi/xash-rt/releases)
<br>
![C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?logo=cplusplus&logoColor=white&style=for-the-badge)
[![Renderer](https://img.shields.io/badge/Renderer-RayTracedGL1-2E3440?logo=github&logoColor=white&style=for-the-badge)](https://github.com/boofiboi/RayTracedGL1)
</div>

Xash-RT Improved is a heavily modified fork of [xash-rt](https://github.com/sultim-t/xash-rt) (originally forked from [xash3d-fwgs](https://github.com/FWGS/xash3d-fwgs)). It modifies the OpenGL renderer to integrate a [custom path-traced renderer](https://github.com/boofiboi/RayTracedGL1), bringing modern graphics and upscaling technologies to the GoldSrc engine.

---

## Key Improvements

### Engine & Compatibility
* **Upstream Sync:** Fully synced with upstream `xash3d-fwgs`, enabling compatibility with a wider range of mods, including *Counter-Strike 1.6*.
* **Native Linux Builds:** Supported out of the box. *(Note: Native Linux builds are limited to FSR 3 due to the lack of `.so` libraries for FSR 4; running the Windows build via Wine/Proton unlocks FSR 4).*
* **Visual & QoL Fixes:** Significant performance optimizations, rendering bug fixes, and quality-of-life improvements over the original fork.

### Upscaling & Denoising
* **AMD FSR 4 Upscaling:** Runs on RX 6000+ series GPUs with comparable performance to FSR 3.1.6 while delivering substantially sharper visuals than a compute-only upscaler. RX 9000 series GPUs utilize the full-size FP8 model for maximum image quality. Uses FSR 3.1.6 for Frame Generation.
* **AMD FSR 3:** Full support, including Frame Generation.
* **NVIDIA DLSS 4.5:** Full support for both Upscaling and Frame Generation.
* **NVIDIA Real-Time Denoisers ([NRD](https://github.com/NVIDIA-RTX/NRD)):** Integrated for cleaner, higher-quality ray-traced denoising.

### Roadmap
* Track ongoing development and upcoming features in [rt-todo.md](https://github.com/boofiboi/xash-rt/blob/master/Documentation/rt-todo.md).

---

## Installation

1. **Stable Builds:** Download the latest compiled Half-Life 1 build from the [Releases](https://github.com/boofiboi/xash-rt/releases) tab.
2. **Development Builds:** Grab the latest CI build artifact directly from the [GitHub Actions](https://github.com/boofiboi/xash-rt/actions) tab.

---

## Issue Guidelines

Before opening an issue, ensure your problem is not already listed and follows the guidelines below. **Invalid reports will be closed immediately without discussion.**

### Automatically Closed Issues
* **Low-Effort Reports:** Issues stating *"it doesn't work"* with no description, logs, error messages, or system specifications.
* **Unsupported Hardware / Emulated RT:** Crashes or errors caused by running on a GPU without hardware ray-tracing support (including setups forcing `RADV_PERFTEST=emulate_rt`, even if it technically boots).

### How to Submit a Valid Report
1. **Search first** to confirm an identical issue does not already exist.
2. Keep the title and description concise.
3. Include the following details:
   * **Logs & Errors:** Exact error messages or crash logs.
   * **Behavior:** Expected behavior vs. actual behavior.
   * **System Specs:** GPU, CPU, OS, and driver version.
   * **Context:** Steps to reproduce or what you were doing when the crash occurred.

> If you are unsure whether your problem qualifies as a bug or need setup help, ask on the [Discord server](https://discord.com/invite/6ueFjD6UTy) instead of opening an issue.