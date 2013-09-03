#include <Windows.h>
#include <iostream>
#include <string>
#include <Windowsx.h>
#include "Log.h"
#include "ProcessUtils.h"

const char* className = "DIEPANDAS";

#define bBrowseID 1
#define bInjectID 2
#define bEjectID 3

Log logger;
ProcessUtils pu;

HWND cbProcess = 0, eDllLoc = 0, eConsole = 0;
char tempPath[ 256 ] = { 0 };

void browseForDll( char retPath[ 256 ] )
{
	char tempPath[ 256 ] = { 0 };
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);	
	ofn.lpstrFile = tempPath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(tempPath);
	ofn.lpstrFilter = ".Dll\0*.DLL\0";
	ofn.lpstrTitle = "Please Locate Dll";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetOpenFileName(&ofn);

	strcpy_s( retPath, 256, tempPath );
}

void initWinElements( HWND hwnd ) 
{
	std::list<std::string> tempProcesses;

	CreateWindowEx( NULL, "Static", "Process:\t", WS_CHILD | WS_VISIBLE, 5, 7, 100, 100, hwnd, NULL, NULL, NULL );
	CreateWindowEx( NULL, "Static", "DLL:    \t", WS_CHILD | WS_VISIBLE, 5, 42, 100, 100, hwnd, NULL, NULL, NULL );
	cbProcess = CreateWindowEx( NULL, "ComboBox", className, 
		WS_BORDER | WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL, 100, 5,
		250, 400, hwnd, NULL, NULL, NULL );
	eDllLoc = CreateWindowEx( NULL, "Edit", "",
		WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 100, 40, 200, 20, hwnd, NULL, NULL, NULL );
	eConsole = CreateWindowEx( NULL, "Edit", "",
		WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 
		5, 130, 350, 100, hwnd, NULL, NULL, NULL );
	CreateWindowEx( NULL, "Button", "...", 
		WS_BORDER | WS_CHILD | WS_VISIBLE, 310, 40, 40, 20, hwnd,(HMENU)bBrowseID, NULL, NULL );
	CreateWindowEx( NULL, "Button", "Inject",
		WS_BORDER | WS_CHILD | WS_VISIBLE, 5, 70, 150, 40, hwnd, (HMENU)bInjectID, NULL, NULL );
	CreateWindowEx( NULL, "Button", "Eject",
		WS_BORDER | WS_CHILD | WS_VISIBLE, 205, 70, 150, 40, hwnd, (HMENU)bEjectID, NULL, NULL );

	logger.Init( eConsole );
	pu.InitLogger( logger );

	tempProcesses = pu.enumateProcesses( );

	for (std::list<std::string>::iterator it = tempProcesses.begin(); it != tempProcesses.end(); it++)
	{
		SendMessage( cbProcess, (UINT) CB_ADDSTRING, 0, (LPARAM) it->c_str( ) );
	}
}

void injectHelper( )
{
	DWORD pid;

	char buffer[ 256 ] = { 0 };
	std::string tempPid;

	ComboBox_GetText(cbProcess, buffer, 256 );
	tempPid.append( buffer );
	tempPid = tempPid.substr( tempPid.find_first_of( "(" ) + 1, (tempPid.find_last_of(")") - tempPid.find_first_of( "(" )) - 1);
	pid = atoi( tempPid.c_str( ) );

	GetWindowText(eDllLoc, buffer, 256 );

	pu.injectDll( pid, buffer );
}

void ejectHelper( )
{
	DWORD pid;

	char buffer[ 256 ] = { 0 };
	std::string tempPid;

	ComboBox_GetText(cbProcess, buffer, 256 );
	tempPid.append( buffer );
	tempPid = tempPid.substr( tempPid.find_first_of( "(" ) + 1, (tempPid.find_last_of(")") - tempPid.find_first_of( "(" )) - 1);
	pid = atoi( tempPid.c_str( ) );

	GetWindowText(eDllLoc, buffer, 256 );

	tempPid.clear( );
	tempPid.append( buffer );

	if( tempPid.find_last_of( "\\" ) != std::string::npos )
		tempPid = tempPid.substr( tempPid.find_last_of( "\\" ) + 1, std::string::npos );

	strcpy_s( buffer, 256, tempPid.c_str( ) );

	pu.ejectDll( pid, buffer );
}

LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_CREATE:
		initWinElements( hwnd );
		break;
	case WM_COMMAND:
		switch( LOWORD( wParam ) )
		{
		case bBrowseID:
			browseForDll( tempPath );
			SetWindowText(eDllLoc, tempPath );
			break;
		case bInjectID:
			injectHelper( );
			break;
		case bEjectID:
			ejectHelper( );
			break;
		}
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	default:
		return DefWindowProc( hwnd, msg, wParam, lParam );
	}
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	WNDCLASSEX wc	= { 0 };
	HWND hwnd		= NULL ;
	MSG message		= { 0 };

	if( pu.AdjustTokenPrivs( ) )
	{
		logger.AddMessage( "Token Privs adjusted successfully" );
	}

	wc.cbSize = sizeof( WNDCLASSEX );
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.lpszClassName = className;
	wc.lpfnWndProc = WndProc;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	RegisterClassEx( &wc );
	
	hwnd = CreateWindowEx( WS_EX_CLIENTEDGE, className, "DIE PANDAS", 
		WS_TILED | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 370, 270, 
		NULL, NULL, hInstance, NULL );

	ShowWindow( hwnd, nCmdShow );
	UpdateWindow( hwnd );

	while( GetMessage( &message, NULL, 0, 0 ) )
	{
		TranslateMessage( &message );
		DispatchMessage( &message );
	}

	return message.wParam;
}