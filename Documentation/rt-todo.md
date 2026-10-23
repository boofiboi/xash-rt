# TODO list for xash-rt and RayTracedGL1, ordered by importance.

- [x] DLSS4.5 support

	- 100% Upscaling + FG, RR is secondary.

- [ ] Update the ray tracing calculations so denoisers already get semi-clean data to work off of
    - I havent done much investigation into this, but there probably are newer and better equations for ray-casting and ray retrieval --> Cleaner data for the denoiser --> Cleaner output and better experience.
    - Obsoleted by a very deep refactor of RTGL1.

- [ ] Get a better way to toggle RTGL1 devmode, maybe a cvar or launch argument or a dev/non-dev build
    - Potential FPS savings for non-dev builds because devmode does take quite a lot of FPS to run and a non-dev build could have all of the dev mode functionality stripped out.

- [ ] Do a deep dive code audit to see how much I have deviated from the original and what im doing wrong
    - The original HL1 campaign STILL does not look right on this fork, tech debt caught up.
        - Blue Shift and Opposing Force are fine because I have made those in this new fork and all of its quirks, the HL1 campaign is from the old engine.
    - This was/is partly caused by wildly different default config values for bloom, EV100 (Exposure), Saturation etc. 

- [ ] Launcher app for updating/managing the installation
    - atp this project is so bloated and installing it is such a mess I might just whip up a simple .net or Electron app to manage installations.
    - Would include an updater, detailed writeups of what works what doesnt etc.
    
- [ ] Platform specific denoisers
	- DLSS Ray Reconstruction for nVidia
	- AMD Ray Regeneration for AMD RDNA3+ - NRD fallback
   		- Might also just drop RDNA2 support, its an old dying platform.
      	- Lower end RDNA3 cards struggle with the WMMA model, better to use NRD on them. The option will be there though, for the masochists who enjoy playing at 30 FPS.
      	- AMD RR is also DX12 only like its cousin FSR4 - This will require waiting for the VK release as passing every bit of data a denoiser needs through a DX12 interop would kill performance.
	- NRD for other platforms (Intel, [*Snapdragon?!??!?! Apple Silicon?!?!?!?!*](https://youtu.be/h3lZXXO8qTk?t=15))
		- I am not writing OIDN code for the 4 Intel GPU users in the world.

- [ ] **STOP** RTGL1 bouncing light so much (I dont get it, why does every surface bounce light so much by default?????)
    - PBR Estimation?
        - PBR is so old there is hundreds if not thousands of very good ways of doing this automatically (GIMP is a very good example of this, can create normal and roughness maps on even 128x128 images.)
        - Or just run a one-time algo that will do this for EVERY material in the game, this however is a lot of work requiring export of EVERY material in EVERY HL1 mod and then building all the BXT7 compressed .ktx2 files for them (100 textures took 20 minutes to bake...)
    - texture.json overrides?
        - Options like Roughness, Concrete etc. that dont affect the look of the material but affect how light interacts.

- [ ] Finding a better way of estimating Quake polygon lights (Yes, **QUAKE** polygon lights, HL1 inherited those with VRAD)
    - BSP files dont contain light entity data, only the VRAD data. Genuinely 0 clue on what to do here.
    - This mainly helps with automatic gLTF scene creation when loading a map that does not have one.
    - This will hopefully obsolete manual scene creation and editing in the future 🤞 

- [ ] PBR materials for HD models
    - The Gauss Gun, Rocket Launcher, Barney model, HEV model, Revolver, MP5 and all of that do not have PBR textures and therefore dont get the same visual flair as the LD models.

- [ ] FSR4 DX12 interop
	- Only for RX 9xxx series, the FP8 WMMA model looks incredibly bad and performs like ass, better to use FSR3.1 on older GPUs
   		- In Call Of Duty: Modern Warfare 4, FSR4 performed insanely bad on my RX 7600 (FSR3.1 Balanced - 150 FPS versus FSR4 Balanced - 50 FPS)
	- Might also be worth it to wait for the official VK version of FSR4, might fix all the issues above.

- [ ] Linux native builds




