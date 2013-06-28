#include "Patch.h"

Patch::Patch(PatchType type, Dll dll, INT offset, int function, INT len) 
    : dll(dll), len(len), newCode(new BYTE[len]), oldCode(new BYTE[len]), isInstalled(FALSE) 
{ 
    address = GetDllOffset(dll, offset);
}

Patch::~Patch()
{
    if (IsInstalled())
    {
        Uninstall();
    }
    delete newCode;
    delete oldCode;
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
    // TODO: Install the patch
    isInstalled = TRUE;
    return isInstalled;
}

BOOL Patch::Uninstall()
{
    // Check if the Patch has already been uninstalled
    if(!IsInstalled())
    {
        return TRUE;
    }
    // TODO: Uninstall the Patch
    isInstalled = FALSE;
    return !isInstalled;
}