// Sceen Space Reflect.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Sceen Space Reflect.h"
#include "..\RenderCore\Direct3D11.h"
#include "..\Logic\\GameLogic.h"

#define MAX_LOADSTRING 100

#define FRAME_BUFFER_WIDTH			1024
#define FRAME_BUFFER_HEIGHT			768

#pragma comment( lib, "../lib/Logic.lib")

namespace
{
	CGameLogic gGameLogic;
}

// ���� ����:
HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
TCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM				MyRegisterClass ( HINSTANCE hInstance );
BOOL				InitInstance ( HINSTANCE, int );
LRESULT CALLBACK	WndProc ( HWND, UINT, WPARAM, LPARAM );
INT_PTR CALLBACK	About ( HWND, UINT, WPARAM, LPARAM );

int APIENTRY _tWinMain ( _In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow )
{
	UNREFERENCED_PARAMETER ( hPrevInstance );
	UNREFERENCED_PARAMETER ( lpCmdLine );

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;
	HACCEL hAccelTable;

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadString ( hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING );
	LoadString ( hInstance, IDC_SCEENSPACEREFLECT, szWindowClass, MAX_LOADSTRING );
	MyRegisterClass ( hInstance );

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if ( !InitInstance ( hInstance, nCmdShow ) )
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators ( hInstance, MAKEINTRESOURCE ( IDC_SCEENSPACEREFLECT ) );

	// �⺻ �޽��� �����Դϴ�.
	while ( 1 )
	{
		if ( PeekMessage ( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( msg.message == WM_QUIT )
				break;
			if ( !TranslateAccelerator ( msg.hwnd, hAccelTable, &msg ) )
			{
				if ( gGameLogic.HandleWindowMessage( msg ) )
				{
					//Do Nothing
				}
				else
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
			}
		}
		
		gGameLogic.UpdateLogic( );
	}

	return (int)msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass ( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon ( hInstance, MAKEINTRESOURCE ( IDI_SCEENSPACEREFLECT ) );
	wcex.hCursor = LoadCursor ( NULL, IDC_ARROW );
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE ( IDC_SCEENSPACEREFLECT );
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon ( wcex.hInstance, MAKEINTRESOURCE ( IDI_SMALL ) );

	return RegisterClassEx ( &wcex );
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance ( HINSTANCE hInstance, int nCmdShow )
{
	HWND hWnd;

	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect( &rc, dwStyle, true );

	hWnd = CreateWindow( szWindowClass, szTitle, dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL );

	if ( !hWnd )
	{
		return FALSE;
	}

	if ( !gGameLogic.Initialize( hWnd, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT ) )
	{
		return false;
	}

	ShowWindow ( hWnd, nCmdShow );
	UpdateWindow ( hWnd );

	return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND	- ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT	- �� â�� �׸��ϴ�.
//  WM_DESTROY	- ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc ( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch ( message )
	{
	case WM_COMMAND:
		wmId = LOWORD ( wParam );
		wmEvent = HIWORD ( wParam );
		// �޴� ������ ���� �м��մϴ�.
		switch ( wmId )
		{
		case IDM_ABOUT:
			DialogBox ( hInst, MAKEINTRESOURCE ( IDD_ABOUTBOX ), hWnd, About );
			break;
		case IDM_EXIT:
			DestroyWindow ( hWnd );
			break;
		default:
			return DefWindowProc ( hWnd, message, wParam, lParam );
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint ( hWnd, &ps );
		// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
		EndPaint ( hWnd, &ps );
		break;
	case WM_DESTROY:
		PostQuitMessage ( 0 );
		break;
	default:
		return DefWindowProc ( hWnd, message, wParam, lParam );
	}
	return 0;
}

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK About ( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
	UNREFERENCED_PARAMETER ( lParam );
	switch ( message )
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if ( LOWORD ( wParam ) == IDOK || LOWORD ( wParam ) == IDCANCEL )
		{
			EndDialog ( hDlg, LOWORD ( wParam ) );
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
