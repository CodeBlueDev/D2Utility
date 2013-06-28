#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)
#include "Injector.h"
#include <TlHelp32.h>

BOOL Injector::EnableDebugPriv(void)
{
    HANDLE hToken;
    // Open the current process to modify the token privileges.
    if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        LUID seDebugNameValue;
        // Lookup the system value for the debug privilege
        if(LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &seDebugNameValue))
        {
            // Assign the debug privilege to a token and adjust the privileges of the current process.
            TOKEN_PRIVILEGES tkPriv;
            tkPriv.PrivilegeCount = 1;
            tkPriv.Privileges[0].Luid = seDebugNameValue;
            tkPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            if(AdjustTokenPrivileges(hToken, FALSE, &tkPriv, sizeof(tkPriv), NULL, NULL))
            {
                CloseHandle(hToken);
                return TRUE;
            }
        }
    }
    return FALSE;
}

HMODULE Injector::GetRemoteDll(DWORD dwPid, std::wstring wDllName)
{
    // Get a snapshot of every module loaded into the remote process.
    MODULEENTRY32W entry;
    entry.dwSize = sizeof(MODULEENTRY32W);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
    // If there is at least one module loaded into the remote process...
    if(Module32First(hSnapshot, &entry))
    {
        do
        {
            // Start checking if the module's name matches the paramter
            if(!_wcsicmp(entry.szModule, wDllName.c_str()))
            {
                CloseHandle(hSnapshot);
                return entry.hModule;
            }
            entry.dwSize = sizeof(MODULEENTRY32);
        }
        //  And every other module in the remote process...
        while(Module32Next(hSnapshot, &entry));
    }
    // No module's name matched the paramters
    CloseHandle(hSnapshot);
    return NULL;
}

BOOL Injector::InjectDll(DWORD dwPid, std::wstring wDllPath)
{
    // If an invalid dwPid is provided don't bother trying to inject.
    if (!dwPid)
    {
        return FALSE;
    }
    // Get a handle to the process to write to its memory space
    HANDLE hProc = OpenProcess(CREATE_THREAD_ACCESS, FALSE, dwPid);
    if(hProc)
    {
        // Loads the Kernel32 library into this address space to retrieve the LoadLibrary function.
        HMODULE hKernel32 = LoadLibrary(L"Kernel32.dll");
        if(hKernel32)
        {
            // Use GetProcAddress function to find the address for the LoadLibraryW function from the Kernel32 library.
            LPVOID lpLoadLibraryW = GetProcAddress(hKernel32, "LoadLibraryW");
            if(lpLoadLibraryW)
            {
                // Alloc space in the remote process for us to use
                LPVOID lpRemoteString = (LPVOID)VirtualAllocEx(hProc, NULL, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                if(lpRemoteString)
                {
                    // Write the dll path value to the memory allocated
                    WriteProcessMemory(hProc, lpRemoteString, wDllPath.c_str(), wDllPath.size() * 2, NULL);
                    // Load the injected dll to the remote process
                    HANDLE hThread = CreateRemoteThread(hProc, NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(lpLoadLibraryW), lpRemoteString, NULL, NULL);
                    WaitForSingleObject(hThread, INFINITE);
                    // Clean up
                    VirtualFreeEx(hProc, lpRemoteString, 0, MEM_RELEASE);
                    FreeLibrary(hKernel32);
                    CloseHandle(hProc);
                    CloseHandle(hThread);
                    return TRUE;
                }
            }
            FreeLibrary(hKernel32);
        }
        CloseHandle(hProc);
    }
    return FALSE;
}

BOOL Injector::UnloadDll(DWORD dwPid, HMODULE hDll)
{
    // If an invalid dwPid is provided don't bother trying to unload.
    if (!dwPid)
    {
        return FALSE;
    }
    // Get a handle to the process to modify its memory space
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
    if(hProc)
    {
        // Loads the Kernel32 library into this address space to retrieve the FreeLibrary function.
        HMODULE hKernel32 = LoadLibrary(L"Kernel32.dll");
        if(hKernel32)
        {
            // Use GetProcAddress function to find the address for the FreeLibrary function from the Kernel32 library.
            LPVOID lpFreeLibrary = GetProcAddress(hKernel32, "FreeLibrary");
            if(lpFreeLibrary)
            {
                // Creates a remote thread to unload the dll from the remote process
                HANDLE hThread = CreateRemoteThread(hProc, NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(lpFreeLibrary), hDll, NULL, NULL);
                WaitForSingleObject(hThread, INFINITE);
                // Cleanup
                FreeLibrary(hKernel32);
                CloseHandle(hThread);
                CloseHandle(hProc);
                return TRUE;
            }
            FreeLibrary(hKernel32);
        }
        CloseHandle(hProc);
    }
    return FALSE;
}