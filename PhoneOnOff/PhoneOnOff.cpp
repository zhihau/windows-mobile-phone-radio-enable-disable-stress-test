//======================================================================================
// MODULE NAME: MainForm
// FILE NAME: MainForm.c
// DATE: 2008/4/7
// VERSION: 1.00
// PURPOSE: For phone on off
//
// AUTHEROR: Zhihau Shiu    REVIEWED by
//
// FUNCTIONS
//
//		MyRegisterClass		Registers the window class.
//		InitInstance		Saves instance handle and creates main window
//		WndProc			Processes messages for the main window.
//
// DEPENDENCES
//
//      	PhoneOnOff.h            To include resourcesp.h
//      	resourcesp.h            To define ID
//		stdafx.h		include file for standard system include files
//		wrlspwr.h
//		WirelessDevices.h		
//
// * $History: $
// MODIFICTION HISTORY:
// Version 1.00 - Date 2008/4/7 By Zhihau Shiu
//		Set the time interval between each phone radio on and off action.
// ** This is the first versoin for creating to support the functions of
//    current module, or the first revision based on the original file.
// [DEFECTS CORRECCTED/NEW FEATURES]
// Version 1.10 - Date 2008/4/8 By Zhihau Shiu
//		Set the number of retries if the phone radio cannot be turned off.
//		Set the number of tests (set timer count)
//
//======================================================================================


// RadioDevices.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PhoneOnOff.h"
#include <windows.h>
#include <commctrl.h>
#include "WirelessDevices.h"

#define LIMIT_EDIT_MAXSIZE 4
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE			g_hInst;		// Current instance
HWND				g_hWndMenuBar;		// Menu bar handle
TCHAR				szText[260];		// Text to display process infomation
DWORD				dwInterval=10;
DWORD				dwRetryTimes=0;		// Phone off retry times
DWORD				dwTimerCount=-1;    	// Set timer count
SYSTEMTIME			stTime;

