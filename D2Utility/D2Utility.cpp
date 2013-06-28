#include "Patch.h"
#include "D2Utility.h"

// Initial Patches to the D2 process
Patch* patches[] = {
    NULL,
    new Patch(Jmp, D2CLIENT, 0x6D703, 0x23, 6), // Display Life Always
};

// Performs all the patches to the process
BOOL D2Utility::Start(HINSTANCE hInstance, LPVOID lpvReserved)
{
    // Installs the patches to the process
    return TRUE;
}

// Removes all the patches to the process
BOOL D2Utility::Stop()
{
    // Cleans up the patches made to the process
    return TRUE;
}