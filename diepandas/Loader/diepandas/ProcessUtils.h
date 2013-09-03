#pragma once

#include "Log.h"
#include <Windows.h>
#include <iostream>
#include <list>

class ProcessUtils
{
private:
	Log logger;

public:
	ProcessUtils( );
	~ProcessUtils( void );

	void InitLogger( Log );

	bool AdjustTokenPrivs( void );
	std::list<std::string> enumateProcesses( void );
	
	DWORD getMainThread( DWORD );
	DWORD injectDll( DWORD, char* );
	DWORD ejectDll( DWORD, char* );
};