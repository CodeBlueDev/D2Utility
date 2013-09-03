#include "ProcessUtils.h"
#include <TlHelp32.h>
#include <string>
#include <sstream>

ProcessUtils::ProcessUtils( )
{

}

ProcessUtils::~ProcessUtils( )
{

}

void ProcessUtils::InitLogger( Log l )
{
	this->logger = l;
}

bool ProcessUtils::AdjustTokenPrivs( )
{
	HANDLE token;
    LUID luid;
    TOKEN_PRIVILEGES tp;

	if( !OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &token) )
	{
		this->logger.AddMessage( "Could not open the Process Token" );
	}
	
	if( !LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid) )
	{
		this->logger.AddMessage( "Could not look-up privilege value" );
	}

	tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if( !AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), NULL, NULL) )
	{
		this->logger.AddMessage( "Could not adjust privilege value" );
	}

	CloseHandle( token );

	return true;
}

std::list<std::string> ProcessUtils::enumateProcesses( )
{
	PROCESSENTRY32 pe32;
	HANDLE hSnapshot;
	std::string tempIdName;
	std::list<std::string> retList;

	pe32.dwSize = sizeof( PROCESSENTRY32 );
	hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hSnapshot == INVALID_HANDLE_VALUE )
	{
		this->logger.AddMessage( "Could not create snapshot" );
		return retList;
	}

	if( Process32First( hSnapshot, &pe32 ) )
	{
		do {
			tempIdName.clear( );
			std::ostringstream stream;
			
			stream << pe32.th32ProcessID;

			tempIdName.append( pe32.szExeFile );
			tempIdName.append( " ( " + stream.str( ) + " ) " );

			retList.push_back( tempIdName.c_str( ) );

		} while( Process32Next( hSnapshot, &pe32 ) );
	}

	CloseHandle( hSnapshot );
	return retList;
}

DWORD ProcessUtils::getMainThread( DWORD pid )
{
	THREADENTRY32 te32;
	HANDLE hSnapshot;

	te32.dwSize = sizeof( THREADENTRY32 );
	hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, pid );
	if( hSnapshot == INVALID_HANDLE_VALUE )
	{
		this->logger.AddMessage( "Couldn't create Thread snapshot" );
		return 0;
	}

	if( Thread32First( hSnapshot, &te32 ) )
	{
		do {
			if( te32.th32OwnerProcessID == pid )
			{
				CloseHandle( hSnapshot );
				return te32.th32ThreadID;
			}
		} while( Thread32Next( hSnapshot, &te32 ) );
	}

	CloseHandle( hSnapshot );
	return 0;
}

HMODULE enumerateModules( DWORD pid, char* module )
{
	MODULEENTRY32 me32;
	HANDLE hSnapshot;

	me32.dwSize = sizeof( MODULEENTRY32 );
	hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, pid );
	if( hSnapshot == INVALID_HANDLE_VALUE )
	{
		//this->logger.AddMessage( "Couldn't create module snapshot" );
		return NULL;
	}

	if( Module32First( hSnapshot, &me32 ) )
	{
		do {
			if( !strcmp( me32.szModule, module ) )
				return me32.hModule;
		} while( Module32Next( hSnapshot, &me32 ) );
	}

	CloseHandle( hSnapshot );
	return NULL;
}

