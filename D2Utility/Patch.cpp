#include "Patch.h"

// Constructor: Saving the Dll so we know what Dll to patch, the totalLen so we know how many bytes to patch, instantiating a buffer to hold the code to write to memory,
// instantiating a buffer to hold the default code, and setting isInstalled to false because the patch hasn't been installed.
Patch::Patch(Dll dll, DWORD offset, DWORD patch, INT len) : dll(dll), len(len), newCode(patch), isInstalled(FALSE) 
{ 
    // Get the proper address to write to by getting the Dll base address and adding the offset parameter to it.
    address = GetDllOffset(dll, offset);
    // Read the address to save the original code being modified and store it in the buffer
    ReadProcessMemory(GetCurrentProcess(), (VOID*)address, &oldCode, len, NULL);
    // Replace the new code with the patch type and the new address to redirect to.
    //newCode[0] = type;
    // Add other conditionals for other sizes if necessary
    //if(patchLen == 2)
    //{
    //    newCode[1] = patch;
    //}
    //else if(patchLen == 4)
    //{
    //    *(DWORD*)newCode[1] = patch;
    //}
}

Patch::~Patch()
{
    if (IsInstalled())
    {
        Uninstall();
    }
}

INT Patch::GetDllOffset(Dll dll, INT offset)
{
    // String representation of the Dlls used by D2
    const char* szDlls[] = { "D2CLIENT.dll", "D2COMMON.dll", "D2GFX.dll", "D2LANG.dll", "D2WIN.dll", "D2NET.dll", "D2GAME.dll", "D2LAUNCH.dll", "FOG.dll", "BNCLIENT.dll", "STORM.dll", "D2CMP.dll", "D2MULTI.dll", "D2MCPCLIENT.dll" };
    // Check if the dll is loaded
    HMODULE hModule = GetModuleHandle(szDlls[dll]);
    if (!hModule)
    {
        // Try to load it if it is not
        hModule = LoadLibrary(szDlls[dll]);
        if(!hModule)
        {
            // Failed to load - something is wrong.
            return 0;
        }
    }
    // If the offset is an ordinal, get the proper address.
    if(offset < 0)
    {
        return (DWORD)GetProcAddress(hModule, (LPCSTR)-offset);
    }
    // Regular offset, return them added together.
    return ((DWORD)hModule) + offset;
}

BOOL Patch::IsInstalled()
{
    return isInstalled;
}

BOOL Patch::Install()
{
    // Check if the Patch has already been installed.
    if(IsInstalled())
    {
        return TRUE;
    }
    MessageBox(NULL, "Trying to install patch!", "Patch Install", MB_OK);
    isInstalled = Write(newCode);
    return isInstalled;
}

BOOL Patch::Uninstall()
{
    // Check if the Patch has already been uninstalled
    if(!IsInstalled())
    {
        return TRUE;
    }
    isInstalled = Write(oldCode);
    return !isInstalled;
}

BOOL Patch::Write(DWORD code)
{
    DWORD protect;
    if(!VirtualProtect((VOID*)address, len, PAGE_READWRITE, &protect))
    {
        return FALSE;
    }
    WriteProcessMemory(GetCurrentProcess(), (VOID*)address, &code, len, NULL);
    return !VirtualProtect((VOID*)address, len, protect, &protect);
}