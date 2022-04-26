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
HWND LeftTextWindow;                            // Левое дочернее окно
WCHAR ltWindowClass[MAX_LOADSTRING];            // Имя класса левого дочернего окна вывода
WCHAR ltTitle[MAX_LOADSTRING];                  // Текст строки заголовка левого дочернего окна

HWND TopToolBar;
static HWND search;
static HWND textbox;
static HWND ReadButton;

 
 HANDLE LeftFile;
 //wchar_t szFile[1000];
 //HFONT FONT;
 DWORDLONG ScrollButtonPos = 0;
 ScrollFileInfo ScrolledFilesInfo;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
ATOM            LeftTextWindowRegisterClass(HINSTANCE hInstance);
ATOM            ToolBarRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    LeftProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ToolBarProc(HWND, UINT, WPARAM, LPARAM);
//SIZE FindMaxTextOffset(HFONT FONT, HDC hdc);
//
// Функция buttonGetFile(OPENFILENAME F) 
//  Позволяет произвести поиск нужного файла через провдник
//  return Возвращает рассположение файла на компьютере
OPENFILENAMEW buttonGetFile(){
    OPENFILENAMEW F;
    wchar_t szFile[1000];
    ZeroMemory(&F, sizeof(F));
    F.lStructSize = sizeof(OPENFILENAME);
    F.hwndOwner = NULL;
    F.lpstrFile = (LPWSTR)szFile;
    F.lpstrFile[0] = '\0';
    F.nMaxFile = sizeof(szFile);
    F.lpstrFilter = L"All Files\0*.*\0\0"; 
    F.nFilterIndex = 1;
    F.lpstrFileTitle = NULL;
    F.nMaxFileTitle = 0;
    F.lpstrInitialDir = NULL;
    F.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
   
    GetOpenFileName(&F);


    return F;
}

BOOL GetEightBitsHex(HANDLE LeftFile, DWORD Granularity, DWORD FileSize, DWORD OFFSET) {
    RECT rt;
   
    SIZE STRSIZE;
    GetClientRect(LeftTextWindow, &rt);
    DWORD Block = Granularity, height = 1;
    char BufferString[100] = "";
    DWORD i = OFFSET;
    int SecondOffset = 0;
    OFFSET = (OFFSET/Granularity);

    OFFSET *= Granularity;
   HDC hdcLF = GetDC(LeftTextWindow);
HFONT FONT = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
 /*  SendMessage(textbox, WM_SETFONT, (WPARAM)FONT,FALSE);

   HFONT hFont = (HFONT)SendMessage(textbox, WM_GETFONT, 0, 0);*/
   
   //SelectObject(hdcLF, FONT);

      int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();
       TextParam TextMetric = ScrolledFilesInfo.ReturnTextMetric();
       if (Block > FileSize- OFFSET) Block = FileSize-OFFSET;
        PBYTE LRFILE = (PBYTE)MapViewOfFile(LeftFile, FILE_MAP_READ, 0, OFFSET, Block);
        if (Block / 8 < Strings_On_Screen) Block = ceil(Block / 8.0); else Block = Strings_On_Screen;

          int StrNum = snprintf(BufferString, sizeof(BufferString), "%X", i + Block*8);
          int StrNumOffset = StrNum;
          StrNumOffset *= TextMetric.max_char_width;
          i -= OFFSET;
          for(int count = 0 ; count < Block; count++)
             {
              
           int BufferOffset = snprintf(BufferString, sizeof(BufferString), "%0*X ",StrNum, i + OFFSET);
                BufferOffset += snprintf(BufferString + BufferOffset, sizeof(BufferString) - BufferOffset , ": %02X %02X %02X %02X %02X %02X %02X %02X", LRFILE[i], 
                    LRFILE[i + 1], LRFILE[i + 2], LRFILE[i + 3], LRFILE[i + 4], LRFILE[i + 5], LRFILE[i + 6], LRFILE[i + 7]);

           TextOutA(hdcLF, 5, height, (LPCSTR)BufferString, strlen(BufferString));
           snprintf(BufferString , sizeof(BufferString) , " | %C %C %C %C %C %C %C %C", LRFILE[i], LRFILE[i + 1], LRFILE[i + 2], LRFILE[i + 3], 
               LRFILE[i + 4], LRFILE[i + 5], LRFILE[i + 6], LRFILE[i + 7]);
           TextOutA(hdcLF, 5 + TextMetric.SIZE_MAIN_STRING.cx + StrNumOffset, height, (LPCSTR)BufferString, strlen(BufferString));
           i+=8;
           if (i == Granularity) {
               UnmapViewOfFile(LRFILE);
               int temp = Block;
               Block = Granularity;
               OFFSET = (((OFFSET / Granularity) + 1) * Granularity);
               if (Block > FileSize - OFFSET) Block = FileSize - OFFSET;
               PBYTE LRFILE = (PBYTE)MapViewOfFile(LeftFile, FILE_MAP_READ, 0, OFFSET, Block); 
               i = 0;
               Block = temp;
           }
           height += TextMetric.SIZE_MAIN_STRING.cy;
            }
          ReleaseDC(LeftTextWindow, hdcLF);
        UnmapViewOfFile(LRFILE);
             
    return true;
}

