# PiCamEmbedded - Droolon Pi 1 camera access library
Droolon Pi 1 camera access library that unlocks the DRM, initializes the tracker and enables lightweight access to the images via a callback that sends pointers to byte arrays.

## Disclaimer
**Your tracker includes a self-bricking function. PiCamLite uses knowledge gained from decompilation to use gutted official functions to prevent triggering this protection. I've tested this for hours on my own hardware, HOWEVER I'm not responsible for bricked trackers!**

## Usage
**For normal users:**
You were probably searching for [PiCamLite](https://github.com/FynnleyNeko/PiCamLite/releases/latest), which is the end-user variant this project here is based on!

Otherwise welcome to the library part of the project! Most usage is pretty clear cut and explained within the headers file, but here is a copy for a quick skim:

| function | return | exec time | explanation |
| --- | --- | --- | --- |
| PCLE_Start() | int (Ok: 0, Busy: 1, Fail: 2) | initial: 5s, subsequent: 2s | Starts the tracker |
| PCLE_Restart() | int (Ok: 0, Busy: 1, Fail: 2) | 5s | Restarts the tracker. Usually doesn't fix freeze frames, but you can instruct the user to replug the hardware and then afterwards run restart to resume operation. |
| PCLE_Stop() | int (Ok: 0, Busy: 1, Fail: 2) | instant | Stops the tracker |
| PCLE_CheckBusy() | bool | instant | Checks if you can run another control command, a previous function may return but leave the tracker in a "cooldown" for a few more seconds to avoid a nasty bug in the firmware |
| PCLE_RegisterFrameCallback(FrameCallback callback) | int (Ok: 0, Busy: 1, Fail: 2) | 1s | Register a callback that gets you pointers to each sides frame in byte array format and the accompanying size, please copy before use |
| PCLE_SetFramerate(int framerate) | int | instant | Requests the input framerate and returns the actual framerate it's capable of getting for you, no restart needed |
| PCLE_SetExposure(int exposurePercent) | int (Ok: 0, Busy: 1, Fail: 2) | 5s | Waits for busy state to clear, changes exposure to percentage (clipped to safe range automatically), then calls restart() and returns restart()s return code |
| PCLE_SetResolution(bool highResolutionWanted) | int (Ok: 0, Busy: 1, Fail: 2) | 5s | Waits for busy state to clear, toggles between false -> lowres (320x240) and true -> highres (640x480), then calls restart() and returns restart()s return code |
| PCLE_GetCurExposure() | int | instant | Returns the current exposure setting |
| PCLE_GetCurMinExposure() | int | instant | Currently hardcoded 50 (%), in future returns the minimum possible exposure for the current video mode |
| PCLE_GetMinExposure(bool highResolutionWanted) | int | instant | Returns the minimum exposure but for  specific video mode |
| PCLE_GetCurMaxExposure() | int | instant | Currently hardcoded 130 (%), in the future returns the maximum possible exposure for the current video mode |
| PCLE_GetMaxExposure(bool highResolutionWanted) | int | instant | Returns the maximum exposure but for a specific video mode |
| PCLE_GetCurFramerate() | int | instant | Returns the current framerate that is being used |
| PCLE_GetCurMaxFramerate() | int | instant | Returns the current maximum possible framerate |
| PCLE_GetMaxFramerate(bool highResolutionWanted) | int | instant | Returns the maximum possible framerate for a specific video mode |
| PCLE_GetCurVideoWidth() | int | instant | Returns the current video width |
| PCLE_GetVideoWidth(bool highResolutionWanted) | int | instant | Returns the video height for a specific video mode |
| PCLE_GetCurVideoHeight() | int | instant | Returns the current video width |
| PCLE_GetVideoHeight(bool highResolutionWanted) | int | instant | Returns the video height for a specific video mode |

## Frequently Asked Questions
### What is this for?
>This is a library based on my previous work, PiCamLite, it allows you to start the tracker, control it's settings and get the video data completely integrated in your app. **If you didn't catch it from the license: You are fully permitted to bundle and ship this library in any form you may want as long as there is credit included.** This should allow you to integrate the video features natively into your app, making it so that users don't need to run PiCamLite and then select it's MJPEG streams in your app.<br>
This library runs none of 7invensuns algorithms and thus also doesn't output any tracking values, it purely allows access to the video feeds using certain parts of the original runtimes functions to initialize the hardware and get through integrity checks/DRM. *That's why there is some files you might recognize the names of, even if they are by far smaller than the official files*

### Why not open source?
>**Most of it was created using decompilation with Ghidra**, so it's using a lot of stuff even I don't have the source to. But most importantly I can't in good conscience publish code that might eventually be able to circumvent 7invensuns code for trackers that are not year old abandonware. *Please feel free to run the app through scanners, have fun with Ghidra yourself etc, I know precompiled binaries are sketchy and I wish I could share stuff!*

### How big is the benefit?
>**In my usecase with a 9800X3D it dropped a whole 12% off my CPU usage (an entire core)** to not run the official bad inference models. RAM usage dropped by around 300-400 MB, but because runtime.exe is very variable in use it could be less or more (it probably has a memory hole somewhere?).

### What is lost over runtime.exe?
>**Every single official tracker feature** which basically amounts to you no longer having access to the absolutely rancid dynamic foveation that wasn't fast enough anyways and direct access to tracker values (which also weren't great... or even working in 80% of cases). *But hey you gain the fancy 640x480 mode, that should also count for something right?*
