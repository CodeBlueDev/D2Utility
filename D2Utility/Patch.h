#pragma once
#include <Windows.h>
#include <string>

// Enumeration of the various dlls used by D2.
enum Dll { D2CLIENT=0, D2COMMON, D2GFX, D2LANG, D2WIN, D2NET, D2GAME, D2LAUNCH, FOG, BNCLIENT, STORM, D2CMP, D2MULTI, D2MCPCLIENT };
// Enumeration of the various patches used and their opcode.
enum PatchType { Call=0xE8, Jmp=0xE9, Nop=0x90 };

// Class to hold patch information to modify the client.
class Patch 
{
    private:
        // Variables:
        Dll dll;
        INT address, function, len;
        BYTE* newCode;
        BYTE* oldCode;
        BOOL isInstalled;
        // Functions:
        // Function to get the address to patch in the specified Dll given an offset.
        INT GetDllOffset(Dll dll, INT offset);

    public:
        // Functions:
        // Calculates the address to write to and the new code values
        Patch(PatchType type, Dll dll, INT offset, INT function, INT len);
        // Destructor to clean up the patches made when the patch was installed.
        ~Patch();
        // Checks if the patch has been installed on the client.
        BOOL IsInstalled();
        // Attempts to install the patch on the client.
        BOOL Install();
        // Attempts to uninstall the patch from the client.
        BOOL Uninstall();
};