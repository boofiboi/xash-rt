
# Xash-RT Improved

  

Xash-RT Improved is a heavily modified fork of [xash-rt](https://github.com/sultim-t/xash-rt) which in of itself is a fork of [xash3d-fwgs](https://github.com/FWGS/xash3d-fwgs) with edits to the OpenGL renderer to allow for integration with [a custom path traced renderer](https://github.com/boofiboi/RayTracedGL1) to add modern graphical improvements to the otherwise dated engine.

## Improvements over the original [fork](https://github.com/sultim-t/xash-rt)
- **Completely synced to upstream (Allows for running a wider-range of mods, including Counter-Strike: 1.6)**
- **FSR3 support (Including frame generation)**
- **Huge performance + visual improvements.**
- **Visual fixes compared to the original.**
- **Improved mod support.**
- **[NRD](https://github.com/NVIDIA-RTX/NRD) (Better denoiser)**
- **Various QoL features**
### In-progress:
- **Native linux builds**
- **Getting an actual release of the engine, including Blue Shift: Ray Traced**
- **Small app to handle updates/installation of the mod, and managing mod adaptations**
  
## Installation without building
Latest compiled build for Half-Life 1 can be found in the [Releases](https://github.com/boofiboi/xash-rt/releases) tab.
## Minor things
- We have a [Discord](https://discord.com/6ueFjD6UTy) server (Come discuss this mod, get support etc.)
- Read up on the original xash3d-fwgs repository [here](https://github.com/FWGS/xash3d-fwgs)
- Also read up on the original xash-rt fork [here](https://github.com/sultim-t/xash-rt)
- A new, fixed and functional release will be released during the month of september along with HL: Blue Shift support.
	- If you are really keen and want to try it out right now, build instructions are below.
- **DLSS will not be supported until/unless I can get someone with an nVidia GPU to test (I run an RX 7600)**

  

## Building & Installing

#### Requirements:

    git
    RayTracedGL1
    Vulkan SDK
    Python 3.11+
    Visual Studio 2026 (C++ desktop workload)
    A higher than room temperature IQ (Ability to understand instructions)
#### Instructions:
These are assuming you are running Windows, have a Visual Studio install with C++ Development Tools installed and know your way around.

1. **Install all requirements**
	- Download and install the *Vulkan SDK* from [here](https://vulkan.lunarg.com/) (Make sure to also tick `Add to PATH`)
	-  Download and install *Python* from [here](https://www.python.org/downloads/) (Make sure to tick `Add to PATH` during installation)
	-  Download and install *Visual Studio 2026* from [here](https://visualstudio.microsoft.com/downloads/) (Get the community version, after installing VS, install the `Desktop development package with C++` package
2. **Compile RTGL1**
	-  Open a `Developer Command Prompt for VS`
	-  Run the following commands:
	```
	cd C:\
	git clone https://github.com/boofiboi/RayTracedGL1 --recursive
	```
	-  Download and extract the FSR3 SDK from [here](https://gpuopen.com/fidelityfx-super-resolution-3/#:~:text=Download%20the%20latest%20version%20%2D%20v3.1.5) (Get the SDK 1.1 package)
	-  Extract the .zip file into `Source\FSR3`
	-  Run the following commands in the command prompt we opened:
	```
	cd RayTracedGL1
	
	cd Source/Shaders
	
	python GenerateShaders.py
	
	cd ../..
	
	cmake -B Build/x64-Release -S .  -A x64 -DCMAKE_BUILD_TYPE=Release
	
	cmake --build Build/x64-Release --config Release --parallel
	
	```
	-  You should now have `RayTracedGL1.dll, amd_fidelityfx_vk.dll, NRI.dll` in the `build\x64-Release\Release` folder and a bunch of `.comp.spv` files in `build/x64-Release/shaders` folder.
	- Leave them be, these will be automatically handled by the next steps.

3. **Set the `RTGL1_SDK_PATH` environment variable to point to the repository we cloned earlier**
	-  Open the start menu and search for `Edit the system environment variables`
	-  A menu will open, click the `Environment Variables` button
	-  Press `New` under the `System Variables` entries
	-  Set `Variable name` to `RTGL1_SDK_PATH`
	-  Set `Variable value` to where you cloned the repo (e.g: `C:\RayTracedGL1`)
4. **Compile xash-rt**
	- Open a regular command prompt window (**Xash-RT does not compile in a VS cmd prompt**)
	- Run the following commands:
	```
	git clone https://github.com/boofiboi/xash-rt --recursive
	cd xash-rt
	```
	- Download *SDL2* from [here](https://github.com/libsdl-org/SDL/releases?page=3#release-release-2.32.8) - Download the `sdl2-devel-2.32.8-vc.zip` file
	- Extract the SDL2 SDK into either:
		- Any folder on your machine: Requires passing `-s=SDL2_PATH` to WAF
		- Extract it into a folder called `SDL2_VC` the xash-rt directory we cloned earlier: This is the best option, WAF will automatically grab it.
	- Run the following commands:
	```
	waf configure -8 --enable-raytracing -s=C:/PATH_TO_SDK_OR_LEAVE_EMPTY --enable-lto --enable-poly-opt -T Release
	
	waf build 
	```
	- **After a while, WAF should finish and you should have recieved the engine files in `build/`**
		- Run `waf install --destdir PATH_TO_YOUR_GAME_INSTALL` (e.g `waf install --destdir D:\HalfLifeRT\` [Its recommended to copy the entire game folder somewhere else so you dont mess up your stock Steam install]
5. **Download the original `hl1-rt-patched.zip` file from the [original repository](https://github.com/sultim-t/xash-rt/releases)**
	- Extract everything EXCEPT the `rt_bin` folder into your game folder (Replace if prompted)
6. **Download the `hl1-rt-nobin.zip` file from [this fork](https://github.com/boofiboi/xash-rt/releases)**
	- Extract the files from this archive into your game folder
7. **Your game folder should now look like this**
	```
	Half-Life
	├── htmlcache
	├── platform
	├── redist
	├── rt
	│	├── data
	│		└── textures.json
	│	├── mat
	│	├── scenes
	│	├── shaders
		│	├── CmAntiFirefly.comp.spv
		│	└── CmASVGFGradientAtrous.comp.spv
	│	└── valve
	│		└── cl_dlls
	│			└── client_amd64.dll
	├── rt_bin
	│	├── amd_fidelityfx_vk.dll
	│	├── filesystem_stdio.dll
	│	├── menu.dll
	│	├── NRI.dll
	│	├── RayTracedGL1.dll
	│	├── ref_gl.dll
	│	├── xash.dll
	│	└── xash3d.exe
	├── valve
	├── hl.exe
	├── hlds.exe
	```
8. **Run `rt_bin/xash3d.exe`**
	- Do NOT run `hl.exe` from the root of the game directory - this is the old engine.

## Issues
Issues that are similar to these will be automatically closed without hesitation:
- It doesnt work
	- No information provided on what is happening
	- No logs/crash/error messages
	- No system specifications provided
	- 0 actual value
- Doesnt run (Using unsupported setup)
	- Contains error message that signifies the person is running a GPU without RT support
	- Using `RADV_PERFTEST=emulate_rt` also falls under this category, although it should work.
	- These will be automatically and without debate closed.
- **Correct way to report an issue**
	- ***Check if an issue does not already exist.***
	- Keep it short and simple
	- Provide:
		- Error message/logs
		- Expected behaviour vs actual behaviour
		- System specs
		- Any extra information (What you were doing when it happened etc.)
	- We also have a [Discord](https://discord.com/6ueFjD6UTy) server, if youre not comfortable with creating an issue.
