#include <windows.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include "../include/PiCamEmbedded.h"

int returnvalue = 2;
int framecount = 0;
int arraysize;
BYTE* left;
BYTE* right;

// We don't actually check data itself here, just if the thing works
void callback(BYTE* leftBits, BYTE* rightBits, int size) {
    // We are getting a pointer to a rapidly changing frame.
    // We should grab a clean copy in case we can't keep pace
    // with the tracker, not gonna happen, because we don't do
    // anything but you get the point. YOU should do it.
    left = new BYTE[size];
    right = new BYTE[size];
    (*left) = (*leftBits);
    (*right) = (*rightBits);

    arraysize = size;

    // We didn't use it, but we still need to clean it up, nobody likes memory leaks
    delete[] left;
    delete[] right;
    left = nullptr;
    right = nullptr;

    framecount++;
}

// This prints 1 [Busy] per second and continues after
void waitForBusyClear() {
    printf("Waiting for busy condition to clear... ");
    while(PCLE_CheckBusy()) {
        printf("[Busy] ");
        Sleep(1000);
    }
    printf("\n");
}

// Prints a whole bunch of info, after busy state is cleared, this ensures values are all updated already
void printAllTheInfo() {
    waitForBusyClear();

    // Add a label for lowres and highres mode
    printf("Getting resolution... ");
    int width = PCLE_GetCurVideoWidth();
    int height = PCLE_GetCurVideoHeight();
    if (width < 480) {
        printf("[LOWRES] ");
    }
    else {
        printf("[HIGHRES] ");
    }

    // Print the exact resolution
    printf("[");
    printf(std::to_string(width).c_str());
    printf("x");
    printf(std::to_string(height).c_str());
    printf("]\n");

    // Print current and max framerate
    printf("Getting framerate... ");
    returnvalue = PCLE_GetCurFramerate();
    printf("[");
    printf(std::to_string(returnvalue).c_str());
    printf("FPS]");
    returnvalue = PCLE_GetCurMaxFramerate();
    printf(" Max: [");
    printf(std::to_string(returnvalue).c_str());
    printf("FPS]\n");

    // Print current, minimum and maximum exposure
    printf("Getting exposure... ");
    returnvalue = PCLE_GetCurExposure();
    printf("[");
    printf(std::to_string(returnvalue).c_str());
    printf("]");
    returnvalue = PCLE_GetCurMinExposure();
    printf(" Min: [");
    printf(std::to_string(returnvalue).c_str());
    printf("]");
    returnvalue = PCLE_GetCurMaxExposure();
    printf(" Max: [");
    printf(std::to_string(returnvalue).c_str());
    printf("]\n");

    // Purely cosmetic wait
    Sleep(1000);
}

void framerateTest(int requested) {
    // Set target framerate and reset the counter
    PCLE_SetFramerate(requested);
    framecount = 0;
    printf("Measuring framerate over 30 seconds... ");
    // Wait for the time of measurement
    Sleep(30000);
    // Calculate real framerate and print comparison with requested framerate
    float framerate = framecount / 30.0f;
    printf("[");
    printf(std::to_string(framerate).c_str());
    printf("FPS]");
    printf(" Expected: [");
    printf(std::to_string(PCLE_GetCurFramerate()).c_str());
    printf("FPS]\n");
}

