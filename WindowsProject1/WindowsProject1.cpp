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
HWND LeftTextWindow;                                  // Левое дочернее окно
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

 HFONT FONT;

 //PBYTE LRFILE;
// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
ATOM            LTRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    LeftProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
SIZE FindMaxTextOffset(HFONT FONT, HDC hdc);
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
    //MessageBox(NULL, F.lpstrFile, L"File Name", MB_OK);
    return F.lpstrFile;
}

BOOL GetEightBitsHex(HANDLE LeftFile, DWORD Granularity, _int64 FileSize) {
    //std::stringstream ss; 
   
    DWORD OFFSET = 0, Block = 0, height = 0;
    char BufferString[100] = "";
    hdcLF = GetDC(LeftTextWindow);
    while (FileSize > 0) { 
        Block = Granularity;
        if (Block > FileSize);
            Block = FileSize;
        PBYTE LRFILE = (PBYTE)MapViewOfFile(LeftFile, FILE_MAP_READ, 0, OFFSET, FileSize);
        //FONT = (HFONT)GetStockObject(SYSTEM_FONT);
        //SelectObject(hdcLF, FONT);
      // if (LRFILE == NULL) {MessageBox(NULL, (wchar_t*)GetLastError(), L"Произошла ошибка при создании проекции:", MB_OK); return FALSE; }
        DWORD i = 0;
        int FirstOffset = 0;
          SIZE STRSIZE;
          LPSIZE Size = &STRSIZE;
          int StrNumOffset = snprintf(BufferString, sizeof(BufferString), "%X", OFFSET + Block);
          for(int i = 0; i < StrNumOffset; i++) snprintf(BufferString+i, sizeof(BufferString)-i, "%C", 0x44 );
          snprintf(BufferString + StrNumOffset, sizeof(BufferString) - i-1, " ");
          GetTextExtentPoint32A(hdcLF, (LPCSTR)BufferString, strlen(BufferString), Size);
          FirstOffset = Size->cx;
          STRSIZE = FindMaxTextOffset(FONT, hdcLF);  
           
        while (i  < Block) {
           int BufferOffset = snprintf(BufferString, sizeof(BufferString), "%0*X ",StrNumOffset, i + OFFSET);
                BufferOffset += snprintf(BufferString + BufferOffset, sizeof(BufferString) - BufferOffset , ":  %02X %02X %02X %02X %02X %02X %02X %02X", LRFILE[i], LRFILE[i + 1], LRFILE[i + 2], LRFILE[i + 3], LRFILE[i + 4], LRFILE[i + 5], LRFILE[i + 6], LRFILE[i + 7]);

           
           TextOutA(hdcLF, 5, height, (LPCSTR)BufferString, strlen(BufferString));
           snprintf(BufferString , sizeof(BufferString) , "| %C %C %C %C %C %C %C %C", LRFILE[i], LRFILE[i + 1], LRFILE[i + 2], LRFILE[i + 3], 
               LRFILE[i + 4], LRFILE[i + 5], LRFILE[i + 6], LRFILE[i + 7]);
           TextOutA(hdcLF, 5 + FirstOffset + STRSIZE.cx, height, (LPCSTR)BufferString, strlen(BufferString));
           i+=8;
           height += STRSIZE.cy;
        }
       
        FileSize -= Block;
        OFFSET += Block;
        UnmapViewOfFile(LRFILE);
    }
    MessageBox(NULL, nullptr, L"Проекция завершена", MB_OK);
    return true;
}

