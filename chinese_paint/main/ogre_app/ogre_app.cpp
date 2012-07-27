#include "ogre_app.h"
#include <Tlhelp32.h>
#include <cstdio>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <ios>
void OgreApp::redirectIOToConsole()
{
	//using namespace std;
	int hConHandle;
	long lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	// allocate a console for this app
	AllocConsole();
	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = 1000;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE),
		coninfo.dwSize);
	// redirect unbuffered STDOUT to the console
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	// redirect unbuffered STDIN to the console
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );
	// redirect unbuffered STDERR to the console
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	std::ios::sync_with_stdio();
}

OgreApp::OgreApp()
{
	redirectIOToConsole();
}