//
// PiCamLite Embedded functions
// (C) FynnleyNeko, 2025
//

#pragma once
#include <vector>

#ifdef PICAMEMBEDDED_EXPORTS
#define PICAMEMBEDDED_API __declspec(dllexport)
#else
#define PICAMEMBEDDED_API __declspec(dllimport)
#endif

// Hardware control functions

// Return: 0 -> ok, 1 -> called while busy, 2 -> error
extern "C" PICAMEMBEDDED_API int PCLE_Start();          // expected to take 3 seconds
extern "C" PICAMEMBEDDED_API int PCLE_Restart();        // expected to take 5 seconds, usually doesn't fix hardware freezes by itself, if you detect a frozen camera feed, instruct the user to replug the Droolon and then call restart()
extern "C" PICAMEMBEDDED_API int PCLE_Stop();           // near instant
extern "C" PICAMEMBEDDED_API bool PCLE_CheckBusy();     // Returns true if the device is currently busy and false you can send another start/restart/stop command

typedef void (*FrameCallback)(BYTE* leftBits, BYTE* rightBits, int size);				// BGRA byte arrays (still a monochrome image with no alpha, but handing these out raw avoids double conversion) !! COPY IT, THE DATA IS OVERRIDDEN THE INSTANT THE NEXT FRAME IS CAPTURED !!
extern "C" PICAMEMBEDDED_API int PCLE_RegisterFrameCallback(FrameCallback callback);    // Register said callback to send our frames when we get them

extern "C" PICAMEMBEDDED_API int PCLE_SetFramerate(int framerate);                      // Requests the input framerate and returns the actual framerate it's capable of getting for you, no restart needed
extern "C" PICAMEMBEDDED_API int PCLE_SetExposure(int exposurePercent);		            // Waits for busy state to clear, changes exposure to percentage (clipped to safe range automatically), then calls restart() and returns restart()s return code
extern "C" PICAMEMBEDDED_API int PCLE_SetResolution(bool highResolutionWanted);         // Waits for busy state to clear, toggles between false -> lowres (320x240) and true -> highres (640x480), then calls restart() and returns restart()s return code

// Information functions, some are currently hardcoded, but you are advised to use them
// in case PiCamLite changes behaviour with updates, which is very likely

extern "C" PICAMEMBEDDED_API int PCLE_GetCurExposure();                                 // Returns the current exposure setting
extern "C" PICAMEMBEDDED_API int PCLE_GetCurMinExposure();                              // Currently hardcoded 50 (%), in future returns the minimum possible exposure for the current video mode
extern "C" PICAMEMBEDDED_API int PCLE_GetMinExposure(bool highResolutionWanted);        // Returns the minimum exposure but for  specific video mode
extern "C" PICAMEMBEDDED_API int PCLE_GetCurMaxExposure();                              // Currently hardcoded 130 (%), in the future returns the maximum possible exposure for the current video mode
extern "C" PICAMEMBEDDED_API int PCLE_GetMaxExposure(bool highResolutionWanted);        // Returns the maximum exposure but for a specific video mode 

extern "C" PICAMEMBEDDED_API int PCLE_GetCurFramerate();                                // Returns the current framerate that is being used
extern "C" PICAMEMBEDDED_API int PCLE_GetCurMaxFramerate();                             // Returns the current maximum possible framerate
extern "C" PICAMEMBEDDED_API int PCLE_GetMaxFramerate(bool highResolutionWanted);       // Returns the maximum possible framerate for a specific video mode

extern "C" PICAMEMBEDDED_API int PCLE_GetCurVideoWidth();                               // Returns the current video width
extern "C" PICAMEMBEDDED_API int PCLE_GetVideoWidth(bool highResolutionWanted);         // Returns the video height for a specific video mode

extern "C" PICAMEMBEDDED_API int PCLE_GetCurVideoHeight();                              // Returns the current video width
extern "C" PICAMEMBEDDED_API int PCLE_GetVideoHeight(bool highResolutionWanted);        // Returns the video height for a specific video mode