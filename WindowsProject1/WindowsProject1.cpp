// WindowsProject1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100
#define MAX_BYTESONSTRING 8

DWORD error;
// Глобальные переменные:

 HANDLE LeftFile;
 ScrollFileInfo ScrolledFilesInfo;
 MainWindows WindowInfo;

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

BOOL GetEightBitsHex(HANDLE LeftFile, DWORD Granularity, DWORD FileSize, DWORD OFFSET, LONG HorizontalOffset) {
    DWORD Block = Granularity, height = 1;
    char BufferString[100*MAX_BYTESONSTRING] = "";
    DWORD i = OFFSET;
    int SecondOffset = 0;
    OFFSET = (OFFSET/Granularity);
    int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();
    TEXTMETRIC TextMetric = ScrolledFilesInfo.ReturnTextMetric();

   HDC hdcLF = GetDC(WindowInfo.LeftTextWindow);
   HFONT FONT = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);

    OFFSET *= Granularity;
   SelectObject(hdcLF, FONT);
       if (Block > FileSize- OFFSET) Block = FileSize-OFFSET;
        PBYTE LRFILE = (PBYTE)MapViewOfFile(LeftFile, FILE_MAP_READ, 0, OFFSET, Block);

        if (Block / MAX_BYTESONSTRING < Strings_On_Screen) Block = ceil(Block / (double)MAX_BYTESONSTRING); else Block = Strings_On_Screen;

          int StrNum = snprintf(BufferString, sizeof(BufferString), "%X", FileSize);
         
         
          int HexOffset = 0;
          i -= OFFSET;
          for(int count = 0 ; count < Block; count++)
             {
              int BufferOffset = snprintf(BufferString, sizeof(BufferString), "%0*X :", StrNum, i + OFFSET);
              if (HorizontalOffset != 0) {
                  if (HorizontalOffset > BufferOffset) {
                      HexOffset = HorizontalOffset - BufferOffset;
                      BufferOffset = 0;
                  }
                  else {
                      for (int i = 0; i < BufferOffset - HorizontalOffset; i++)
                          BufferString[i] = BufferString[i + HorizontalOffset];
                      BufferOffset = BufferOffset - HorizontalOffset;
                  }
              }
              int StrNumOffset = BufferOffset;

           for (int j = HexOffset; j < MAX_BYTESONSTRING; j++) {
               BufferOffset += snprintf(BufferString + BufferOffset, sizeof(BufferString) - BufferOffset, " %02X", LRFILE[i + j]);
              }
         BufferOffset += snprintf(BufferString+BufferOffset, sizeof(BufferString)-BufferOffset, " |");

           if (HorizontalOffset > MAX_BYTESONSTRING + StrNum+2) {
               HexOffset = HorizontalOffset - (MAX_BYTESONSTRING + StrNum + 2);
               BufferOffset = 0;
           }
           else {
               HexOffset = 0;
               BufferOffset *= TextMetric.tmAveCharWidth;
               TextOutA(hdcLF, 5, height, (LPCSTR)BufferString, strlen(BufferString));
           }
           
          
          /* snprintf(BufferString , sizeof(BufferString) , PrintStringChar, LRFILE[i], LRFILE[i + 1], LRFILE[i + 2], LRFILE[i + 3], 
               LRFILE[i + 4], LRFILE[i + 5], LRFILE[i + 6], LRFILE[i + 7]);*/
           int CharOffset = 0;
                  for (int j = HexOffset; j < MAX_BYTESONSTRING; j++) {
                      if (LRFILE[i + j] == '\0') {
                          CharOffset += snprintf(BufferString + CharOffset, sizeof(BufferString) - CharOffset, "  ");
                          continue;
                      }
             CharOffset +=snprintf(BufferString+CharOffset , sizeof(BufferString)-CharOffset, " %C", LRFILE[i + j]);
           }
           TextOutA(hdcLF, 5 + BufferOffset + /*StrNumOffset*/ + 5, height, (LPCSTR)BufferString, strlen(BufferString));

           i+=MAX_BYTESONSTRING;

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
           height += TextMetric.tmHeight;
            }
          ReleaseDC(WindowInfo.LeftTextWindow, hdcLF);
        UnmapViewOfFile(LRFILE);
             
    return true;
}


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
            , WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_BORDER /*| WS_CLIPSIBLINGS*/,
            0, ceil(rect.bottom*0.1), rect.right / 2, ceil(rect.bottom*0.9), hWnd, nullptr, WindowInfo.hInst, nullptr);

        WindowInfo.ToolBar = CreateWindowW((LPCWSTR)WindowInfo.GetToolBarClass(), NULL, WS_CHILD | WS_BORDER, 0, 0,
            rect.right, ceil(rect.bottom*0.1), hWnd, nullptr, WindowInfo.hInst, nullptr);

        WindowInfo.RightTextWindow = CreateWindowW((LPCWSTR)WindowInfo.GetRightWindowClass(), NULL
            , WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_BORDER /*| WS_CLIPSIBLINGS*/,
            rect.right/2, ceil(rect.bottom * 0.1), rect.right / 2, ceil(rect.bottom * 0.9), hWnd, nullptr, WindowInfo.hInst, nullptr);
      error =   GetLastError();
        HFONT FONT = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
        ScrolledFilesInfo.GetTextMetric(WindowInfo.LeftTextWindow, FONT);
        ShowScrollBar(WindowInfo.LeftTextWindow, SB_HORZ, TRUE);
        SetScrollRange(WindowInfo.LeftTextWindow, SB_VERT, 0, 1000, FALSE);

        //SetScrollPos(LeftTextWindow, SB_VERT, 0, TRUE);
    }
    case WM_SIZE:
    {
        RECT rc;
        GetClientRect(hWnd, &rc);

        int Screen_height = GetSystemMetrics(SM_CYFULLSCREEN);
        int Menu_height = GetSystemMetrics(SM_CYMENUSIZE);
        //int rr = GetSystemMetrics(SM_CYCAPTION);
        //SystemParametersInfo(SPI_GETWORKAREA, NULL , &rc, NULL);
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

//
//  ФУНКЦИЯ: LeftProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в левом окне вывода.
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
            ScrolledFilesInfo.GetNumStringsAndCharOnScreen(WindowInfo.LeftTextWindow);

        break;
    }
    case WM_VSCROLL: {
        DWORDLONG ScrollButtonPos = 0;
        LARGE_INTEGER LeftFileSize = ScrolledFilesInfo.ReturnLeftFileSize();
        int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();
       
        switch (LOWORD(wParam)) {
            
        case SB_LINEUP:
            ScrolledFilesInfo.m_ScrollVerticalOffset -= 1;
            break;
        case SB_LINEDOWN:
           ScrolledFilesInfo.m_ScrollVerticalOffset+= 1;
            break;
        case SB_PAGEDOWN:
            ScrolledFilesInfo.m_ScrollVerticalOffset += Strings_On_Screen;
            break;
        case SB_PAGEUP:
            ScrolledFilesInfo.m_ScrollVerticalOffset -= Strings_On_Screen;
            break;
       /* case SB_THUMBPOSITION:
            ScrollButtonPos = HIWORD(wParam);
            ScrolledFilesInfo.ScrollVerticalOffset = ceil(ceil(LeftFileSize.LowPart / 8.0) - Strings_On_Screen) * ((float)ScrollButtonPos / 1000);
            break;*/
        case SB_THUMBTRACK:
            ScrollButtonPos = HIWORD(wParam);
            ScrolledFilesInfo.m_ScrollVerticalOffset = ceil(ceil(LeftFileSize.LowPart / (double)MAX_BYTESONSTRING) - Strings_On_Screen) * ((float)ScrollButtonPos / 1000);
            break;
        default:
            return 0 ;
        }

        if (LeftFile!= NULL && LeftFileSize.LowPart >= Strings_On_Screen*8 )
        {
            
          ScrollButtonPos = ceil(((ScrolledFilesInfo.m_ScrollVerticalOffset* 1000)/(ceil(LeftFileSize.LowPart / (double)MAX_BYTESONSTRING) - Strings_On_Screen)));
            ScrollButtonPos = min(1000, ScrollButtonPos);
           ScrolledFilesInfo.m_ScrollVerticalOffset = min(ceil(LeftFileSize.LowPart / (double)MAX_BYTESONSTRING) - Strings_On_Screen, ScrolledFilesInfo.m_ScrollVerticalOffset);
           ScrolledFilesInfo.m_ScrollVerticalOffset = max(0, ScrolledFilesInfo.m_ScrollVerticalOffset);
            SetScrollPos(WindowInfo.LeftTextWindow, SB_VERT, ScrollButtonPos, TRUE);
            InvalidateRect(WindowInfo.LeftTextWindow, NULL, TRUE);
            UpdateWindow(WindowInfo.LeftTextWindow);
          
            GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.LowPart,
                ScrolledFilesInfo.m_ScrollVerticalOffset * MAX_BYTESONSTRING, ScrolledFilesInfo.m_ScrollHorizontalOffset);
            
        }
        break;
    }
    case WM_HSCROLL: {
        DWORDLONG ScrollHorizontalButtonPos = 0;
        LARGE_INTEGER LeftFileSize = ScrolledFilesInfo.ReturnLeftFileSize();
        LONG CharOnString = ScrolledFilesInfo.ReturnCharsOnScreen();
        switch (LOWORD(wParam)) {

        case SB_LINELEFT:
            ScrolledFilesInfo.m_ScrollHorizontalOffset -= 1;
            break;
        case SB_LINERIGHT:
            ScrolledFilesInfo.m_ScrollHorizontalOffset += 1;
            break;
        case SB_THUMBTRACK:
            ScrollHorizontalButtonPos = HIWORD(wParam);
            ScrolledFilesInfo.m_ScrollHorizontalOffset = ScrollHorizontalButtonPos;
            break;
        default:
            return 0;
        }

        if (LeftFile != NULL /*&& LeftFileSize.LowPart >= Strings_On_Screen * 8*/)
        {
            ScrolledFilesInfo.m_ScrollHorizontalOffset = min(50, ScrolledFilesInfo.m_ScrollHorizontalOffset);
            ScrolledFilesInfo.m_ScrollHorizontalOffset = max(0, ScrolledFilesInfo.m_ScrollHorizontalOffset);
            ScrollHorizontalButtonPos = ScrolledFilesInfo.m_ScrollHorizontalOffset;
            ScrollHorizontalButtonPos = min(50, ScrollHorizontalButtonPos);

            SetScrollPos(WindowInfo.LeftTextWindow, SB_HORZ, ScrollHorizontalButtonPos, TRUE);
            InvalidateRect(WindowInfo.LeftTextWindow, NULL, TRUE);
            UpdateWindow(WindowInfo.LeftTextWindow);

            GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.LowPart,
                ScrolledFilesInfo.m_ScrollVerticalOffset * MAX_BYTESONSTRING, ScrolledFilesInfo.m_ScrollHorizontalOffset);

        }
        break;

    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {

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

LRESULT CALLBACK ToolProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    switch (message)
    {
    case WM_CREATE: {
        RECT rect;
        GetClientRect(hWnd, &rect);
        int sc = GetSystemMetrics(SM_CXVSCROLL);
       
        WindowInfo.LeftTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("D:\\XP\\TurboXP\\TurboXP.vdi"),
            WS_CHILD | WS_VISIBLE, 1, 1, ceil(rect.right/2-rect.right*0.01-sc)-5, 20, hWnd, NULL, NULL, NULL);
        WindowInfo.LeftSearch = CreateWindowA("button", ">>", WS_CHILD |
            WS_VISIBLE | WS_BORDER, ceil(rect.right / 2 - rect.right * 0.01 - sc)-4, 1, ceil(rect.right * 0.01 + sc),
            20, hWnd, (HMENU)IDB_SearchButton_Left, WindowInfo.hInst, nullptr);
        WindowInfo.ReadButton = CreateWindowA("button", "Сравнить", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 750, 20, 30, 30, hWnd, (HMENU)IDB_ReadButton, WindowInfo.hInst, nullptr);
        WindowInfo.RightTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("WORK IN PROGRESS"),
            WS_CHILD | WS_VISIBLE, rect.right/2, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc), 20, hWnd, NULL, NULL, NULL);
        WindowInfo.RightSearch = CreateWindowA("button", ">>", WS_CHILD |
            WS_VISIBLE | WS_BORDER, ceil(rect.right - rect.right * 0.01 - sc) + 1, 1, ceil(rect.right * 0.01 + sc), 20,
            hWnd, (HMENU)IDB_SearchButton_Right, WindowInfo.hInst, nullptr);
        WindowInfo.ChangeFont = CreateWindowA("button", "WORK IN PRIGRESS", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 0, 0, 0, 0,
            hWnd, (HMENU)IDB_Change_Font, WindowInfo.hInst, nullptr);
        /*WindowInfo.List = CreateWindowA("button", "Сравнить", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 750, 20, 30, 30, hWnd, (HMENU)IDB_ReadButton, WindowInfo.hInst, nullptr);*/
    }

    case WM_SIZE: {
        RECT rect;
        GetClientRect(hWnd, &rect);
     
        int sc = GetSystemMetrics(SM_CXVSCROLL);
        MoveWindow(WindowInfo.LeftTextbox, 1, 1 , ceil(rect.right / 2 - rect.right * 0.01 - sc)-5, ceil(rect.bottom *0.33), TRUE);
        MoveWindow(WindowInfo.LeftSearch,  ceil(rect.right / 2 - rect.right * 0.01 - sc)-4, 1,ceil(rect.right*0.01+sc), ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(WindowInfo.RightTextbox, rect.right / 2, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc), ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(WindowInfo.RightSearch, ceil(rect.right - rect.right * 0.01 - sc) + 1, 1, ceil(rect.right * 0.01 + sc), ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(WindowInfo.ReadButton, 1, ceil(rect.bottom * 0.33) + 1, ceil(rect.right*0.2),ceil(rect.bottom * 0.33) ,TRUE);
        MoveWindow(WindowInfo.ChangeFont, 1, (ceil(rect.bottom * 0.33) + 1)*2, ceil(rect.right * 0.2), ceil(rect.bottom * 0.3), TRUE);
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
            Edit_SetText(WindowInfo.LeftTextbox, szFile);
            break;
        }
        case IDB_ReadButton: {

            InvalidateRect(WindowInfo.LeftTextWindow, NULL, TRUE);
            UpdateWindow(WindowInfo.LeftTextWindow);
            wchar_t fn1[1000];
            Edit_GetText(WindowInfo.LeftTextbox, fn1, 1000);

            if (LeftFile != NULL) CloseHandle(LeftFile);
            HDC hdcLF = GetDC(WindowInfo.LeftTextWindow);

            HANDLE LeftFile_a = CreateFile(fn1, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (LeftFile_a == (HANDLE)0xffffffffffffffff) {
                MessageBox(NULL, L"https://sun3.tele2-nn.userapi.com/s/v1/ig2/exhJclZCmFUi4sWoYy0VYeJ2giBulR_5z4o0zfF-5E12Ib2lQZp_v3yK3tDxdJ7qj-zJBj-Q4r_DM5vHJ__l5lmV.jpg?size=1457x1600&quality=96&type=album",
                    L"Произошла ошибка при открытии файла:", MB_OK);
                //ShellExecute(hWnd, L"open", L"https://sun3.tele2-nn.userapi.com/s/v1/ig2/exhJclZCmFUi4sWoYy0VYeJ2giBulR_5z4o0zfF-5E12Ib2lQZp_v3yK3tDxdJ7qj-zJBj-Q4r_DM5vHJ__l5lmV.jpg?size=1457x1600&quality=96&type=album", NULL, NULL, SW_SHOWDEFAULT); 
                break;
            }

            ScrolledFilesInfo.GetLeftFileSize(LeftFile_a);
            LARGE_INTEGER LeftFileSize = ScrolledFilesInfo.ReturnLeftFileSize();

            LeftFile = CreateFileMapping(LeftFile_a, NULL, PAGE_READONLY, 0, 0, NULL);
            if (LeftFile == NULL) { MessageBox(NULL, L"", L"Произошла ошибка при открытии файла:", MB_OK);  break; }

            GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.LowPart, 0, 0);

            ReleaseDC(WindowInfo.LeftTextWindow, hdcLF);

            CloseHandle(LeftFile_a);

            break;
        }
        case IDB_ChangeBytesNumButton: {
            int i = MAX_BYTESONSTRING;



            int di = MAX_BYTESONSTRING;

            break;
        }
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


LRESULT CALLBACK RightProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    switch (message)
    {
    case WM_SIZE: {
       // ScrolledFilesInfo.GetNumStringsAndCharOnScreen(WindowInfo.LeftTextWindow);
        break;
    }
    case WM_VSCROLL: {
    //    DWORDLONG ScrollButtonPos = 0;
    //    LARGE_INTEGER LeftFileSize = ScrolledFilesInfo.ReturnLeftFileSize();
    //    int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();

    //    switch (LOWORD(wParam)) {

    //    case SB_LINEUP:
    //        ScrolledFilesInfo.m_ScrollVerticalOffset -= 1;
    //        break;
    //    case SB_LINEDOWN:
    //        ScrolledFilesInfo.m_ScrollVerticalOffset += 1;
    //        break;
    //    case SB_PAGEDOWN:
    //        ScrolledFilesInfo.m_ScrollVerticalOffset += Strings_On_Screen;
    //        break;
    //    case SB_PAGEUP:
    //        ScrolledFilesInfo.m_ScrollVerticalOffset -= Strings_On_Screen;
    //        break;
    //        /* case SB_THUMBPOSITION:
    //             ScrollButtonPos = HIWORD(wParam);
    //             ScrolledFilesInfo.ScrollVerticalOffset = ceil(ceil(LeftFileSize.LowPart / 8.0) - Strings_On_Screen) * ((float)ScrollButtonPos / 1000);
    //             break;*/
    //    case SB_THUMBTRACK:
    //        ScrollButtonPos = HIWORD(wParam);
    //        ScrolledFilesInfo.m_ScrollVerticalOffset = ceil(ceil(LeftFileSize.LowPart / (double)MAX_BYTESONSTRING) - Strings_On_Screen) * ((float)ScrollButtonPos / 1000);
    //        break;
    //    default:
    //        return 0;
    //    }

    //    if (LeftFile != NULL && LeftFileSize.LowPart >= Strings_On_Screen * 8)
    //    {

    //        ScrollButtonPos = ceil(((ScrolledFilesInfo.m_ScrollVerticalOffset * 1000) / (ceil(LeftFileSize.LowPart / (double)MAX_BYTESONSTRING) - Strings_On_Screen)));
    //        ScrollButtonPos = min(1000, ScrollButtonPos);
    //        ScrolledFilesInfo.m_ScrollVerticalOffset = min(ceil(LeftFileSize.LowPart / (double)MAX_BYTESONSTRING) - Strings_On_Screen, ScrolledFilesInfo.m_ScrollVerticalOffset);
    //        ScrolledFilesInfo.m_ScrollVerticalOffset = max(0, ScrolledFilesInfo.m_ScrollVerticalOffset);
    //        SetScrollPos(WindowInfo.LeftTextWindow, SB_VERT, ScrollButtonPos, TRUE);
    //        InvalidateRect(WindowInfo.LeftTextWindow, NULL, TRUE);
    //        UpdateWindow(WindowInfo.LeftTextWindow);

    //        GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.LowPart,
    //            ScrolledFilesInfo.m_ScrollVerticalOffset * MAX_BYTESONSTRING, ScrolledFilesInfo.m_ScrollHorizontalOffset);

    //    }
    //    break;
    //}
    //case WM_HSCROLL: {
    //    DWORDLONG ScrollHorizontalButtonPos = 0;
    //    LARGE_INTEGER LeftFileSize = ScrolledFilesInfo.ReturnLeftFileSize();
    //    LONG CharOnString = ScrolledFilesInfo.ReturnCharsOnScreen();
    //    switch (LOWORD(wParam)) {

    //    case SB_LINELEFT:
    //        ScrolledFilesInfo.m_ScrollHorizontalOffset -= 1;
    //        break;
    //    case SB_LINERIGHT:
    //        ScrolledFilesInfo.m_ScrollHorizontalOffset += 1;
    //        break;
    //    case SB_THUMBTRACK:
    //        ScrollHorizontalButtonPos = HIWORD(wParam);
    //        ScrolledFilesInfo.m_ScrollHorizontalOffset = ScrollHorizontalButtonPos;
    //        break;
    //    default:
    //        return 0;
    //    }

    //    if (LeftFile != NULL /*&& LeftFileSize.LowPart >= Strings_On_Screen * 8*/)
    //    {
    //        ScrolledFilesInfo.m_ScrollHorizontalOffset = min(50, ScrolledFilesInfo.m_ScrollHorizontalOffset);
    //        ScrolledFilesInfo.m_ScrollHorizontalOffset = max(0, ScrolledFilesInfo.m_ScrollHorizontalOffset);
    //        ScrollHorizontalButtonPos = ScrolledFilesInfo.m_ScrollHorizontalOffset;
    //        ScrollHorizontalButtonPos = min(50, ScrollHorizontalButtonPos);

    //        SetScrollPos(WindowInfo.LeftTextWindow, SB_HORZ, ScrollHorizontalButtonPos, TRUE);
    //        InvalidateRect(WindowInfo.LeftTextWindow, NULL, TRUE);
    //        UpdateWindow(WindowInfo.LeftTextWindow);

    //        GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.LowPart,
    //            ScrolledFilesInfo.m_ScrollVerticalOffset * MAX_BYTESONSTRING, ScrolledFilesInfo.m_ScrollHorizontalOffset);

    //    }
    //    break;

    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {

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
