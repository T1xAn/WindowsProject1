// WindowsProject1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100
//s#define MAX_BYTESONSTRING 8

DWORD error;
// Глобальные переменные:

 HANDLE LeftFile;
 HANDLE RightFile;
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

BOOL GetEightBitsHex(HWND Window ,HANDLE File, DWORD Granularity, DWORDLONG FileSize, DWORDLONG OFFSET, LONG HorizontalOffset , LONG BytesOnString) {
    DWORD Block = Granularity, height = 1;
    char BufferString[256] = ""; // РАЗМЕРНОСТЬ
    OFFSET *= BytesOnString;
    //OFFSET = 9525264384 - ScrolledFilesInfo.ReturnStringsOnScreen()*8;
    DWORDLONG i = OFFSET;
    int SecondOffset = 0;
    OFFSET = (OFFSET/Granularity);
    int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();
    TEXTMETRIC TextMetric = ScrolledFilesInfo.ReturnTextMetric();

   HDC hdcLF = GetDC(Window);
   HFONT FONT = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);

   RECT rc;
   GetClientRect(Window, &rc);

   HDC BlitHDC = CreateCompatibleDC(hdcLF);
   HBITMAP BitMap = CreateCompatibleBitmap(hdcLF, rc.right, rc.bottom);
   SelectObject(BlitHDC, BitMap);
   SelectObject(BlitHDC, FONT);
   DeleteObject(BitMap);
   BitBlt(BlitHDC, 0, 0, rc.right, rc.bottom, BlitHDC, 0, 0, WHITENESS);

    OFFSET *= Granularity;
    DWORD HighOffset = ((OFFSET >> 32) & 0xFFFFFFFFul);
    DWORD LowOffset = static_cast<DWORD>(OFFSET & 0xFFFFFFFFul);

   SelectObject(hdcLF, FONT);

       if (Block > FileSize- OFFSET) Block = FileSize-OFFSET;
        PBYTE LRFILE = (PBYTE)MapViewOfFile(File, FILE_MAP_READ, HighOffset, LowOffset, Block);
        error = GetLastError();
        if (Block / BytesOnString < Strings_On_Screen) Block = ceil(Block / (double)BytesOnString); else Block = Strings_On_Screen;

          int StrNum = snprintf(BufferString, sizeof(BufferString), "%llX", FileSize);
         
         
          int HexOffset = 0;
          i -= OFFSET;
          for(int count = 0 ; count < Block; count++)
             {
              int BufferOffset = snprintf(BufferString, sizeof(BufferString), "%0*llX :", StrNum, i + OFFSET);

              if (i + OFFSET >= FileSize) {
                  i += BytesOnString;
                  TextOutA(BlitHDC, 5, height, (LPCSTR)BufferString, strlen(BufferString));
                  height += TextMetric.tmHeight;
                  BufferString[0] = '\0';
                  continue;
              }

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

           /*   if (i+ OFFSET >= FileSize) {
                  i+= BytesOnString;
                  TextOutA(BlitHDC, 5, height, (LPCSTR)BufferString, strlen(BufferString));
                  height += TextMetric.tmHeight;
                  BufferString[0] = '\0';
                  continue;
              }*/

           for (int j = HexOffset; j < BytesOnString; j++) {
               BufferOffset += snprintf(BufferString + BufferOffset, sizeof(BufferString) - BufferOffset, " %02X", LRFILE[i + j]);
              }
         BufferOffset += snprintf(BufferString+BufferOffset, sizeof(BufferString)-BufferOffset, " |");

           if (HorizontalOffset > BytesOnString + StrNum+2) {
               HexOffset = HorizontalOffset - (BytesOnString + StrNum + 2);
               BufferOffset = 0;
           }
           else {
               HexOffset = 0;
               BufferOffset *= TextMetric.tmAveCharWidth;
               TextOutA(BlitHDC, 5, height, (LPCSTR)BufferString, strlen(BufferString));
           }
           
          
          /* snprintf(BufferString , sizeof(BufferString) , PrintStringChar, LRFILE[i], LRFILE[i + 1], LRFILE[i + 2], LRFILE[i + 3], 
               LRFILE[i + 4], LRFILE[i + 5], LRFILE[i + 6], LRFILE[i + 7]);*/
           int CharOffset = 0;
                  for (int j = HexOffset; j < BytesOnString; j++) {
                      if (LRFILE[i + j] == '\0') {
                          CharOffset += snprintf(BufferString + CharOffset, sizeof(BufferString) - CharOffset, "  ");
                          continue;
                      }
             CharOffset +=snprintf(BufferString+CharOffset , sizeof(BufferString)-CharOffset, " %C", LRFILE[i + j]);
           }
           TextOutA(BlitHDC, 5 + BufferOffset + /*StrNumOffset*/ + 5, height, (LPCSTR)BufferString, strlen(BufferString));

           i+= BytesOnString;

           if (i == Granularity) {
               UnmapViewOfFile(LRFILE);
               int temp = Block;
               Block = Granularity;
               OFFSET = (((OFFSET / Granularity) + 1) * Granularity);
               if (Block > FileSize - OFFSET) Block = FileSize - OFFSET;
               PBYTE LRFILE = (PBYTE)MapViewOfFile(File, FILE_MAP_READ, HighOffset, LowOffset, Block);
               i = 0;
               Block = temp;
           }
           height += TextMetric.tmHeight;
           }
         
        UnmapViewOfFile(LRFILE);
        BitBlt(hdcLF, 0, 0, rc.right, rc.bottom, BlitHDC,0, 0 , SRCCOPY);
        DeleteDC(BlitHDC); 
        ReleaseDC(Window, hdcLF);
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
      
      WindowInfo.m_UpdatingWindows[0] = WindowInfo.LeftTextWindow;
      WindowInfo.m_UpdatingWindows[1] = WindowInfo.RightTextWindow;

        HFONT FONT = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
        ScrolledFilesInfo.GetTextMetric(WindowInfo.LeftTextWindow, FONT);

        for (int i = 0; i < 2; i++) {
        ShowScrollBar(WindowInfo.m_UpdatingWindows[i], SB_HORZ, FALSE);
        SetScrollRange(WindowInfo.m_UpdatingWindows[i], SB_VERT, 0, 1000, FALSE);
        }
        

      /*  ShowScrollBar(WindowInfo.RightTextWindow, SB_HORZ, FALSE);
        SetScrollRange(WindowInfo.RightTextWindow, SB_VERT, 0, 1000, FALSE);*/

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
    switch (message)
    {

    case WM_MOUSEWHEEL: {
            if (int Wheel = GET_WHEEL_DELTA_WPARAM(wParam) > 0) 
                ScrolledFilesInfo.m_ScrollVerticalOffset -= 5;
            else 
                ScrolledFilesInfo.m_ScrollVerticalOffset += 5;

            for (int i = 0; i < 2; i++) {
                SendMessage(WindowInfo.m_UpdatingWindows[i], WM_VSCROLL, -1L, 0L);
            }

        break;
    }
    
    case WM_SIZE: {
            ScrolledFilesInfo.GetNumStringsAndCharOnScreen(WindowInfo.LeftTextWindow);
            LONG HorizontalMaxScroll = ScrolledFilesInfo.HorizontalOffset();
            if (HorizontalMaxScroll!= 0)
            {
                SetScrollRange(WindowInfo.LeftTextWindow, SB_HORZ, 0, HorizontalMaxScroll, TRUE);
                ShowScrollBar(WindowInfo.LeftTextWindow, SB_HORZ, TRUE);
                SetScrollPos(WindowInfo.LeftTextWindow, SB_HORZ, 0, TRUE);

            }
            else {
                ShowScrollBar(WindowInfo.LeftTextWindow, SB_HORZ, FALSE);
                SetScrollPos(WindowInfo.LeftTextWindow, SB_HORZ, 0, TRUE);
                ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
            }
            for (int i = 0; i < 2; i++) {
                SendMessage(WindowInfo.LeftTextWindow, WM_PAINT, 0, 0);
            }
        break;
    }
    case WM_VSCROLL: {
        DWORDLONG ScrollButtonPos = 0;
        LONGLONG BiggestFileSize = ScrolledFilesInfo.ReturnBiggestFile();
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
            ScrolledFilesInfo.m_ScrollVerticalOffset = ceil(ceil(BiggestFileSize / (double)ScrolledFilesInfo.m_BytesOnString) - Strings_On_Screen) * ((float)ScrollButtonPos / 1000);
            break;
        default:
           break ;
        }


        if (LeftFile!= NULL)
        {

            if (ScrolledFilesInfo.m_ScrollVerticalOffset >= 18446744073709551614)
            {
                ScrolledFilesInfo.m_ScrollVerticalOffset = 0;
            }

            if(wParam != -1)
                SendNotifyMessage(WindowInfo.RightTextWindow, WM_VSCROLL, -1L, -1L);
          

          ScrollButtonPos = floor(((ScrolledFilesInfo.m_ScrollVerticalOffset* 1000)/(ceil(BiggestFileSize / (double)ScrolledFilesInfo.m_BytesOnString) - Strings_On_Screen)));
            ScrollButtonPos = min(1000, ScrollButtonPos);
            ScrolledFilesInfo.m_ScrollVerticalOffset = max(0, (min(ceil(BiggestFileSize / (double)ScrolledFilesInfo.m_BytesOnString) - Strings_On_Screen,
                ScrolledFilesInfo.m_ScrollVerticalOffset)));
            SetScrollPos(WindowInfo.LeftTextWindow, SB_VERT, ScrollButtonPos, TRUE);
         /*InvalidateRect(WindowInfo.LeftTextWindow,, TRUE);
            UpdateWindow(WindowInfo.LeftTextWindow);*/
            SendMessage(WindowInfo.LeftTextWindow, WM_PAINT, 0, 0);
            /*GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.QuadPart,
                ScrolledFilesInfo.m_ScrollVerticalOffset, ScrolledFilesInfo.m_ScrollHorizontalOffset, ScrolledFilesInfo.m_BytesOnString);*/
            
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
            break;
        }

        if (LeftFile != NULL /*&& LeftFileSize.LowPart >= Strings_On_Screen * 8*/)
        {

            if (wParam != -1)
                SendNotifyMessage(WindowInfo.RightTextWindow, WM_HSCROLL, -1L, -1L);
         
            int MaxRange = 0, MinRange = 0;
            GetScrollRange(WindowInfo.LeftTextWindow, SB_HORZ, &MinRange, &MaxRange);
            ScrolledFilesInfo.m_ScrollHorizontalOffset = min(MaxRange, ScrolledFilesInfo.m_ScrollHorizontalOffset);
            ScrolledFilesInfo.m_ScrollHorizontalOffset = max(MinRange, ScrolledFilesInfo.m_ScrollHorizontalOffset);
            ScrollHorizontalButtonPos = ScrolledFilesInfo.m_ScrollHorizontalOffset;
            ScrollHorizontalButtonPos = min(50, ScrollHorizontalButtonPos);

            SetScrollPos(WindowInfo.LeftTextWindow, SB_HORZ, ScrollHorizontalButtonPos, TRUE);
           /* InvalidateRect(WindowInfo.LeftTextWindow, NULL, TRUE);*/
            //UpdateWindow(WindowInfo.LeftTextWindow);
            
            SendMessage(WindowInfo.LeftTextWindow, WM_PAINT, 0, 0);

            /*GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.QuadPart,
                ScrolledFilesInfo.m_ScrollVerticalOffset , ScrolledFilesInfo.m_ScrollHorizontalOffset, ScrolledFilesInfo.m_BytesOnString);*/

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
        if (LeftFile != NULL) {
            LARGE_INTEGER LeftFileSize = ScrolledFilesInfo.ReturnLeftFileSize();
            GetEightBitsHex(WindowInfo.LeftTextWindow, LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.QuadPart,
                ScrolledFilesInfo.m_ScrollVerticalOffset, ScrolledFilesInfo.m_ScrollHorizontalOffset, ScrolledFilesInfo.m_BytesOnString);
        }
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
       
        WindowInfo.LeftTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("D:\\BFF.rar"),
            WS_CHILD | WS_VISIBLE, 1, 1, ceil(rect.right/2-rect.right*0.01-sc)-5, 20, hWnd, NULL, NULL, NULL);
        WindowInfo.LeftSearch = CreateWindowA("button", ">>", WS_CHILD |
            WS_VISIBLE | WS_BORDER, ceil(rect.right / 2 - rect.right * 0.01 - sc)-4, 1, ceil(rect.right * 0.01 + sc),
            20, hWnd, (HMENU)IDB_SearchButton_Left, WindowInfo.hInst, nullptr);
        WindowInfo.ReadButton = CreateWindowA("button", "Сравнить", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 750, 20, 30, 30, hWnd, (HMENU)IDB_ReadButton, WindowInfo.hInst, nullptr);
        WindowInfo.RightTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("D:\\XP\\TurboServer\\TurboServer.vdi"),
            WS_CHILD | WS_VISIBLE, rect.right/2, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc), 20, hWnd, NULL, NULL, NULL);
        WindowInfo.RightSearch = CreateWindowA("button", ">>", WS_CHILD |
            WS_VISIBLE | WS_BORDER, ceil(rect.right - rect.right * 0.01 - sc) + 1, 1, ceil(rect.right * 0.01 + sc), 20,
            hWnd, (HMENU)IDB_SearchButton_Right, WindowInfo.hInst, nullptr);
        WindowInfo.ChangeFont = CreateWindowA("button", "WORK IN PRIGRESS", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 0, 0, 0, 0,
            hWnd, (HMENU)IDB_Change_Font, WindowInfo.hInst, nullptr);

        WindowInfo.List = CreateWindowW(WC_LISTBOX, NULL, WS_CHILD | LBS_MULTICOLUMN | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, ceil(rect.right*0.2),
            ceil(rect.bottom * 0.33) + 1, ceil(rect.right * 0.2), ceil(rect.bottom * 0.33) + 1 , hWnd, (HMENU)IDB_ListBox,WindowInfo.hInst,nullptr);
      
       SendMessage(WindowInfo.List, LB_ADDSTRING, 0,(LPARAM)L"8");
       SendMessage(WindowInfo.List, LB_ADDSTRING, 0, (LPARAM)L"16");
       SendMessage(WindowInfo.List, LB_ADDSTRING, 0, (LPARAM)L"32");
       for (int count = 0; count < 3; count++) {
           int data = pow(2, count+3);
        SendMessage(WindowInfo.List, LB_SETITEMDATA, count, (LPARAM)data);

       }
      //SendMessage(WindowInfo.List, LB_SETCURSEL, 0, 0L)
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
        MoveWindow(WindowInfo.List, ceil(rect.right * 0.2),ceil(rect.bottom * 0.33) + 1, ceil(rect.right * 0.2), ceil(rect.bottom * 0.66) + 1, TRUE);
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
        case IDB_SearchButton_Right: {
            OPENFILENAMEW File;
            File = buttonGetFile();
            wchar_t szFile[1000];
            wcscpy_s(szFile, File.lpstrFile);
            Edit_SetText(WindowInfo.RightTextbox, szFile);
            break;
        }
        case IDB_ReadButton: {

         /*   InvalidateRect(WindowInfo.LeftTextWindow, NULL, TRUE);
            UpdateWindow(WindowInfo.LeftTextWindow);*/
            wchar_t FirstFile[1000];
            wchar_t SecondFile[1000];
            Edit_GetText(WindowInfo.LeftTextbox, FirstFile, 1000);
            Edit_GetText(WindowInfo.RightTextbox, SecondFile, 1000);
            if (LeftFile != NULL) CloseHandle(LeftFile);

            if (RightFile != NULL) CloseHandle(RightFile);
            /*HDC hdcLF = GetDC(WindowInfo.LeftTextWindow);*/

            HANDLE LeftFile_a = CreateFile(FirstFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            HANDLE RightFile_a = CreateFile(SecondFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            if (LeftFile_a == (HANDLE)0xffffffffffffffff && RightFile_a == (HANDLE)0xffffffffffffffff) {
                MessageBox(NULL, L"Exorcizo te, immundissime spiritus, omnis incursio adversarii, omne phantasma, omnis legio, in nomine Domini nostri Jesu Christi eradicare, et effugare ab hoc plasmate Dei. Ipse tibi imperat, qui te de supernis caelorum in inferiora terrae demergi praecepit. Ipse tibi imperat, qui mari, ventis, et tempestatibus impersvit. Audi ergo, et time, satana, inimice fidei, hostis generis humani, mortis adductor, vitae raptor, justitiae declinator, malorum radix, fomes vitiorum, seductor hominum, proditor gentium, incitator invidiae, origo avaritiae, causa discordiae, excitator dolorum: quid stas, et resistis, cum scias. Christum Dominum vias tuas perdere? Illum metue, qui in Isaac immolatus est, in joseph venumdatus, in sgno occisus, in homine cruci- fixus, deinde inferni triumphator fuit. Sequentes cruces fiant in fronte obsessi. Recede ergo in nomine Patris et Filii, et Spiritus Sancti: da locum Spiritui Sancto, per hoc signum sanctae Cruci Jesu Christi Domini nostri: Qui cum Patre et eodem Spiritu Sancto vivit et regnat Deus, Per omnia saecula saeculorum. Et cum spiritu tuo. Amen.",
                    L"Произошла ошибка при открытии файла:", MB_OK);
                /*ReleaseDC(WindowInfo.LeftTextWindow, hdcLF);*/
                CloseHandle(LeftFile_a);
                CloseHandle(RightFile_a);
                //ShellExecute(hWnd, L"open", L"https://sun3.tele2-nn.userapi.com/s/v1/ig2/exhJclZCmFUi4sWoYy0VYeJ2giBulR_5z4o0zfF-5E12Ib2lQZp_v3yK3tDxdJ7qj-zJBj-Q4r_DM5vHJ__l5lmV.jpg?size=1457x1600&quality=96&type=album", NULL, NULL, SW_SHOWDEFAULT); 
                break;
            }
           
    /*        SetScrollPos(WindowInfo.RightTextWindow, SB_VERT, 0, TRUE);
            SetScrollPos(WindowInfo.RightTextWindow, SB_HORZ, 0, TRUE);*/
            ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
            ScrolledFilesInfo.m_ScrollVerticalOffset = 0;

            ScrolledFilesInfo.GetLeftFileSize(LeftFile_a);
            ScrolledFilesInfo.GetRightFileSize(RightFile_a);
           
           // LARGE_INTEGER LeftFileSize = ScrolledFilesInfo.ReturnLeftFileSize();
            
            LeftFile = CreateFileMapping(LeftFile_a, NULL, PAGE_READONLY, 0, 0, NULL);
            RightFile = CreateFileMapping(RightFile_a, NULL, PAGE_READONLY, 0, 0, NULL);

            if (LeftFile == NULL && RightFile == NULL) { MessageBox(NULL, L"", L"Произошла ошибка при открытии файла:", MB_OK);  break; }

            for (int i = 0; i < 2; i++) {
            SetScrollPos(WindowInfo.m_UpdatingWindows[i], SB_VERT, 0, TRUE);
            SetScrollPos(WindowInfo.m_UpdatingWindows[i], SB_HORZ, 0, TRUE);
            SendMessage(WindowInfo.m_UpdatingWindows[i], WM_PAINT, 0, 0);
            }
           
           /* for (int i = 0; i < 2; i++) {
            
            }*/
           
           // SendMessage(WindowInfo.RightTextWindow, WM_PAINT, 0, 0);
           // GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.QuadPart, 0, 0, ScrolledFilesInfo.m_BytesOnString);

           /* ReleaseDC(WindowInfo.LeftTextWindow, hdcLF);*/

            CloseHandle(LeftFile_a);
            CloseHandle(RightFile_a);

            break;
        }

        case IDB_ListBox:{

            if (HIWORD(wParam) == LBN_SELCHANGE) {

                int I = (int)SendMessage(WindowInfo.List, LB_GETCURSEL, 0, 0L);
                ScrolledFilesInfo.m_BytesOnString = SendMessage(WindowInfo.List, LB_GETITEMDATA, I, 0L);
                LARGE_INTEGER LeftFileSize = ScrolledFilesInfo.ReturnLeftFileSize();
     /*           int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();
                ScrolledFilesInfo.m_ScrollVerticalOffset = max(0, (min(ceil(LeftFileSize.QuadPart / (double)ScrolledFilesInfo.m_BytesOnString) - Strings_On_Screen,
                    ScrolledFilesInfo.m_ScrollVerticalOffset)));*/
                ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
                for (int i = 0; i < 2; i++) {
                    SendNotifyMessage(WindowInfo.m_UpdatingWindows[i], WM_VSCROLL, LOWORD(-1), NULL);
                    ShowScrollBar(WindowInfo.m_UpdatingWindows[i], SB_HORZ, FALSE);
                }
                LONG HorizontalMaxScroll = ScrolledFilesInfo.HorizontalOffset();

                if (HorizontalMaxScroll != 0)
                {
                    for (int i = 0; i < 2; i++) {
                        SetScrollRange(WindowInfo.m_UpdatingWindows[i], SB_HORZ, 0, HorizontalMaxScroll, TRUE);
                        ShowScrollBar(WindowInfo.m_UpdatingWindows[i], SB_HORZ, TRUE);
                    }
                }
                for (int i = 0; i < 2; i++) {
                    SetScrollPos(WindowInfo.m_UpdatingWindows[i], SB_HORZ, 0, TRUE);
                }
                    ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
                
               /* SendNotifyMessage(WindowInfo.LeftTextWindow, WM_PAINT, 0, 0);
                SendMessage(WindowInfo.LeftTextWindow, WM_VSCROLL, LOWORD(-1), NULL);*/
                break;
            }

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
    switch (message)
    {
    case WM_MOUSEWHEEL: {
        if (int Wheel = GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            ScrolledFilesInfo.m_ScrollVerticalOffset -= 5;
        else
            ScrolledFilesInfo.m_ScrollVerticalOffset += 5;

        for (int i = 0; i < 2; i++) {
            SendMessage(WindowInfo.m_UpdatingWindows[i], WM_VSCROLL, -1L, 0L);
        }
        break;
    }
    case WM_SIZE: {
        ScrolledFilesInfo.GetNumStringsAndCharOnScreen(WindowInfo.RightTextWindow);
        LONG HorizontalMaxScroll = ScrolledFilesInfo.HorizontalOffset();
        if (HorizontalMaxScroll != 0)
        {
            SetScrollRange(WindowInfo.RightTextWindow, SB_HORZ, 0, HorizontalMaxScroll, TRUE);
            ShowScrollBar(WindowInfo.RightTextWindow, SB_HORZ, TRUE);
            SetScrollPos(WindowInfo.RightTextWindow, SB_HORZ, 0, TRUE);

        }
        else {
            ShowScrollBar(WindowInfo.RightTextWindow, SB_HORZ, FALSE);
            SetScrollPos(WindowInfo.RightTextWindow, SB_HORZ, 0, TRUE);
            ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
        }
        SendMessage(WindowInfo.RightTextWindow, WM_PAINT, 0, 0);
        break;
    }
    case WM_VSCROLL: {
        DWORDLONG ScrollButtonPos = 0;
        LONGLONG BiggestFileSize = ScrolledFilesInfo.ReturnBiggestFile();
        int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();

        switch (LOWORD(wParam)) {

        case SB_LINEUP:
            ScrolledFilesInfo.m_ScrollVerticalOffset -= 1;
            break;
        case SB_LINEDOWN:
            ScrolledFilesInfo.m_ScrollVerticalOffset += 1;
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
            ScrolledFilesInfo.m_ScrollVerticalOffset = ceil(ceil(BiggestFileSize / (double)ScrolledFilesInfo.m_BytesOnString) - Strings_On_Screen) * ((float)ScrollButtonPos / 1000);
            break;
        default:
            break;
        }

        if (RightFile != NULL)
        {

            if (ScrolledFilesInfo.m_ScrollVerticalOffset >= 18446744073709551614)
            {
                ScrolledFilesInfo.m_ScrollVerticalOffset = 0;
            }

            if (wParam != -1)
                SendNotifyMessage(WindowInfo.LeftTextWindow, WM_VSCROLL, -1L, -1L);

            ScrollButtonPos = floor(((ScrolledFilesInfo.m_ScrollVerticalOffset * 1000) / (ceil(BiggestFileSize / (double)ScrolledFilesInfo.m_BytesOnString) - Strings_On_Screen)));
            ScrollButtonPos = min(1000, ScrollButtonPos);
            ScrolledFilesInfo.m_ScrollVerticalOffset = max(0, (min(ceil(BiggestFileSize / (double)ScrolledFilesInfo.m_BytesOnString) - Strings_On_Screen,
                ScrolledFilesInfo.m_ScrollVerticalOffset)));
            SetScrollPos(WindowInfo.RightTextWindow, SB_VERT, ScrollButtonPos, TRUE);
            /*InvalidateRect(WindowInfo.LeftTextWindow,, TRUE);
               UpdateWindow(WindowInfo.LeftTextWindow);*/
            SendMessage(WindowInfo.RightTextWindow, WM_PAINT, 0, 0);
            /*GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.QuadPart,
                ScrolledFilesInfo.m_ScrollVerticalOffset, ScrolledFilesInfo.m_ScrollHorizontalOffset, ScrolledFilesInfo.m_BytesOnString);*/
        }
        break;
    }
    case WM_HSCROLL: {
        DWORDLONG ScrollHorizontalButtonPos = 0;
        LONGLONG BiggestFileSize = ScrolledFilesInfo.ReturnBiggestFile();
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
            break;
        }

        if (RightFile != NULL /*&& LeftFileSize.LowPart >= Strings_On_Screen * 8*/)
        {

            if (wParam != -1)
                SendNotifyMessage(WindowInfo.LeftTextWindow, WM_HSCROLL, -1L, -1L);
            int MaxRange = 0, MinRange = 0;
            GetScrollRange(WindowInfo.LeftTextWindow, SB_HORZ, &MinRange, &MaxRange);
            ScrolledFilesInfo.m_ScrollHorizontalOffset = min(MaxRange, ScrolledFilesInfo.m_ScrollHorizontalOffset);
            ScrolledFilesInfo.m_ScrollHorizontalOffset = max(MinRange, ScrolledFilesInfo.m_ScrollHorizontalOffset);
            ScrollHorizontalButtonPos = ScrolledFilesInfo.m_ScrollHorizontalOffset;
            ScrollHorizontalButtonPos = min(50, ScrollHorizontalButtonPos);

            SetScrollPos(WindowInfo.RightTextWindow, SB_HORZ, ScrollHorizontalButtonPos, TRUE);
            /* InvalidateRect(WindowInfo.LeftTextWindow, NULL, TRUE);*/
             //UpdateWindow(WindowInfo.LeftTextWindow);

            SendMessage(WindowInfo.RightTextWindow, WM_PAINT, 0, 0);

            /*GetEightBitsHex(LeftFile, ScrolledFilesInfo.ReturnGranularity(), LeftFileSize.QuadPart,
                ScrolledFilesInfo.m_ScrollVerticalOffset , ScrolledFilesInfo.m_ScrollHorizontalOffset, ScrolledFilesInfo.m_BytesOnString);*/

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
        if (RightFile != NULL) {
            LARGE_INTEGER RightFileSize = ScrolledFilesInfo.ReturnRightFileSize();
            GetEightBitsHex(WindowInfo.RightTextWindow,RightFile, ScrolledFilesInfo.ReturnGranularity(), RightFileSize.QuadPart,
                ScrolledFilesInfo.m_ScrollVerticalOffset, ScrolledFilesInfo.m_ScrollHorizontalOffset, ScrolledFilesInfo.m_BytesOnString);
        }
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
