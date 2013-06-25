#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>

int getCurrentPlayerHp( )
{
	DWORD curHP = 0;

	__asm {
		pushad
		pushfd
		mov eax, 6
		mov esi, 0x6fb1cc10
		call esi
		sar eax,8
		mov curHP,eax
		popfd
		popad
	}

	return curHP;
}

int getMaxPlayerHp( )
{
	DWORD maxHP = 0;

	__asm {
		pushad
		pushfd
		mov ecx, dword ptr ds:[0x6fbcd050]
		push ecx
		mov esi, 0x6fabc662
		call esi
		sar eax,8
		mov maxHP,eax
		popfd
		popad
	}

	return maxHP;
}

void displayChatMessage( char* message )
{
	__asm {
		pushad
		pushfd
		push 0
		mov eax, message
		mov dword ptr ds:[ 0x6fbcd650 ], eax
		mov ecx, 0x6fbcd650
		mov edx, dword ptr ds:[ message ]
		mov esi, 0x6FB61CE0
		call esi
		popfd
		popad
	}
}

DWORD WINAPI manageHP( LPVOID lpParam )
{
	while( true )
	{	
		if( ((double)getCurrentPlayerHp( ) / getMaxPlayerHp( )) <= .5 )
		{
			displayChatMessage( "Potion!" );
		}

		Sleep( 100 );
	}

	return 0;
}

bool __stdcall DllMain( HANDLE process, DWORD reason, LPVOID lpReserved )
{
	switch( reason )
	{
		case DLL_PROCESS_ATTACH:
			CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)manageHP, NULL, NULL, NULL );
			return 1;
		default:
			return 0;
	}

}