//SIZE FindMaxTextOffset(HFONT FONT , HDC hdc) {
//    if (FONT == NULL) 
//        FONT = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
//    
//    SelectObject(hdc, FONT);
//    error = GetLastError();
//    char buf[60];
//    int max = 0, maxSymbol;
//    SIZE STRSIZE;
//    LPSIZE Size = &STRSIZE;
//    for (int Symbol = 0x41; Symbol < 0x47; Symbol++) {
//        snprintf(buf, sizeof(buf), "%C", Symbol);
//        GetTextExtentPoint32A(hdc, (LPCSTR)buf, strlen(buf), Size);
//        if (Size->cx > max) { max = Size->cx; maxSymbol = Symbol; }
//    }
//    snprintf(buf, sizeof(buf),": %C%C %C%C %C%C %C%C %C%C %C%C %C%C %C%C ", maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol,
//        maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol, maxSymbol);
//    GetTextExtentPoint32A(hdc, (LPCSTR)buf, strlen(buf), Size);
//    return STRSIZE;
//}

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
    LeftTextWindowRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
    
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
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW-1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM  LeftTextWindowRegisterClass(HINSTANCE hInstance)
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
    wcex1.lpszMenuName = NULL;//MAKEINTRESOURCEW(IDC_LeftTextWindow);
    wcex1.lpszClassName = ltWindowClass;
    wcex1.hIconSm = LoadIcon(wcex1.hInstance, MAKEINTRESOURCE(IDI_SMALL));
 
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
       WS_VISIBLE | WS_BORDER, 1050, 20, 30, 30, hWnd, (HMENU)IDB_SearchButton_Left, hInstance, nullptr);
    textbox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("D:\\XP\\TurboXP\\TurboXP.vdi"),
       WS_CHILD | WS_VISIBLE, 30, 20, 1000,
       30, hWnd, NULL, NULL, NULL);
    ReadButton = CreateWindowA("button", "o", WS_CHILD |
        WS_VISIBLE | WS_BORDER, 1100, 20, 30, 30, hWnd, (HMENU)IDB_ReadButton, hInstance, nullptr);
  

   int scroll = GetSystemMetrics(SM_CXVSCROLL); // 17
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
    case WM_CREATE: {
        RECT rect;
        GetClientRect(hWnd, &rect);
        LeftTextWindow = CreateWindowW(ltWindowClass, ltTitle, WS_CHILD | WS_VSCROLL | WS_BORDER | WS_CLIPSIBLINGS,
            0, 80, rect.right / 2, rect.bottom - 80, hWnd, nullptr, hInst, nullptr);
        HFONT FONT = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);

        ScrolledFilesInfo.GetTextMetric(LeftTextWindow, FONT);
        SetScrollRange(LeftTextWindow, SB_VERT, 0, 1000, FALSE);
        SetScrollPos(LeftTextWindow, SB_VERT, 0, TRUE);
    }
    case WM_SIZE:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);
        MoveWindow(LeftTextWindow, 0, 80, rc.right / 2, rc.bottom - 80, TRUE);
        break;
    }
    case WM_COMMAND:
        {
            
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDB_SearchButton_Left: {
                OPENFILENAMEW File;
                   File = buttonGetFile();
                   wchar_t szFile[1000];
                   wcscpy_s(szFile, File.lpstrFile);
                Edit_SetText(textbox, szFile);
                break;
            }
            case IDB_ReadButton: {
               /* CHOOSEFONT cf;
                
                memset(&cf, 0, sizeof(CHOOSEFONT));
                cf.lStructSize = sizeof(CHOOSEFONT);
                cf.Flags = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
                cf.hwndOwner =  LeftTextWindow;
                cf.lpLogFont = 0;
                cf.rgbColors = RGB(0, 0, 0);
                ChooseFont(&cf);*/
                InvalidateRect(LeftTextWindow, NULL, TRUE);
                UpdateWindow(LeftTextWindow);
                wchar_t fn1[1000];
                Edit_GetText(textbox, fn1, 1000);
               
                if(LeftFile!= NULL) CloseHandle(LeftFile); 
                HDC hdcLF = GetDC(LeftTextWindow);

                HANDLE LeftFile_a = CreateFile(fn1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (LeftFile_a == (HANDLE)0xffffffff) {
                    MessageBox(NULL, L"https://sun3.tele2-nn.userapi.com/s/v1/ig2/exhJclZCmFUi4sWoYy0VYeJ2giBulR_5z4o0zfF-5E12Ib2lQZp_v3yK3tDxdJ7qj-zJBj-Q4r_DM5vHJ__l5lmV.jpg?size=1457x1600&quality=96&type=album",
                        L"Произошла ошибка при открытии файла:", MB_OK);
                    //ShellExecute(hWnd, L"open", L"https://sun3.tele2-nn.userapi.com/s/v1/ig2/exhJclZCmFUi4sWoYy0VYeJ2giBulR_5z4o0zfF-5E12Ib2lQZp_v3yK3tDxdJ7qj-zJBj-Q4r_DM5vHJ__l5lmV.jpg?size=1457x1600&quality=96&type=album", NULL, NULL, SW_SHOWDEFAULT); 
                    break;
                }
                ScrolledFilesInfo.GetLeftFileSize(LeftFile_a);
                LARGE_INTEGER LeftFileSize = ScrolledFilesInfo.ReturnLeftFileSize();

                LeftFile = CreateFileMapping(LeftFile_a, NULL, PAGE_READONLY, 0, 0, NULL);
                if (LeftFile == NULL) { MessageBox(NULL, L"", L"Произошла ошибка при открытии файла:", MB_OK);  break; }
                
                GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.LowPart, 0);

                ReleaseDC(LeftTextWindow, hdcLF);
                
                CloseHandle(LeftFile_a);
               
                break;
            }
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
    case WM_SIZE: {
            ScrolledFilesInfo.GetNumStringsOnScreen(LeftTextWindow);
        break;
    }
    case WM_VSCROLL: {
        LARGE_INTEGER LeftFileSize = ScrolledFilesInfo.ReturnLeftFileSize();
        int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();
        switch (LOWORD(wParam)) {
            
        case SB_LINEUP:
            ScrolledFilesInfo.Scrolloffset -= 1;
            break;
        case SB_LINEDOWN:
           ScrolledFilesInfo.Scrolloffset+= 1;
            break;
        case SB_PAGEDOWN:
            ScrolledFilesInfo.Scrolloffset += Strings_On_Screen;
            break;
        case SB_PAGEUP:
            ScrolledFilesInfo.Scrolloffset -= Strings_On_Screen;
            break;
       /* case SB_THUMBPOSITION:
            ScrollButtonPos = HIWORD(wParam);
            ScrolledFilesInfo.Scrolloffset = ceil(ceil(LeftFileSize.LowPart / 8.0) - Strings_On_Screen) * ((float)ScrollButtonPos / 1000);
            break;*/
        case SB_THUMBTRACK:
            ScrollButtonPos = HIWORD(wParam);
            ScrolledFilesInfo.Scrolloffset = ceil(ceil(LeftFileSize.LowPart / 8.0) - Strings_On_Screen) * ((float)ScrollButtonPos / 1000);
            break;
        default:
            return 0 ;
        }

        if (/*ScrollButtonPos != GetScrollPos(LeftTextWindow, SB_VERT) &&*/ LeftFile!= NULL && LeftFileSize.LowPart >= Strings_On_Screen*8 )
        {
            
          ScrollButtonPos = ceil(((ScrolledFilesInfo.Scrolloffset* 1000)/(ceil(LeftFileSize.LowPart / 8.0) - Strings_On_Screen)));
            ScrollButtonPos = min(1000, ScrollButtonPos);
           ScrolledFilesInfo.Scrolloffset = min(ceil(LeftFileSize.LowPart / 8.0) - Strings_On_Screen, ScrolledFilesInfo.Scrolloffset);
           ScrolledFilesInfo.Scrolloffset = max(0, ScrolledFilesInfo.Scrolloffset);
            SetScrollPos(LeftTextWindow, SB_VERT, ScrollButtonPos, TRUE);
            InvalidateRect(LeftTextWindow, NULL, TRUE);
            UpdateWindow(LeftTextWindow);
            //ScrollButtonPos = (DWORD)ScrolledFilesInfo.Scrolloffset;
            GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.LowPart, ScrolledFilesInfo.Scrolloffset * 8);
            //ScrollButtonPos = temp;
            
        }
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
      /*  case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;*/
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
