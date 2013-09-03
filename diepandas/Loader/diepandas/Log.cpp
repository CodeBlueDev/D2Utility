#include <string>
#include <time.h>
#include "Log.h"

Log::Log( )
{

}

Log::~Log( )
{

}

void Log::Init( HWND handle )
{
	this->output = handle;
	this->AddMessage( "Logging started" );

	if( strlen( this->backLog ) > 0 )
	{
		this->AddMessage( "------------------------------------------------------------", false);
		this->AddMessage( this->backLog );
		this->AddMessage( "Backlog:", false );
		this->AddMessage( "------------------------------------------------------------", false );

		free( this->backLog );
	}
}

bool Log::AddMessage( const char* message )
{
	return this->AddMessage( message, true );
}

bool Log::AddMessage( const char* message, bool timeCode )
{
	if( this->output == NULL )
	{
		if( this->backLog == NULL )
		{
			this->backLog = (char*)malloc(1024);
			ZeroMemory( this->backLog, 1024 );
		}

		if( strlen( this->backLog) > 0 )
			strcat_s( backLog, 1024, "\r\n" );

		strcat_s( this->backLog, 1024, message );
	}

	char* curLog = (char*) malloc( 1024 );
	char* tempLog = (char*) malloc( 1024 );

	ZeroMemory( curLog, 1024 );

	if( timeCode)
	{
		time_t rawtime;
		struct tm * timeinfo;

		time (&rawtime);
		timeinfo = localtime (&rawtime);

		char tempTime[ 5 ] = { 0 };
		std::string curTime;

		curTime.append( "[");

		_itoa_s( timeinfo->tm_hour, tempTime, 5, 10 );
		curTime.append( tempTime );
		curTime.append( ":" );

		_itoa_s( timeinfo->tm_min, tempTime, 5, 10 );
		if( timeinfo->tm_min < 10 )
			curTime.append( "0" );
		curTime.append( tempTime );
		curTime.append( ":" );

		_itoa_s( timeinfo->tm_sec, tempTime, 5, 10 );
		if( timeinfo->tm_sec < 10 )
			curTime.append( "0" );
		curTime.append( tempTime );

		strcat_s( curLog, 1024, curTime.c_str( ) );
		strcat_s( curLog, 1024, "]: " );
	}

	strcat_s( curLog, 1024, message );

	if( strlen( curLog ) > 0 )
		strcat_s( curLog, 1024, "\r\n" );

	GetWindowText(this->output, tempLog, 1024 - strlen( message ) - 20 );

	strcat_s( curLog, 1024, tempLog );

	bool ret = SetWindowText(this->output, curLog );

	free( curLog );
	free( tempLog );

	return ret;
}