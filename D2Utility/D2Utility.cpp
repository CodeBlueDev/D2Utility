#include "D2Utility.h"
#include "Patch.h"

// Initial Patches to the D2 process
Patch* patches[] = {
    new Patch(D2CLIENT, (DWORD)0x6D703, (DWORD)0x23EB, 2), // Display Life Always
};

// Performs all the initial patches to the process
BOOL D2Utility::Start(HINSTANCE hInstance, LPVOID lpvReserved)
{
    // Installs the patches to the process
    for(int i = 0; i < (sizeof(patches) / sizeof(Patch*)); i++)
    {
        patches[i]->Install();
    }
    return TRUE;
}

// Removes all the patches to the process
BOOL D2Utility::Stop()
{
    // Cleans up the patches made to the process
    for(int i = 0; i < (sizeof(patches) / sizeof(Patch*)); i++)
    {
        patches[i]->Uninstall();
    }
    return TRUE;
}