DWORD ProcessUtils::ejectDll( DWORD pid, char* dllName )
{
	PROCESS_INFORMATION pi = { 0 };
	HANDLE thread = NULL;
	DWORD exitCode = 0;

	if( !pid )
	{
		this->logger.AddMessage( "No process selected" );
		return 0;
	}

	pi.dwProcessId = pid;
	pi.dwThreadId = this->getMainThread( pid );
	pi.hProcess = OpenProcess( PROCESS_ALL_ACCESS, TRUE, pi.dwProcessId );
	pi.hThread = OpenThread( THREAD_ALL_ACCESS, TRUE, pi.dwThreadId );

	if( pi.hProcess == 0 || pi.hThread == 0 )
	{
		if( pi.hProcess )
			CloseHandle( pi.hProcess );
		if( pi.hThread )
			CloseHandle( pi.hThread );

		this->logger.AddMessage( "Could not open process/thread" );

		return 0;
	}

	SuspendThread( pi.hThread );
	Sleep( 100 );

	HMODULE ejectModule = enumerateModules( pid, dllName );

	thread = CreateRemoteThread( pi.hProcess, NULL, 0, 
		(LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary("kernel32.dll"), "FreeLibrary"), 
		ejectModule , 0, NULL );

	if( thread == NULL )
	{
		this->logger.AddMessage( "Error creating thread" );
		return 0;
	}

	WaitForSingleObject(thread, INFINITE);
	GetExitCodeThread(thread, &exitCode );

	CloseHandle( thread );
	ResumeThread( pi.hThread );
	CloseHandle( pi.hThread );
	CloseHandle( pi.hProcess );

	std::string exitFormatting;

	if( exitCode == 0 )
	{
		exitFormatting.append( "Error ejecting " );
		exitFormatting.append( dllName );
		this->logger.AddMessage( exitFormatting.c_str( ) );
		return 0;
	}
	else
	{
		exitFormatting.append( dllName );
		exitFormatting.append( " Ejected successfully!");
		this->logger.AddMessage( exitFormatting.c_str( ) );
		return 0;
	}

	return exitCode;
}

DWORD ProcessUtils::injectDll( DWORD pid, char* dllName )
{
	DWORD exitCode = 0;
	HANDLE thread = NULL;
	PROCESS_INFORMATION pi = { 0 };
	LPVOID allocDllName;

	if( !pid )
	{
		this->logger.AddMessage( "No process selected" );
		return 0;
	}

	pi.dwProcessId = pid;
	pi.dwThreadId = this->getMainThread( pid );
	pi.hProcess = OpenProcess( PROCESS_ALL_ACCESS, TRUE, pi.dwProcessId );
	pi.hThread = OpenThread( THREAD_ALL_ACCESS, TRUE, pi.dwThreadId );

	if( pi.hProcess == 0 || pi.hThread == 0 )
	{
		if( pi.hProcess )
			CloseHandle( pi.hProcess );
		if( pi.hThread )
			CloseHandle( pi.hThread );

		this->logger.AddMessage( "Could not open process/thread" );

		return 0;
	}

	SuspendThread( pi.hThread );
	Sleep( 100 );

	allocDllName = VirtualAllocEx( pi.hProcess, NULL, strlen( dllName ) + 1, MEM_COMMIT, PAGE_READWRITE );

	int allocError = GetLastError( );

	WriteProcessMemory( pi.hProcess, allocDllName, dllName, strlen( dllName ) + 1, NULL );

	thread = CreateRemoteThread( pi.hProcess, NULL, 0, 
		(LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibrary("kernel32.dll"), "LoadLibraryA"), 
		allocDllName, 0, NULL );

	if( thread == NULL )
	{
		this->logger.AddMessage( "Error creating thread" );
		return 0;
	}

	WaitForSingleObject(thread, INFINITE);
	GetExitCodeThread(thread, &exitCode );

	CloseHandle( thread );
	VirtualFreeEx( pi.hProcess, NULL, strlen( dllName ) + 1, MEM_DECOMMIT );
	ResumeThread( pi.hThread );
	CloseHandle( pi.hThread );
	CloseHandle( pi.hProcess );

	std::string exitFormatting;

	if( exitCode == 0 )
	{
		exitFormatting.append( "Error injecting " );
		exitFormatting.append( dllName );
		this->logger.AddMessage( exitFormatting.c_str( ) );
		return 0;
	}
	else
	{
		exitFormatting.append( dllName );
		exitFormatting.append( " Injected successfully!");
		this->logger.AddMessage( exitFormatting.c_str( ) );
		return 0;
	}

	exitFormatting.clear( );

	if( allocError != 0 )
	{
		char temp[ 5 ] = {0};

		_itoa_s(allocError, temp, 5, 10 );

		exitFormatting.append( "Error code: " );
		exitFormatting.append( temp );
		this->logger.AddMessage( exitFormatting.c_str( ) );
	}

	return exitCode;
}