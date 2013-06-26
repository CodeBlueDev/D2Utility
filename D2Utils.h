#pragma once

#include "constants.h"
#include <iostream>
#include <vector>

class D2Utils
{
public:
	D2Utils( void );
	~D2Utils( void );
	void displayChatMessage( char[] );
	int getMaxPlayerHp( void );
	int getCurPlayerInfo( int );
	bool initPatches( std::vector<Patch> );
};

