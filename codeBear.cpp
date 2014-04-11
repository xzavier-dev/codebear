#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <math.h>
#include "resource.h"
#include <mmsystem.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <time.h>
#include <shellapi.h>

#define VK_L 0x4c        // for line comment
#define VK_C 0x43        // for class comment
#define VK_F 0x46        // for function comment
#define VK_S 0x53

#define KEYDOWN( vk_code ) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1:0 )
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0:1)

//#pragma comment( lib, "lib/winmm.lib")

#define WINDOW_CLASS_NAME "WINCLASS1"
#define WM_TRAY (WM_USER + 100)

enum comment_t {
	COMMENT_LINE,
	COMMENT_FUNCTION,
	COMMENT_CLASS,
	COMMENT_FILE,
};

//comment_t comment;


NOTIFYICONDATA nid;
HMENU hMenu;

HINSTANCE hinstance_app = NULL;

HWND hwnd_main = NULL;
char buffer[1000];
static int wm_paint_count = 0;
bool isControlPressed = false;
bool isAltPressed = false;
bool isSpacePressed = false;
bool isInsertLock = false;

void InitTray( HINSTANCE hInstance, HWND hWnd ){
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = IDI_ICON1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	nid.uCallbackMessage = WM_TRAY;
	nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE( IDI_ICON1 ));
	lstrcpy(nid.szTip,"CodeBear");

	hMenu = CreatePopupMenu();

	AppendMenu( hMenu, MF_STRING,0,TEXT("Exit"));
	Shell_NotifyIcon(NIM_ADD,&nid);
}

void OutPutMessage(char str[],HWND hwnd){
	for( int i = 0; i < strlen(str); ++i ){
		PostMessage(hwnd,WM_CHAR,str[i],0);
	}
}

void InitCommentStr( comment_t comment ) {
	
	SYSTEMTIME sys_time;
	GetLocalTime( &sys_time );
	
	switch( comment ) {
	case COMMENT_LINE:                     // line comment
		sprintf(buffer,"// <linkrules> [%d-%.2d-%.2d] ",sys_time.wYear, sys_time.wMonth, sys_time.wDay);
		break;
	case COMMENT_FUNCTION:                // function comment
		sprintf( buffer, "/*\n====================\n\n====================\n*/" );
		break;
	case COMMENT_CLASS:                  // class comment
		sprintf( buffer, "/*\n============================================================\n\n\n============================================================\n*/" );
		break;
	case COMMENT_FILE:
		sprintf( buffer, "/*\n================================================================================\nFileName    : \nDescription : \nDate        : %d-%.2d-%.2d\nAuthor      : Linkrules\n================================================================================\n*/",sys_time.wYear, sys_time.wMonth, sys_time.wDay );
		break;
	}
	

}

void InsertComment( comment_t commentID ) {
	isInsertLock = true;
	HWND hWnd;
	hWnd = GetForegroundWindow();
	if( hWnd != hwnd_main ){
		DWORD fromThreadID = GetCurrentThreadId();
		DWORD cWndThreadID = GetWindowThreadProcessId(hWnd,NULL);
		AttachThreadInput(cWndThreadID,fromThreadID,true);
		hWnd = GetFocus();
		InitCommentStr( commentID );
		OutPutMessage(buffer,hWnd);
		AttachThreadInput(cWndThreadID,fromThreadID,false);
	}
}

void InsertCommentControl (){
    // keydown
	if( KEYDOWN( VK_CONTROL ) && KEYDOWN( VK_MENU ) && KEYDOWN( VK_LSHIFT ) && !isInsertLock ) {
		if( KEYDOWN( VK_L ) ) {                     // line comment
			InsertComment( COMMENT_LINE );
		}else if( KEYDOWN( VK_C ) ) {               // class comment
			InsertComment( COMMENT_CLASS );
		}else if( KEYDOWN( VK_F ) ) {               // function comment
			InsertComment( COMMENT_FUNCTION );
		}else if( KEYDOWN( VK_S ) ) {               // file comment
			InsertComment( COMMENT_FILE );
		}
	}
		
	// keyup
	if( isInsertLock ){
		if( ( KEYUP( VK_CONTROL ) && KEYUP( VK_MENU ) && KEYUP( VK_LSHIFT ) ) && ( KEYUP( VK_L ) || KEYUP( VK_C ) || KEYUP( VK_F ) || KEYUP( VK_S ) ) ) {
			isInsertLock = false;
		}
	}
	
}


