#pragma once
#include <Windows.h>
#include <string>

// Class definition to provide methods for injecting into remote processes.
class Injector
{
    public:
        // Attempts to enable debug privileges in order to manipulate remote processes.
        static BOOL EnableDebugPriv(void);
        // Attempts to get a handle on a remote dll to verify if it exists in the remote process.
        static HMODULE GetRemoteDll(DWORD dwPid, std::wstring wDllName);
        // Attempts to write the dll path to load to the process memory and load it.
        static BOOL InjectDll(DWORD dwPid, std::wstring wDllPath);
        // Attempts to unload a dll from the process' memory
        static BOOL UnloadDll(DWORD dwPid, HMODULE hDll);
};