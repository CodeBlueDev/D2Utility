#pragma once
#define DLL_NAME L"D2Utility.dll"
#define WNDW_CLASS_NAME L"Diablo II"
#include "Injector.h"
#include <vector>

// Class to hold the window information and provide methods to interact with it.
class GameWindow {
    private:
        // Variables
        HWND pHwnd;
        HMODULE pRemoteDll;
    public:
        // Variables:
        DWORD dwPid;
        wchar_t szTitle[1024];
        // Functions:
        // Retrieves any information missing from the system.
        GameWindow(HWND hwnd);
        // Checks if the dll is injected into the process.
        BOOL IsInjected();
        // Attempts to inject the Dll at the path passed in into the process to load.
        BOOL Inject(std::wstring wDllPath);
        // Attempts to unload the Dll name defined here out of the process.
        BOOL Unload();
};
// Checks for window processes that match the WNDW_CLASS_NAME defined above.
BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);