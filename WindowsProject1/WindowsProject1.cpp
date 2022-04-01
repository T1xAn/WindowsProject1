// WindowsProject1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100

DWORD error;
// Глобальные переменные:

HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND leftText;                                  // Левое дочернее окно
WCHAR ltWindowClass[MAX_LOADSTRING];            // Имя класса левого дочернего окна вывода
WCHAR ltTitle[MAX_LOADSTRING];                  // Текст строки заголовка левого дочернего окна

static HWND search;
static HWND textbox;
static HWND ReadButton;


////
HDC hdcLF;
 OPENFILENAME File;
 wchar_t buf[100];
 HANDLE LeftFile_a;
 HANDLE LeftFile;
 char* LRFILE;
// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
ATOM            LTRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    LeftProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//
// Функция buttonGetFile(OPENFILENAME F) 
//  Позволяет произвести поиск нужного файла через провдник
//  return Возвращает рассположение файла на компьютере
LPWSTR buttonGetFile(OPENFILENAME F) {
    wchar_t szFile[1000];
    ZeroMemory(&File, sizeof(File));
    F.lStructSize = sizeof(OPENFILENAME);
    F.hwndOwner = NULL;
    F.lpstrFile = szFile;
    F.lpstrFile[0] = '\0';
    F.nMaxFile = sizeof(szFile);
    F.lpstrFilter = L"All Files\0*.*\0\0"; //"All\0*.*\0Text\0*.TXT\0";
    F.nFilterIndex = 1;
    F.lpstrFileTitle = NULL;
    F.nMaxFileTitle = 0;
    F.lpstrInitialDir = NULL;
    F.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    GetOpenFileName(&F);
    MessageBox(NULL, F.lpstrFile, L"File Name", MB_OK);
    return F.lpstrFile;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
  
    LoadStringW(hInstance, IDS_LT_TITLE, ltTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LEFTTEXT, ltWindowClass, MAX_LOADSTRING);
    LTRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

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
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = NULL;//CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM LTRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex1;

    wcex1.cbSize = sizeof(WNDCLASSEX);

    wcex1.style = CS_NOCLOSE;
    wcex1.lpfnWndProc = LeftProc;
    wcex1.cbClsExtra = 0;
    wcex1.cbWndExtra = 0;
    wcex1.hInstance = hInstance;
    wcex1.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex1.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex1.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex1.lpszMenuName = MAKEINTRESOURCEW(IDC_LEFTTEXT);
    wcex1.lpszClassName = ltWindowClass;
    wcex1.hIconSm = LoadIcon(wcex1.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    /*if (!&wcex1) return false;

    DWORD  error = GetLastError();*/
    return RegisterClassExW(&wcex1);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_MAXIMIZE|  WS_CLIPCHILDREN,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   search = CreateWindowA("button", ">>", WS_CHILD |
       WS_VISIBLE | WS_BORDER, 1050, 20, 30, 30, hWnd, (HMENU)1000, hInstance, nullptr);
    textbox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("//"),
       WS_CHILD | WS_VISIBLE, 30, 20, 1000,
       30, hWnd, NULL, NULL, NULL);
    ReadButton = CreateWindowA("button", "o", WS_CHILD |
        WS_VISIBLE | WS_BORDER, 1100, 20, 30, 30, hWnd, (HMENU)1001, hInstance, nullptr);
   RECT rect;
    GetClientRect(hWnd, &rect);
   leftText = CreateWindowW(ltWindowClass, ltTitle,WS_CHILD | WS_VSCROLL| WS_BORDER | WS_CLIPSIBLINGS, 
      0, 80, rect.right/2, rect.bottom -80, hWnd, nullptr, hInstance, nullptr);

    
   if (!hWnd)
   {
      return FALSE;
   }

   if (!leftText)
   {
       return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   ShowWindow(leftText, nCmdShow);
   UpdateWindow(leftText);
   return TRUE;
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
   /* case WM_SIZE:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        MoveWindow(leftText, 0, 80, rc.right / 2, rc.bottom - 80, FALSE);
        break;
    }*/
    case WM_COMMAND:
        {
            //size_t result;
            
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            LPWSTR fn;
            
            switch (wmId)
            {
            case 1000:
                fn = buttonGetFile(File);
                Edit_SetText(textbox, fn);
                break;
            case 1001:
                
                wchar_t fn1[1000];
                Edit_GetText(textbox,fn1 , 1000);
                MessageBox(NULL, fn1, L"File Name in TextBox:", MB_OK);
                LeftFile_a =  CreateFile(fn1,GENERIC_READ,FILE_SHARE_READ, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
                LeftFile = CreateFileMapping(LeftFile_a, NULL, PAGE_READONLY,0, 0,NULL);
                LRFILE = (char*) MapViewOfFile(LeftFile,FILE_MAP_READ,0,0,0);
                hdcLF = GetDC(leftText);
                ///*buf[2];
                //buf[0] = ;*/
                //buf[1] = '\0';
                for (int i = 0; i < 16; i = i+2) {
                   
                    TextOutA(hdcLF, 20 + i, 20, (LPCSTR)LRFILE, strlen(LRFILE));
                  
                }
                ReleaseDC(leftText, hdcLF);

                

                UnmapViewOfFile(LRFILE);
                CloseHandle(LeftFile);
                CloseHandle(LeftFile_a);
                UpdateWindow(leftText);
                break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
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
            /////////////
            RECT rect;
            /*GetClientRect(hWnd, &rect);
            DrawTextW(hdc,L"Вывело", -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);*/
            //TextOutA(hdc, 200, 200,L"rery", 4);
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

//
//  ФУНКЦИЯ: LeftProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK LeftProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
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
       /* GetClientRect(hWnd, &rect);
        DrawTextW(hdc, buf, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);*/
        //UINT a = GetTextAlign(hdc);
        //TextOutA(hdc, 20,20, buf, wcslen(buf));
        // TODO: Добавьте сюда любой код прорисовки, использующий HDC..
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
