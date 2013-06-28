#include "Console.h"

// Builds the expected Dll location
std::wstring BuildDllFilePath()
{
    wchar_t dllPath[MAX_PATH] = L"";
    GetCurrentDirectory(sizeof(dllPath), dllPath);
    wcscat_s(dllPath, L"\\");
    wcscat_s(dllPath, DLL_NAME);
    return std::wstring(dllPath);
}

// Prints a menu item so the user knows whether to inject or unload from a process
VOID PrintProcessMenuMessage(GameWindow* win, INT* opt)
{
    if(!win->IsInjected())
    {
        wprintf(L"\t%d) Inject into \"%s\" process (PROCESSID: %d)\n", *opt++, win->szTitle, win->dwPid);
        return;
    }
    wprintf(L"\t%d) Unload from \"%s\" process (PROCESSID: %d)\n", *opt++, win->szTitle, win->dwPid);
}

// Prints the status of the unload action
VOID PrintUnloadMessage(GameWindow* win)
{
    if(win->Unload())
    {
        wprintf(L"Unloaded \"%s\" from \"%s\" process (PROCESSID: %d)\n", DLL_NAME, win->szTitle, win->dwPid);
        return;
    }
    wprintf(L"Failed to unload \"%s\" from \"%s\" window (PROCESSID: %d)\n", DLL_NAME, win->szTitle, win->dwPid);
}

// Prints the status of the inject action
VOID PrintLoadMessage(GameWindow* win, std::wstring wDllPath)
{
    if(win->Inject(wDllPath))
    {
        wprintf(L"Injected \"%s\" into \"%s\" window (PROCESSID: %d)\n", DLL_NAME, win->szTitle, win->dwPid);
        return;
    }
    wprintf(L"Failed to inject \"%s\" into \"%s\" window (PROCESSID: %d)\n", DLL_NAME, win->szTitle, win->dwPid);
}

// Prints a menu for the user to decide which processes to inject into or unload from
VOID PrintMenu(std::vector<GameWindow*> windows, std::wstring wDllPath)
{
    INT opt = 0;
    // Display a menu to inject into or unload from all windows found.
    wprintf(L"Please choose an option:\n");
    wprintf(L"\t%d) Inject into all \"%s\" processes.\n", opt++, WNDW_CLASS_NAME);
    wprintf(L"\t%d) Unload from all \"%s\" processes.\n", opt++, WNDW_CLASS_NAME);
    // Prints a menu item for each window
    for(std::vector<GameWindow*>::iterator w = windows.begin(); w < windows.end(); w++)
    {
        PrintProcessMenuMessage(*w, &opt);
    }
    // Get input from the user on what action to perform
    opt = _getch() - 48;
    switch(opt)
    {
    case 0:
    case 1:
        // Loop over all the windows and perform the action.
        for(std::vector<GameWindow*>::iterator w = windows.begin(); w < windows.end(); w++)
        {
            !opt ? PrintUnloadMessage(*w) : PrintLoadMessage(*w, wDllPath);
        }
        break;
    default:
        // Check that the input for single window is invalid
        opt -= 2;
        if(opt < 0 || opt >= (int)windows.size())
        {
            wprintf(L"You have chosen an invalid option.\n");
            break;
        }
        GameWindow* w = windows[opt];
        w->IsInjected() ? PrintUnloadMessage(windows[opt]) : PrintLoadMessage(windows[opt], wDllPath);
        break;
    }
    wprintf(L"\n");
}

INT main(INT argc, const CHAR* argv[])
{
    // Verify the application is running as an administrator.
    if(!Injector::EnableDebugPriv())
    {
        wprintf(L"This program must be run as an administrator!\nPress the 'Enter' key to exit.");
        _getch();
        return 1;
    }
    // Get the anticipated Dll path.
    std::wstring wDllPath = BuildDllFilePath();
    // Verify the Dll exists at this location.
    /*std::ifstream dllFile(wDllPath);
    if(!dllFile.good())
    {
        wprintf(L"Could not locate the \"%s\" file. Verify the file exists and is in the same directory as the application.\nPress the 'Enter' key to exit.", DLL_NAME);
        _getch();
        return 2;
    }
    dllFile.close();*/
    // Get a list of window instances.
    std::vector<GameWindow*> windows;
    EnumWindows(EnumWindowsCallback, (LPARAM)&windows);
    // Verify windows were found if there are any window instances
    if(!windows.size())
    {
        wprintf(L"No \"%s\" windows found!\nPress the 'Enter' key to exit.", WNDW_CLASS_NAME);
        _getch();
        return 3;
    }
    // Display a menu for the user to determine the course of action.
    while(TRUE)
    {
        PrintMenu(windows, wDllPath);
    }
    return 0;
}