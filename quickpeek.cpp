// quickpeek.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "quickpeek.h"
#include "preview.h"
#include "Layout.h"
#include <dwmapi.h>
#include <vector>
#include <algorithm>
#include "PreviewDragger.h"
using namespace std;

std::vector<CPreview> g_previews;
bool g_arrange_mode = false;
PreviewDragger g_dragger;


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// thumbnail handling
HRESULT hr = S_OK;


int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_QUICKPEEK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QUICKPEEK));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


void DrawPreview(const CPreview& in)
{
	in.Draw();
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QUICKPEEK));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_QUICKPEEK);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	CLayout layout;
	std::vector<CPreview> res = layout.ReadConfig(hWnd, "multibox.txt");
	g_previews.insert(g_previews.end(), res.begin(), res.end());

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

struct MouseProcessor 
{
	MouseProcessor( UINT message, WPARAM wParam, LPARAM lParam) : 
m_message(message), m_l_param(lParam), m_w_param(wParam), m_accepted (false)
{
}

void operator()(  CPreview& in)
{
	if (!m_accepted)
		m_accepted = in.AcceptMouse( m_message, m_w_param, m_l_param);
}
bool m_accepted;
UINT m_message;
WPARAM m_w_param;
LPARAM m_l_param;
};

CPreview * GetActivePreview(LPARAM lParam) 
{
	for (unsigned i = 0 ;i < g_previews.size() ; i ++ )
	{
		CPreview* ptr = &g_previews[i];
		if (ptr->IsActive(lParam))
		{
			return ptr;
		}

	}
	return NULL;
}

struct KeyProcessor 
{

	KeyProcessor( UINT message, WPARAM wParam, LPARAM lParam) : 
m_message(message), m_l_param(lParam), m_w_param(wParam)
{
}

void operator()(  CPreview& in)
{
	in.AcceptKeyboard( m_message, m_w_param, m_l_param);
}
UINT m_message;
WPARAM m_w_param;
LPARAM m_l_param;
};

BOOL CALLBACK wnd_enumerator(HWND wnd, LPARAM param )
{
	char text[256];
	std::vector<std::string>* list = (std::vector<std::string>*) param;

	HRESULT err = GetWindowTextA(wnd, text, 255);

	if (strlen(text) && text[0] =='E' && text[1] =='V' && text[2] =='E' )
	{
		list->push_back(std::string(text));
	}
	
	return TRUE;
}

void ProcessSelectionChange( LPARAM lParam );

LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		{
			std::vector<std::string> wnd_text;
			EnumWindows(wnd_enumerator, (LPARAM) &wnd_text);
			HWND combo = GetDlgItem(hWndDlg, IDC_WINDOWS);
			for (size_t i = 0 ;i < wnd_text.size() ; i ++ )
			{
				const char * msg = wnd_text[i].c_str();
				DWORD dwIndex = SendMessage(combo,
					CB_INSERTSTRING,
					0,
					reinterpret_cast<LPARAM>((LPCTSTR)msg));
			}
		}
		break;
	
	case WM_COMMAND:
		{
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				ProcessSelectionChange(lParam);

			}

			switch(wParam)
			{
			case IDCANCEL:
				EndDialog(hWndDlg, 0);
				return TRUE;
			}

		}
		break;
	}

	return FALSE;
}


void AddPreview(HWND hWnd) 
{
	DialogBox(hInst, MAKEINTRESOURCE(IDD_ADD_PREVIEW),
		hWnd, reinterpret_cast<DLGPROC>(DlgProc));
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	
	if (!g_arrange_mode && message >= WM_MOUSEFIRST && message <= WM_MOUSELAST && GetFocus() == hWnd)
	{
		MouseProcessor proc(message, wParam, lParam);
		for_each(g_previews.rbegin(), g_previews.rend(), proc);
		return 0 ;
	}
	if (!g_arrange_mode &&  message >= WM_KEYFIRST && message <= WM_KEYLAST&& GetFocus() == hWnd)
	{
		KeyProcessor proc(message, wParam, lParam);
		for_each(g_previews.rbegin(), g_previews.rend(), proc);
	}
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_SAVE:
			{
				CLayout layout;
				layout.SaveConfig("multibox.txt", g_previews);
			}
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case ID_PREVIEWS_ADD:
			{
				AddPreview(hWnd);
			}
			break;
		case ID_FILE_ARRANGE:
			{
				HMENU hMenu = GetMenu(hWnd);
				UINT f = GetMenuState(hMenu, ID_FILE_ARRANGE , MF_BYCOMMAND );
				if (f & MF_CHECKED )
				{
					g_arrange_mode = false;
					
					CheckMenuItem(hMenu, ID_FILE_ARRANGE, 
						MF_BYCOMMAND | MF_UNCHECKED); 
				}
				else
				{
					g_arrange_mode = true;
					CheckMenuItem(hMenu, ID_FILE_ARRANGE, 
						MF_BYCOMMAND | MF_CHECKED); 

				}
			}
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			LPCTSTR cursor = MAKEINTRESOURCE(IDC_CROSS);
			SetCursor(LoadCursor(hInst,cursor));
			CPreview * p_preview = GetActivePreview(lParam);
			g_dragger.SetActivePreview(p_preview);
		}
		break;
	case WM_LBUTTONUP:
		{
			LPCTSTR cursor = MAKEINTRESOURCE(IDC_ARROW);
			SetCursor(LoadCursor(hInst,cursor));
			g_dragger.SetActivePreview(NULL);
		}
		break;
	case WM_MOUSEMOVE:
		{
			int  xPos = GET_X_LPARAM(lParam); 
			int  yPos = GET_Y_LPARAM(lParam); 
			if (GetKeyState(VK_LBUTTON) & 0x8000)
				g_dragger.ProcessMove(xPos, yPos);
		}
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			for_each(g_previews.rbegin(), g_previews.rend(), DrawPreview);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void ProcessSelectionChange( LPARAM lParam )
{
	char  window_name[256];
	int item_index = SendMessage((HWND) lParam, (UINT) CB_GETCURSEL, 
		(WPARAM) 0, (LPARAM) 0);
	SendMessage((HWND) lParam, (UINT) CB_GETLBTEXT, 
		(WPARAM) item_index, (LPARAM) window_name);
	HWND wnd = FindWindow(NULL, window_name);


}
