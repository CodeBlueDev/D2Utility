#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include "D2Utils.h"

D2Utils util;

DWORD WINAPI manageHP( LPVOID lpParam )
{
	char buffer[ 128 ] = {0};

	sprintf( buffer, "Life: %d, Mana: %d, Stamina: %d", util.getCurPlayerInfo( Life ), util.getCurPlayerInfo( Mana ), util.getCurPlayerInfo( Stamina ) );
	util.displayChatMessage( buffer );

	while( true )
	{
		if( ((double)util.getCurPlayerInfo( Life ) / util.getMaxPlayerHp()) <= .5 )
		{
			util.displayChatMessage( "Potion!" );
		}

		Sleep( 100 );
	}

	return 0;
}

__declspec( naked ) void gameloop_Hook( )
{
	util.drawRectangle( 0xec, 0x2c, 0x150, 0x4f, 0x00, 2 );
	util.displayGameText( L"2 Code Blue", 0xf1, 0x3b, 3 );
	util.displayGameText( L"Hi n stuff", 0xfa, 0x4b, 0 );
	util.drawLine( 0xec, 0x2c, 0x150, 0x2c, 0xc8 );
	util.drawLine( 0xec, 0x2c, 0xec, 0x4f, 0xc8 );
	util.drawLine( 0xec, 0x4f, 0x150, 0x4f, 0xc8 );
	util.drawLine( 0x150, 0x2c, 0x150, 0x4f, 0xc8 );

	__asm {
		mov esi,0x6FAC8BB0
		call esi
		retn
	}
}

void managePatches( )
{
	std::vector< Patch > patches;

	Patch tempPatches[ 3 ] = { 0 };

	tempPatches[ 0 ].address = 0x6FB1D703;
	tempPatches[ 0 ].newVal = 0x23EB;
	tempPatches[ 0 ].size = 2;

	tempPatches[ 1 ].address = 0x6FB1D7D6;
	tempPatches[ 1 ].newVal = 0x25EB;
	tempPatches[ 1 ].size = 2;

	tempPatches[ 2 ].address = 0x6FB1DAC9;
	tempPatches[ 2 ].newVal = 0x1EEB;
	tempPatches[ 2 ].size = 2;

	patches.push_back( tempPatches[ 0 ] );
	patches.push_back( tempPatches[ 1 ] );
	patches.push_back( tempPatches[ 2 ] );
	
	if( !util.initPatches( patches ) )
	{
		MessageBox( NULL, "Issues initialising patches", "D2Util", MB_OK );
	}

	//main gameloop patch for graphics
	unsigned long hold = NULL;

	if( VirtualProtect((void*)0x6FACD7AB, 4, PAGE_EXECUTE_READWRITE, &hold ) )
	{
		*(DWORD*)(0x6FACD7AB) = (unsigned long)&gameloop_Hook - 0x6FACD7AF;
		VirtualProtect((void*)0x6FACD7AB, 4, hold, &hold );
	}
	else
	{
		MessageBox( NULL, "Issues initialising main game loop patch", "D2Util", MB_OK );
	}

}

bool __stdcall DllMain( HANDLE process, DWORD reason, LPVOID lpReserved )
{
	switch( reason )
	{
		case DLL_PROCESS_ATTACH:
			managePatches( );
			CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)manageHP, NULL, NULL, NULL );
			return 1;
		default:
			return 0;
	}

}