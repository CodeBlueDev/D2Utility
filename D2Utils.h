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
	void displayGameText( wchar_t[], int x, int y, int color );
	void drawRectangle( int x1, int y1, int x2, int y2, int color, int trans );
	void drawLine( int x1, int y1, int x2, int y2, int color );
	int getMaxPlayerHp( void );
	int getCurPlayerInfo( int );
	bool initPatches( std::vector<Patch> );
};

