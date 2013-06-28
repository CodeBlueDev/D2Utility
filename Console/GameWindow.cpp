#include "GameWindow.h"

BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) 
{
    wchar_t szClassName[1024];
    GetClassName(hwnd, szClassName, 1024);
    if(!wcscmp(szClassName, WNDW_CLASS_NAME))
    {
        std::vector<GameWindow*>* pVector = (std::vector<GameWindow*>*)lParam;
        pVector->push_back(new GameWindow(hwnd));
    }
    return TRUE;
}

GameWindow::GameWindow(HWND hwnd) : pHwnd(hwnd)
{
    // Use Windows system calls to find additional information about the remote process.
    GetWindowThreadProcessId(pHwnd, &dwPid);
    GetWindowText(pHwnd, szTitle, 1024);
    pRemoteDll = Injector::GetRemoteDll(dwPid, DLL_NAME);
}

BOOL GameWindow::IsInjected() 
{
    return pRemoteDll != NULL;
}

BOOL GameWindow::Inject(std::wstring wDllPath)
{
    // Check that the Dll isn't already loaded.
    if(IsInjected())
    {
        return TRUE;
    }
    // Otherwise inject the Dll
    if(Injector::InjectDll(dwPid, wDllPath))
    {
        // If the Dll was injected successfully, update the remote reference to it.
        pRemoteDll = Injector::GetRemoteDll(dwPid, DLL_NAME);
        // Verify the Dll was injected
        if(IsInjected())
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL GameWindow::Unload()
{
    // Check that the Dll isn't already unloaded.
    if(!IsInjected())
    {
        return TRUE;
    }
    // Otherwise unload the Dll
    if(Injector::UnloadDll(dwPid, pRemoteDll))
    {
        // If the Dll was unloaded successfully, update the remote reference to it.
        pRemoteDll = Injector::GetRemoteDll(dwPid, DLL_NAME);
        // Verify the Dll was unloaded
        if(!IsInjected())
        {
            return TRUE;
        }
    }
    return FALSE;
}