SIZE FindMaxTextOffset(HFONT FONT , HDC hdc) {
    if (!FONT) {
        FONT = (HFONT)GetStockObject(SYSTEM_FONT);
        SelectObject(hdc, FONT);
    }
    char buf[60];
    int max = 0, maxSymbol;
    SIZE STRSIZE;
    LPSIZE Size = &STRSIZE;

    for (int Symbol = 0x41; Symbol < 0x47; Symbol++) {
        snprintf(buf, sizeof(buf), "%C", Symbol);
        GetTextExtentPoint32A(hdc, (LPCSTR)buf, strlen(buf), Size);
        if (Size->cx > max) { max = Size->cx; maxSymbol = Symbol; }
    }
    snprintf(buf, sizeof(buf),": %C%C %C%C %C%C %C%C %C%C %C%C %C%C %C%C", maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol,
        maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol);
    GetTextExtentPoint32A(hdc, (LPCSTR)buf, strlen(buf), Size);
    return STRSIZE;
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
    LoadStringW(hInstance, IDC_LeftTextWindow, ltWindowClass, MAX_LOADSTRING);
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
    wcex1.lpszMenuName = MAKEINTRESOURCEW(IDC_LeftTextWindow);
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
    textbox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("D:\\XP\\XP\\Logs\\HexTest — копия.txt"),
       WS_CHILD | WS_VISIBLE, 30, 20, 1000,
       30, hWnd, NULL, NULL, NULL);
    ReadButton = CreateWindowA("button", "o", WS_CHILD |
        WS_VISIBLE | WS_BORDER, 1100, 20, 30, 30, hWnd, (HMENU)1001, hInstance, nullptr);
   RECT rect;
    GetClientRect(hWnd, &rect);
   LeftTextWindow = CreateWindowW(ltWindowClass, ltTitle,WS_CHILD | WS_VSCROLL| WS_BORDER | WS_CLIPSIBLINGS, 
      0, 80, rect.right/2, rect.bottom -80, hWnd, nullptr, hInstance, nullptr);
  /*LeftTextWindow =  CreateWindowA("EDIT", NULL, WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | ES_UPPERCASE, 0, 80, 1000, 800, hWnd, (HMENU)1004, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);*/
  /*  LeftTextWindow = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("//"),
       WS_CHILD | WS_VISIBLE, 0, 80, 1000,
       800, hWnd, NULL, NULL, NULL);
    */
   if (!hWnd)
   {
      return FALSE;
   }

   if (!LeftTextWindow)
   {
       return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   ShowWindow(LeftTextWindow, nCmdShow);
   UpdateWindow(LeftTextWindow);
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
    case WM_SIZE:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        MoveWindow(LeftTextWindow, 0, 80, rc.right / 2, rc.bottom - 80, TRUE);
        break;
    }
    case WM_COMMAND:
        {
            //size_t result;
            
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
           // LPCSTR fn; 
           // DWORD LFSIZE; 
            //File_offset_inf LFOFFSET;
            //int height = 0;
            switch (wmId)
            {
            case 1000: {

                wchar_t* fn;
                   fn = buttonGetFile(File);
                Edit_SetText(textbox, (LPWSTR)fn);
                error = GetLastError();
                break;
            }
            case 1001: {
                InvalidateRect(LeftTextWindow, NULL, TRUE);
                UpdateWindow(LeftTextWindow);
                wchar_t fn1[1000];
                Edit_GetText(textbox, fn1, 1000);
                SYSTEM_INFO SYSINF;
                GetSystemInfo(&SYSINF);
                DWORD LFGranularity = SYSINF.dwAllocationGranularity;
               
                hdcLF = GetDC(LeftTextWindow);
                //MessageBox(NULL, fn1, L"File Name in TextBox:", MB_OK);
                LeftFile_a = CreateFile(fn1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (LeftFile_a == NULL) { MessageBox(NULL, (wchar_t*)GetLastError() , L"Произошла ошибка при открытии файла:", MB_OK); break; }
                _int64 LFSIZE = GetFileSize(LeftFile_a, NULL);
 /*               DWORD FileMapStart = (LFSIZE / LFGranularity)*LFGranularity;
                DWORD LFViewSize = (LFSIZE%LFGranularity)+8;
                DWORD LFMapSize = LFSIZE + 8;
                DWORD LFViewDataS = LFSIZE - FileMapStart;*/
                LeftFile = CreateFileMapping(LeftFile_a, NULL, PAGE_READONLY, 0, 0, NULL);
                if (LeftFile == NULL) { MessageBox(NULL, (wchar_t*)GetLastError(), L"Произошла ошибка при открытии файла:", MB_OK); break; }
                //FONT = CreateFont(0, 10, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, L"Courier");
                //for (DWORD i = 0; i < LFSIZE; i += LFGranularity) {
              /*  DWORD OFFSET = 0;
                while(LFSIZE > 0)*/
                   GetEightBitsHex(LeftFile, LFGranularity, LFSIZE);
                   // TextOutA(hdcLF, 20, 20 + height, (LPCSTR)ss.c_str(), strlen(ss.c_str()));
                   // height += 20;

                //}
                ReleaseDC(LeftTextWindow, hdcLF);
                CloseHandle(LeftFile); 
                CloseHandle(LeftFile_a);
               
                break;
            }
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
