# TODO list for xash-rt and RayTracedGL1, ordered by importance.

- [ ] DLSS4.5 support

	- 100% Upscaling + FG, RR is secondary.

- [ ] Update the ray tracing shaders so denoisers already get semi-clean data to work off of
    - I havent done much investigation into this, but there probably are newer and better equations for ray-casting and ray retrieval --> Converting to data the denoiser uses to actually paint lighting on the 3D world.

- [ ] Take an actual deep dive into the shaders to see how much they have deviated from the original and what im doing wrong
    - The original HL1 campaign STILL does not look right on this fork, tech debt caught up.
        - Blue Shift and Opposing Force are fine because I have made those in this new fork and all of its quirks, the HL1 campaign is from the old engine.

- [ ] Platform specific denoisers
	- DLSS Ray Reconstruction for nVidia
	- AMD SSSR for AMD 
	- NRD or A-SVGF for other platforms (Intel, [*Snapdragon?!??!?! Apple Silicon?!?!?!?!*](https://youtu.be/h3lZXXO8qTk?t=15))

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
	- Only for RX 9xxx series, the FP8 model looks incredibly bad and performs like ass, better to use FSR3.1 on older GPUs
	- Might also be worth it to wait for the official VK version of FSR4, might fix all the issues above.

- [ ] Linux native builds

- [ ] Launcher app for updating/managing the installation


