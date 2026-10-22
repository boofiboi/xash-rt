# TODO list for xash-rt and RayTracedGL1

- [ ] DLSS4.5 support

	- 100% Upscaling + FG, RR is secondary.

- [ ] FSR4 DX12 interop

	- Only for RX 9xxx series, the FP8 model looks incredibly bad and performs like ass, better to use FSR3.1 on older GPUs

	- Might also be worth it to wait for the official VK version of FSR4, might fix all the issues above.

- [ ] Platform specific denoisers
	- DLSS Ray Reconstruction for nVidia
	- AMD SSSR for AMD 
	- NRD or A-SVGF for other platforms (Intel, [*Snapdragon?!??!?! Apple Silicon?!?!?!?!*](https://youtu.be/h3lZXXO8qTk?t=15))

- [ ] Update the ray tracing shaders so denoisers already get semi-clean data to work off of.
    - I havent done much investigation into this, but there probably are newer and better equations for ray-casting and ray retrieval --> Converting to data the denoiser uses to actually paint lighting on the 3D world.

- [ ] **STOP** RTGL1 bouncing light so much (I dont get it, why does every surface bounce light so much by default?????)
    - PBR Estimation?
        - PBR is so old there is hundreds if not thousands of very good ways of doing this automatically (GIMP is a very good example of this, can create normal and roughness maps on even 128x128 images.)

- [ ] Linux native builds

- [ ] Launcher app for updating/managing the installation.

- [ ] Finding a better way of estimating Quake polygon lights (Yes, **QUAKE** polygon lights, HL1 inherited those with VRAD)
    - This mainly helps with automatic gLTF scene creation when loading a map that does not have one.
    - This will hopefully obsolete manual scene creation and editing in the future 🤞 