// Forward declarations of functions included in this code module:
ATOM			MyRegisterClass(HINSTANCE, LPTSTR);
BOOL			InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
	MSG msg;

	// Load ossvcs.dll
	InitDLL();

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RADIODEVICES));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Free ossvcs.dll
	DeinitDLL();

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
ATOM MyRegisterClass(HINSTANCE hInstance, LPTSTR szWindowClass)
{
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RADIODEVICES));
	wc.hCursor       = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = szWindowClass;

	return RegisterClass(&wc);
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
    TCHAR szTitle[MAX_LOADSTRING];		// title bar text
    TCHAR szWindowClass[MAX_LOADSTRING];	// main window class name
	HWND hEditCheckTime;
    g_hInst = hInstance;                        // Store instance handle in our global variable

    // SHInitExtraControls should be called once during your application's initialization to initialize any
    // of the device specific controls such as CAPEDIT and SIPPREF.
    SHInitExtraControls();

    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING); 
    LoadString(hInstance, IDC_RADIODEVICES, szWindowClass, MAX_LOADSTRING);

    //If it is already running, then focus on the window, and exit
    hWnd = FindWindow(szWindowClass, szTitle);	
    if (hWnd) 
    {
        // set focus to foremost child window
        // The "| 0x00000001" is used to bring any owned windows to the foreground and
        // activate them.
        SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
        return 0;
    } 

    if (!MyRegisterClass(hInstance, szWindowClass))
    {
    	return FALSE;
    }

    hWnd = CreateWindow(szWindowClass, szTitle, WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

	// Create Child controls
	CreateWindow(TEXT("listbox"), 0, 
            WS_CHILD | WS_VISIBLE | WS_BORDER|WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
            10, 10, 220, 140, hWnd, (HMENU)IDE_LOG, hInstance, 0);
	CreateWindow(TEXT("edit"),TEXT("check time interval(sec):"),WS_VISIBLE|WS_CHILD,
		10,160,170,20,
		hWnd,(HMENU)IDL_CHECKTIME,hInstance,0);
	hEditCheckTime = CreateWindow(TEXT("edit"),TEXT("10"),WS_VISIBLE|WS_CHILD|WS_BORDER,
		180,160,50,20,
		hWnd,(HMENU)IDE_CHECKTIME,hInstance,0);
	CreateWindow(TEXT("edit"),TEXT("retry times:"),WS_VISIBLE|WS_CHILD,
		10,190,170,20,
		hWnd,(HMENU)IDL_RETRYTIMES,hInstance,0);
	CreateWindow(TEXT("edit"),TEXT("10"),WS_VISIBLE|WS_CHILD|WS_BORDER,
		180,190,50,20,
		hWnd,(HMENU)IDE_RETRYTIMES,hInstance,0);

	CreateWindow(TEXT("edit"),TEXT("set timer count:"),WS_VISIBLE|WS_CHILD,
		10,220,170,20,
		hWnd,(HMENU)IDL_TIMERCOUNT,hInstance,0);
	CreateWindow(TEXT("edit"),TEXT("10"),WS_VISIBLE|WS_CHILD|WS_BORDER,
		180,220,50,20,
		hWnd,(HMENU)IDE_TIMERCOUNT,hInstance,0);
	SetFocus(hEditCheckTime);

    // When the main window is created using CW_USEDEFAULT the height of the menubar (if one
    // is created is not taken into account). So we resize the window after creating it
    // if a menubar is present
    if (g_hWndMenuBar)
    {
        RECT rc;
        RECT rcMenuBar;

        GetWindowRect(hWnd, &rc);
        GetWindowRect(g_hWndMenuBar, &rcMenuBar);
        rc.bottom -= (rcMenuBar.bottom - rcMenuBar.top);
		
        MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, FALSE);
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
	DWORD	dwWifi, dwPhone, dwBT, dwListboxCount;
	LPSYSTEMTIME lpSystemTime=&stTime ;


    static SHACTIVATEINFO s_sai;
	
    switch (message) 
    {
        case WM_COMMAND:
            wmId    = LOWORD(wParam); 
            wmEvent = HIWORD(wParam); 
            // Parse the menu selections:
            switch (wmId)
            {
		case IDM_HELP_START:
			dwInterval = GetDlgItemInt (hWnd, IDE_CHECKTIME , NULL,  LIMIT_EDIT_MAXSIZE);

			// Set timer to set on off for phone
			SetTimer(hWnd, IDT_PHONEONOFFTIMER, dwInterval*1000, NULL);

			SendDlgItemMessage(hWnd, IDE_LOG, LB_RESETCONTENT, 0, 0);
			break;

		case IDM_HELP_STOP:
			KillTimer(hWnd, IDT_PHONEONOFFTIMER);
			dwTimerCount=-1;
			break;

                case IDM_HELP_ABOUT:
			MessageBox(hWnd,TEXT("PhoneOnOff \n(C)2008 , Version: V1.1.0\n\nVisit\nhttps://github.com/zhihau\nfor other interesting samples or articles!"),
			TEXT("About"),MB_ICONINFORMATION);
                    break;

                case IDM_OK:
                    SendMessage (hWnd, WM_CLOSE, 0, 0);				
                    break;

                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_CREATE:
            SHMENUBARINFO mbi;

            memset(&mbi, 0, sizeof(SHMENUBARINFO));
            mbi.cbSize     = sizeof(SHMENUBARINFO);
            mbi.hwndParent = hWnd;
            mbi.nToolBarId = IDR_MENU;
            mbi.hInstRes   = g_hInst;

            if (!SHCreateMenuBar(&mbi)) 
            {
                g_hWndMenuBar = NULL;
            }
            else
            {
                g_hWndMenuBar = mbi.hwndMB;
            }

            // Initialize the shell activate info structure
            memset(&s_sai, 0, sizeof (s_sai));
            s_sai.cbSize = sizeof (s_sai);
			
            break;
		case WM_TIMER:
			dwTimerCount++;
			
			KillTimer(hWnd,IDT_PHONEONOFFTIMER );

			if (dwTimerCount==GetDlgItemInt (hWnd,IDE_TIMERCOUNT,NULL,LIMIT_EDIT_MAXSIZE))
			{
				/*GetLocalTime(lpSystemTime);
				_stprintf(szText, _T("%d:%d:%d| %d"), lpSystemTime->wHour  ,lpSystemTime->wMinute   ,
							lpSystemTime->wSecond ,TEXT("Finished"));*/
				_stprintf(szText, _T("%d| %s"), dwTimerCount ,TEXT("Finished"));
				SendDlgItemMessage(hWnd, IDE_LOG, LB_ADDSTRING, 0, (LPARAM)szText);
				dwTimerCount=-1;
				return 0;
			}
			
			//dwPhone=0 , current phone off, dwPhone=1 ,current phone on
			GetWDevState(&dwWifi, &dwPhone, &dwBT);
			_stprintf(szText, _T("%d| state %d"),  dwTimerCount ,dwPhone);
			SendDlgItemMessage(hWnd, IDE_LOG, LB_ADDSTRING, 0, (LPARAM)szText);
			//phone on
			if(dwPhone!=0)
			{
				//Set phone on off
				dwPhone = SetWDevState( RADIODEVICES_PHONE, 0);
				if (dwPhone==-1)
				{
					_stprintf(szText, _T("%d| %s"), dwTimerCount,TEXT("phone off: Unspecified failure. " ));
				}	
				else if(dwPhone==-2)
				{
					_stprintf(szText, _T("%d| %s"), dwTimerCount,TEXT("phone off: Insufficient memory" ));
				}
				else if(dwPhone=-3)
				{
					_stprintf(szText, _T("%d| %s"), dwTimerCount,TEXT("phone off: ChangeRadioState fail" ));
				}
				else //dwPhone=0
				{
					_stprintf(szText, _T("%d| %s"), dwTimerCount,TEXT("phone off: ok" ));
				}
				SendDlgItemMessage(hWnd, IDE_LOG, LB_ADDSTRING, 0, (LPARAM)szText);
				dwPhone=0;
			}

			//dwPhone=0 : phone off
			while (!dwPhone)
			{
				dwPhone = SetWDevState( RADIODEVICES_PHONE, 1);
				
				GetLocalTime(lpSystemTime);
				if (dwPhone==-1)
				{
					_stprintf(szText, _T("%d| %s"),dwTimerCount ,TEXT("Phone on: Unspecified failure."));
				}
				else if (dwPhone=-2)
				{
					_stprintf(szText, _T("%d| %s"),dwTimerCount,TEXT("Phone on: Insufficient memory"));
				}
				else if(dwPhone=-3)
				{
					_stprintf(szText, _T("%d| %s"), dwTimerCount ,TEXT("Phone on: ChangeRadioState fail"));
				}
				else
				{
					_stprintf(szText, _T("%d| %s"), dwTimerCount,TEXT("Phone on: ok."));
				}
				SendDlgItemMessage(hWnd, IDE_LOG, LB_ADDSTRING, 0, (LPARAM)szText);
				
				
				dwListboxCount = SendDlgItemMessage(hWnd, IDE_LOG, LB_GETCOUNT, 0, 0);
				SendDlgItemMessage(hWnd, IDE_LOG, LB_SETCURSEL , dwListboxCount-1, 0);
				dwRetryTimes++;

				if(dwPhone==0)
				{
					GetLocalTime(lpSystemTime);
					dwPhone = SetWDevState( RADIODEVICES_PHONE, 0);
					if (dwPhone==-1)
					{
						_stprintf(szText, _T("%d| %s"), dwTimerCount ,TEXT("phone off: Unspecified failure. " ));
					}	
					else if(dwPhone==-2)
					{
						_stprintf(szText, _T("%d| %s"), dwTimerCount ,TEXT("phone off: Insufficient memory" ));
					}
					else if(dwPhone=-3)
					{
						_stprintf(szText, _T("%d| %s"), dwTimerCount ,TEXT("phone off: ChangeRadioState fail" ));
					}
					else //dwPhone=0
					{
						_stprintf(szText, _T("%d| %s"), dwTimerCount ,TEXT("phone off: ok" ));
					}
					SendDlgItemMessage(hWnd, IDE_LOG, LB_ADDSTRING, 0, (LPARAM)szText);
					break;
				}
				if(dwRetryTimes==GetDlgItemInt (hWnd,IDE_RETRYTIMES,NULL,LIMIT_EDIT_MAXSIZE))
				{
					_stprintf(szText, _T("%d| %s"), dwTimerCount,TEXT("Reach the limited times of retry"));
					SendDlgItemMessage(hWnd, IDE_LOG, LB_ADDSTRING, 0, (LPARAM)szText);
					dwRetryTimes=0;
					break;
				}
			}
			
			dwInterval = GetDlgItemInt (hWnd, 
                            IDE_CHECKTIME , 
                            NULL, 
                            LIMIT_EDIT_MAXSIZE);

			// Set timer to set on off for phone
			SetTimer(hWnd, IDT_PHONEONOFFTIMER, dwInterval*1000, NULL);
			
			break;
			
        case WM_DESTROY:
            CommandBar_Destroy(g_hWndMenuBar);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}



