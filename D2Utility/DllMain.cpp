#include "D2Utility.h"

// Dll entry point: redirects to D2Utility::Start or D2Utility::Stop
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved)
{
    switch(dwReason)
    {
    case DLL_PROCESS_ATTACH:
        return D2Utility::Start(hInstance, lpvReserved);
        break;
    case DLL_PROCESS_DETACH:
        return D2Utility::Stop();
        break;
    }
    return TRUE;
}