int main()
{
    // I don't think this is necessary in general, but if fixes running this inside Visual Studio for me
    wchar_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::filesystem::path cwd(buffer);
    cwd = cwd.parent_path();
    std::filesystem::current_path(cwd);

    printf("--- Begin testing PCLE ---\n\n");

    // Make sure we are requesting a start in lowres mode, because this triggers the more complex
    // startup sequence that is WAY more likely to misbehave
    WritePrivateProfileStringA("EMBEDDED", "highres", "0", (cwd / "PiCamEmbedded/config.ini").string().c_str());
    WritePrivateProfileStringA("EMBEDDED", "framerate", "100", (cwd / "PiCamEmbedded/config.ini").string().c_str());
    WritePrivateProfileStringA("EMBEDDED", "exposure", "100", (cwd / "PiCamEmbedded / config.ini").string().c_str());

    // Request the startup and see how it goes, also print info so we can check
    printf("--- Initial start ---\n");
    printf("Starting tracker... ");
    returnvalue = PCLE_Start();
    if (returnvalue == 0) {
        printf("[Success]\n");
    }
    else {
        printf("[FAIL]\n");
        exit(0);
    }
    printAllTheInfo();

    // After we survived the lowres startup, we still need to see if highres actually works,
    // even tho it is technically already part of the lowres startup routine (for memory alloc reasons)
    printf("\n--- Resolution change ---\n");
    printf("Changing resolution... ");
    returnvalue = PCLE_SetResolution(true);
    if (returnvalue == 0) {
        printf("[Success]\n");
    }
    else {
        printf("[FAIL]\n");
        exit(0);
    }
    printAllTheInfo();

    // While in highres mode we test if we can restart the tracker and up the exposure
    // this can sometimes randomly fail thanks to the firmware having bugs in highres mode
    // there is no way we can fix this, except pray and give it some time between commands
    // this checks if I gave it enough time in PiCamEmbedded.dll, if it crashes, I didn't
    printf("\n--- Exposure change ---\n");
    printf("Changing exposure... ");
    returnvalue = PCLE_SetExposure(PCLE_GetCurMaxExposure());
    if (returnvalue == 0) {
        printf("[Success]\n");
    }
    else {
        printf("[FAIL]\n");
        exit(0);
    }
    printAllTheInfo();

    // Now we need to issue a stop command and ensure the tracker is back in it's default config
    // for the next tests, because we are going to check framerates over the entire spectrum.
    // 
    // You can use this technique to change multiple settings at once or to push config changes
    // externally (for example update them with your apps updater, even without launching it first).
    // First issue a stop if it is running, change the config values and then issue a start.
    printf("\n--- Manual stop ---\n");
    printf("Stopping tracker... ");
    returnvalue = PCLE_Stop();
    if (returnvalue == 0) {
        printf("[Success]\n");
    }
    else {
        printf("[FAIL]\n");
        exit(0);
    }
    waitForBusyClear();
    printf("Externally modifying settings for frame tests... \n");
    WritePrivateProfileStringA("EMBEDDED", "highres", "0", (cwd / "PiCamEmbedded/config.ini").string().c_str());
    WritePrivateProfileStringA("EMBEDDED", "framerate", "100", (cwd / "PiCamEmbedded/config.ini").string().c_str());
    WritePrivateProfileStringA("EMBEDDED", "exposure", "100", (cwd / "PiCamEmbedded/config.ini").string().c_str());
    Sleep(1000);

    // With the config values set for the test we can now bring the tracker back online
    printf("\n--- Manual start ---\n");
    printf("Starting tracker... ");
    returnvalue = PCLE_Start();
    if (returnvalue == 0) {
        printf("[Success]\n");
    }
    else {
        printf("[FAIL]\n");
        exit(0);
    }
    printAllTheInfo();

    // The fun starts here, we test if the callback function works and that we can get the framerates we want
    // also a great time to check the CPU impacts of PiCamEmbedded and to test for memory leaks
    printf("\n--- Frame testing ---\n");
    printf("Registering callback... ");
    returnvalue = PCLE_RegisterFrameCallback(callback);
    if (returnvalue == 0) {
        printf("[Success]\n");
    }
    else {
        printf("[FAIL]\n");
        exit(0);
    }
    Sleep(2000); // The callback needs about 1 second to set up and start sending stuff, so we give it 2

    printf("Array size: [");
    printf(std::to_string(arraysize).c_str());
    printf("]\n");
    framerateTest(100);
    framerateTest(60);
    framerateTest(30);
    framerateTest(15);
    Sleep(1000);

    // Now do the entire thing over in highres mode to see if the callback is remembered and switches resolution
    printf("Testing callback survival across restarts and resolution change...\n");
    printf("Restarting tracker... ");
    returnvalue = PCLE_SetResolution(true);
    if (returnvalue == 0) {
        printf("[Success]\n");
    }
    else {
        printf("[FAIL]\n");
        exit(0);
    }
    Sleep(2000); // The callback needs about 1 second to set up and start sending stuff, so we give it 2

    printf("Array size: [");
    printf(std::to_string(arraysize).c_str());
    printf("]\n");
    framerateTest(100); // We are not going to reach 100, but this tests if the auto-limiting works
    framerateTest(15);
    Sleep(1000);

    printf("\nAll tests complete!");

    // Reset settings again, not needed, but clean
    WritePrivateProfileStringA("EMBEDDED", "highres", "0", (cwd / "PiCamEmbedded/config.ini").string().c_str());
    WritePrivateProfileStringA("EMBEDDED", "framerate", "100", (cwd / "PiCamEmbedded/config.ini").string().c_str());
    WritePrivateProfileStringA("EMBEDDED", "exposure", "100", (cwd / "PiCamEmbedded/config.ini").string().c_str());

    PCLE_Stop();
    exit(0);
}