// FUNCTION ///////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam ){
	PAINTSTRUCT ps;
	HDC         hdc;
	RECT        rect;
	int         result;

	switch( msg ){
		/*
	case WM_KEYDOWN:                        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! process this
		switch( (int)wparam ){
		case VK_CONTROL:
			if( !isControlPressed ){
				isControlPressed = true;
				printf("control is pressed");
			}			
			break;
		case VK_MENU:
			if( !isAltPressed ){
				isAltPressed = true;
			}
			break;
		case VK_SPACE:
			if( !isSpacePressed ){
				isSpacePressed  = true;
			}
			break;
		case VK_SHIFT:
			
			break;
		}
		break;
	case WM_CHAR:                        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! process this
		switch( wparam ){        // wparam is key of ascii code, type is char
		case 'a':
			break;
		case 'b':
			break;
		}
		break;			
	case WM_KEYUP:                        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! process this
		switch( (int)wparam ){
		case VK_CONTROL:
			isControlPressed = false;
			break;
		case VK_MENU:
			isAltPressed = false;
			break;
		case VK_SPACE:
			isSpacePressed = false;
			break;
		}
		break;
		*/
	 case WM_CREATE:
		{
			//PlaySound(MAKEINTRESOURCE( IDW_WAVE1), hinstance_app, SND_RESOURCE | SND_ASYNC | SND_LOOP);
			// do something
			return 0;
		}
		break;
	case WM_COMMAND:
		{
			switch( LOWORD( wparam ) ){
			case MENU_FILE_ID_OPEN:
				//PlaySound( MAKEINTRESOURCE( IDW_WAVE1 ), hinstance_app, SND_RESOURCE | SND_ASYNC | SND_LOOP );
				break;
			case MENU_FILE_ID_CLOSE:
				//PlaySound( NULL, hinstance_app, SND_PURGE );
				
				break; 
			case MENU_FILE_ID_SAVE:
				HWND hwnd2;
				if( !( hwnd2 = CreateWindowEx( NULL,                               // extended style
								  WINDOW_CLASS_NAME,                  // class
								  "My Window",                        // title
								  WS_OVERLAPPEDWINDOW | WS_VISIBLE,   // 
								  400,0,                                // initial x,y
								  400,400,                            // initial width height
								  NULL,                               // handle to parent
								  NULL,                               // handle to menu
								  hinstance_app,                          // instance of this application
								  NULL )))                           // extra creation parms
			{
				return 0;
			}

				break;
			case MENU_FILE_ID_EXIT:

				break;
			case MENU_HELP_ID_ABOUT:
				MessageBox( hwnd, "Menu from Main window", "About Code Bear", MB_OK | MB_ICONEXCLAMATION );
				break;
			default:
				break;
			}

		}
		break;
	case WM_PAINT:
		{
			//const time_t t = time(NULL);
			//struct tm* current_time = localtime( &t );
			/*
			SYSTEMTIME sys_time;
			GetLocalTime( &sys_time );
			hdc = BeginPaint( hwnd, &ps );
			
			SetTextColor(hdc, RGB( 0, 0, 255) );
			SetBkColor( hdc, RGB( 0,0,0) );
			SetBkMode( hdc, OPAQUE );
			sprintf(buffer, "%d-%.2d-%.2d",sys_time.wYear, sys_time.wMonth, sys_time.wDay );
			TextOut( hdc, 0,0,buffer,strlen(buffer) );
			*/
			//EndPaint( hwnd, &ps );
			//return(0);
		}
		break;
	case WM_CLOSE:
		break;
		result = MessageBox( hwnd, "Are you sure you want to close this application?", "WM_CLOSE Message Processor", MB_YESNO | MB_ICONQUESTION );
		if( result == IDYES ){
			//return (DefWindowProc( hwnd, msg, wparam, lparam) );
			break;
		}
		else{
			return 0;
		}
		break;
	case WM_DESTROY:
		{
			// kill the application , this sends a WM_QUIT message
			if( hwnd == hwnd_main ){
				PostQuitMessage( 0 );
			}
			return (0);
		}
		break;
	default:
		break;
	}
	//InsertCommentControl();
	return( DefWindowProc( hwnd, msg, wparam, lparam ) );
}        // end winProc

int WINAPI WinMain( HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow ){
	hinstance_app = hinstance;
	WNDCLASSEX winclass;        // this will hold the class we create
	HWND      hwnd;            // generic window handle
	
	MSG       msg;             // generic message
	
	// first fill in the window class structure
	winclass.cbSize			= sizeof( WNDCLASSEX );
	winclass.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(hinstance, MAKEINTRESOURCE( IDI_ICON1 ));
	winclass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	winclass.hbrBackground	= (HBRUSH)GetStockObject( BLACK_BRUSH );
	winclass.lpszMenuName   = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.hIconSm		= LoadIcon( hinstance, MAKEINTRESOURCE( IDI_ICON1 ));

	// register the window class
	if( !RegisterClassEx( &winclass ) ){
		return ( 0 );
	}
	/*
	// create the window
	if( !( hwnd = CreateWindowEx( NULL,                               // extended style
								  WINDOW_CLASS_NAME,                  // class
								  "My Window",                        // title
								  WS_OVERLAPPEDWINDOW | WS_VISIBLE,   // 
								  0,0,                                // initial x,y
								  400,400,                            // initial width height
								  NULL,                               // handle to parent
								  NULL,                               // handle to menu
								  hinstance,                          // instance of this application
								  NULL )))                           // extra creation parms
	{
		return 0;
	}

	hwnd_main = hwnd;
	

	HMENU hmenuhandle = LoadMenu( hinstance, "MainMenu" );
	SetMenu( hwnd, hmenuhandle );

	//HDC hdc = GetDC( hwnd );
	*/
	
	while( TRUE ){
		InsertCommentControl();
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ){
			if( msg.message == WM_QUIT ){
				break;
			}
			
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		Sleep(1);
	}

	

	return (msg.wParam);
}        // end WinMain

