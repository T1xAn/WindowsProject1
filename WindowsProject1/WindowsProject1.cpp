// WindowsProject1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WindowsProject1.h"
#include "Functions_elements.h"
#define MAX_LOADSTRING 100

DWORD error;
// Глобальные переменные:

 HANDLE LeftFile;
 HANDLE RightFile;
 ScrollFileInfo ScrolledFilesInfo;
 MainWindows WindowInfo;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    WindowInfo.RegisterStrings(hInstance);
    WindowInfo.RegisterWindowClasses(hInstance);

    // Выполнение инициализации приложения
    if (!WindowInfo.CreateAllWindow(hInstance, nCmdShow))
        return FALSE;

    ScrolledFilesInfo.GetSystemGranularity();
 

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;
  
    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, 
    UINT message, 
    WPARAM wParam, 
    LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        RECT rect;
        GetClientRect(hWnd, &rect);
        int screen_height = GetSystemMetrics(SM_CYSCREEN);
        WindowInfo.LeftTextWindow = CreateWindowW((LPCWSTR)WindowInfo.GetLeftWindowClass(), (LPCWSTR)WindowInfo.GetLeftWindowTitle()
            , WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_BORDER,
            0, ceil(rect.bottom*0.1), rect.right / 2, ceil(rect.bottom*0.9), hWnd, nullptr, WindowInfo.hInst, nullptr);

        WindowInfo.ToolBar = CreateWindowW((LPCWSTR)WindowInfo.GetToolBarClass(), NULL, WS_CHILD | WS_BORDER, 0, 0,
            rect.right, ceil(rect.bottom*0.1), hWnd, nullptr, WindowInfo.hInst, nullptr);

        WindowInfo.RightTextWindow = CreateWindowW((LPCWSTR)WindowInfo.GetRightWindowClass(), NULL
            , WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_BORDER,
            rect.right/2, ceil(rect.bottom * 0.1), rect.right / 2, ceil(rect.bottom * 0.9), hWnd, nullptr, WindowInfo.hInst, nullptr);
      error =   GetLastError();
      
      WindowInfo.m_UpdatingWindows[0] = WindowInfo.LeftTextWindow;
      WindowInfo.m_UpdatingWindows[1] = WindowInfo.RightTextWindow;

        HFONT FONT = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
        ScrolledFilesInfo.GetTextMetric(WindowInfo.LeftTextWindow, FONT);

        for (int i = 0; i < 2; i++) {
        ShowScrollBar(WindowInfo.m_UpdatingWindows[i], SB_HORZ, FALSE);
        SetScrollRange(WindowInfo.m_UpdatingWindows[i], SB_VERT, 0, 1000, FALSE);
        }
        
    }
    case WM_SIZE:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);

        int Screen_height = GetSystemMetrics(SM_CYFULLSCREEN);
        int Menu_height = GetSystemMetrics(SM_CYMENUSIZE);
        MoveWindow(WindowInfo.LeftTextWindow, 0, ceil((Screen_height - Menu_height)*0.1), rc.right / 2, ceil(rc.bottom-(Screen_height - Menu_height) * 0.1), TRUE);
        MoveWindow(WindowInfo.ToolBar, 0, 0, rc.right, ceil((Screen_height - Menu_height) * 0.1), TRUE);
        MoveWindow(WindowInfo.RightTextWindow, rc.right/2, ceil((Screen_height - Menu_height) * 0.1), rc.right / 2, ceil(rc.bottom - (Screen_height - Menu_height) * 0.1), TRUE);
        break;
    }
    case WM_COMMAND:
        {
      
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(WindowInfo.hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                if (LeftFile != NULL) CloseHandle(LeftFile);
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
       
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
           
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
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

// Обработчик сообщений для окна "О программе".
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
