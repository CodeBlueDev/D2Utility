#pragma once

#include <Windows.h>

class Log
{
private:
	HWND output;
	char* backLog;

public:
	Log( void );
	~Log( void );

	void Init( HWND );
	bool AddMessage( const char* );
	bool AddMessage( const char*, bool );
};