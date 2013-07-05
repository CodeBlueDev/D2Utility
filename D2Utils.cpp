#include "D2Utils.h"
#include <Windows.h>

D2Utils::D2Utils(void)
{
}

D2Utils::~D2Utils(void)
{
}

bool D2Utils::initPatches( std::vector< Patch > patches)
{
	unsigned long hold = NULL;

	for( unsigned int i = 0; i < patches.size( ); i++ )
	{
		hold = NULL;
		
		if( !VirtualProtect((void*)patches[ i ].address, patches[ i ].size, PAGE_EXECUTE_READWRITE, &hold ) )
			return false;
	
		//add other conditionals for other sizes
		if( patches[ i ].size == 2 )			
		{
			*(WORD*)patches[ i ].address = patches[ i ].newVal;
		}
		else if( patches[ i ].size == 4 )
		{
			*(DWORD*)patches[ i ].address = patches[ i ].newVal;
		}

		if( !VirtualProtect((void*)patches[ i ].address, patches[ i ].size, hold, &hold ) )
			return false;
	}

	return true;
}

void D2Utils::displayGameText( wchar_t message[128], int x, int y, int color )
{
	__asm {
		pushad
		pushfd
		push 0
		push color
		push y
		mov edx,x
		mov ecx, dword ptr ds:[message]
		mov esi, 0x6fabd298
		call esi
		popfd
		popad
	}
}

void D2Utils::drawLine( int x1, int y1, int x2, int y2, int color )
{
	__asm {
		pushad
		pushfd

		push 0x7f
		push color
		push y2
		push x2
		push y1
		push x1
		mov esi, 0x6fabd016
		call esi

		popfd
		popad
	}
}

void D2Utils::drawRectangle( int x1, int y1, int x2, int y2, int color, int trans )
{
	__asm {
		pushad
		pushfd
		push trans
		push color
		push y2
		push x2
		push y1
		push x1
		mov esi,0x6fabd02e
		call esi
		popfd
		popad
	}
}

void D2Utils::displayChatMessage( char message[128] )
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

int D2Utils::getMaxPlayerHp( )
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

int D2Utils::getCurPlayerInfo( int type )
{
	DWORD curInfo = 0;
	DWORD infoType = type;

	__asm {
		pushad
		pushfd
		mov eax, infoType
		mov esi, 0x6fb1cc10
		call esi
		sar eax,8
		mov curInfo,eax
		popfd
		popad
	}

	return curInfo;
}