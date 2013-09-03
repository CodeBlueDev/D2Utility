//Test program to show how to interface with the logger and properly unload itself

#include <Windows.h>

bool runThread = true;

void displayChatMessage( char message[128] )
{
	HANDLE handle = GetModuleHandle( "D2Client.dll" );
	DWORD chatBuffer = (DWORD)handle + 0x11d650;
	DWORD chatCall = (DWORD)handle + 0xb1ce0;

	__asm {
		pushad
		pushfd
		push 0
		mov eax, message
		mov dword ptr ds:[ chatBuffer ], eax
		mov ecx, chatBuffer
		mov edx, dword ptr ds:[ message ]
		mov esi, chatCall
		call esi
		popfd
		popad
	}
}

DWORD WINAPI spamChatMessage( LPVOID lpParam )
{
	bool alternate = true;

	while( runThread )
	{
		if( alternate )
		{
			displayChatMessage( "Test!" );
		}
		else
		{
			displayChatMessage( "NEVER" );
		}

		alternate = !alternate;

		Sleep( 100 );
	}

	return 0;
}

void createPatches( )
{
	HANDLE handle = GetModuleHandle( "D2Client.dll" );
	
	DWORD offset = (DWORD)handle + 0x6D703;

	unsigned long hold = NULL;

	VirtualProtect((void*)offset, 2, PAGE_EXECUTE_READWRITE, &hold );
	*(WORD*)offset = 0x23EB;
	VirtualProtect((void*)offset, 2, hold, &hold );
}

void cleanupPatches( )
{
	HANDLE handle = GetModuleHandle( "D2Client.dll" );
	
	DWORD offset = (DWORD)handle + 0x6D703;

	unsigned long hold = NULL;

	VirtualProtect((void*)offset, 2, PAGE_EXECUTE_READWRITE, &hold );
	*(WORD*)offset = 0x8f0f;
	VirtualProtect((void*)offset, 2, hold, &hold );
}

bool __stdcall DllMain( HANDLE process, DWORD reason, LPVOID lpReserved )
{
	switch( reason )
	{
		case DLL_PROCESS_ATTACH:
			createPatches( );
			CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)spamChatMessage, NULL, NULL, NULL );
			return 1;
		case DLL_PROCESS_DETACH:
			cleanupPatches( );
			runThread = false;
			Sleep( 100 );
			return 1;
		default:
			return 0;
	}

}