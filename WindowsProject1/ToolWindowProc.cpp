#include "framework.h"
#include "Resource.h"
#include "Functions_elements.h"

extern HANDLE LeftFile;
extern HANDLE RightFile;
extern ScrollFileInfo ScrolledFilesInfo;
extern MainWindows WindowInfo;
extern  COMPARATOR Comparator;

LRESULT CALLBACK ToolProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rect;
    switch (message)
    {
    case WM_CREATE: {
        RECT rect;
        GetClientRect(hWnd, &rect);
        int sc = GetSystemMetrics(SM_CXVSCROLL);
        cWindowInfo* CurrentWindowInfo = new cWindowInfo;
        CurrentWindowInfo->SetWindowHandle(hWnd);
        CurrentWindowInfo->SetWindowKey((char*)"ToolBarWindow");
        
      HWND LeftTextBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("D:\\BFF.rar"),
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 1, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc) - 5, 20, hWnd, NULL, NULL, NULL);
    CurrentWindowInfo->AddChildWindows(LeftTextBox, (char*)"LeftTextBox");

        HWND LeftSearch = CreateWindowA("button", ">>", WS_CHILD |
            WS_VISIBLE | WS_BORDER, ceil(rect.right / 2 - rect.right * 0.01 - sc) - 4, 1, ceil(rect.right * 0.01 + sc),
            20, hWnd, (HMENU)IDB_SearchButton_Left, WindowInfo.hInst, nullptr);
        CurrentWindowInfo->AddChildWindows(LeftSearch, (char*)"LeftSearch");

       HWND ReadButton = CreateWindowA("button", "Сравнить", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 750, 20, 30, 30, hWnd, (HMENU)IDB_ReadButton, WindowInfo.hInst, nullptr);
      CurrentWindowInfo->AddChildWindows(ReadButton, (char*)"ReadButton");

      HWND RightTextBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("D:\\XP\\TurboServer\\TurboServer.vdi"),
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, rect.right / 2, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc), 20, hWnd, NULL, NULL, NULL);
      CurrentWindowInfo->AddChildWindows(RightTextBox, (char*)"RightTextBox");
      
      HWND RightSearch = CreateWindowA("button", ">>", WS_CHILD |
            WS_VISIBLE | WS_BORDER, ceil(rect.right - rect.right * 0.01 - sc) + 1, 1, ceil(rect.right * 0.01 + sc), 20,
            hWnd, (HMENU)IDB_SearchButton_Right, WindowInfo.hInst, nullptr);
      CurrentWindowInfo->AddChildWindows(RightSearch, (char*)"RightSearch");

       HWND ChangeFont = CreateWindowA("button", "WORK IN PRIGRESS", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 0, 0, 0, 0,
            hWnd, (HMENU)IDB_Change_Font, WindowInfo.hInst, nullptr);
       CurrentWindowInfo->AddChildWindows(ChangeFont, (char*)"ChangeFont");

        HWND List = CreateWindowW(WC_LISTBOX, NULL, WS_CHILD | LBS_MULTICOLUMN | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, ceil(rect.right * 0.2),
            ceil(rect.bottom * 0.33) + 1, ceil(rect.right * 0.2), ceil(rect.bottom * 0.33) + 1, hWnd, (HMENU)IDB_ListBox, WindowInfo.hInst, nullptr);
        CurrentWindowInfo->AddChildWindows(List, (char*)"List");

        //for (int count = 0; count < NUMBER_OF_WINDOWS; count++) {
        //    HWND TextBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("D:\\BFF.rar"),
        //        WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 1, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc) - 5, 20, hWnd, NULL, NULL, NULL);
            char* Key = (char*)calloc(50, sizeof(char));
        //    Comparator.GenerateKey(Key);
        //    char keyF[50];
        //    strcpy_s(keyF, 50, Key);
        //    CurrentWindowInfo->AddChildWindows(TextBox, keyF);
        ZeroMemory(Key, sizeof(Key));
        //    HWND LeftSearch = CreateWindowA("button", ">>", WS_CHILD |
        //        WS_VISIBLE | WS_BORDER, ceil(rect.right / 2 - rect.right * 0.01 - sc) - 4, 1, ceil(rect.right * 0.01 + sc),
        //        20, hWnd, (HMENU)IDB_SearchButton_Left, WindowInfo.hInst, nullptr);
        //    CurrentWindowInfo->AddChildWindows(LeftSearch, (char*)"LeftSearch");

        //}
      /*  HWND OpenNewWindow = CreateWindowA("button", "Сравнить", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 750, 20, 30, 30, hWnd, (HMENU)IDB_ReadButton, WindowInfo.hInst, nullptr);*/

        Comparator.AddWindows(List);

        SendMessage(List, LB_ADDSTRING, 0, (LPARAM)L"8");
        SendMessage(List, LB_ADDSTRING, 0, (LPARAM)L"16");
        SendMessage(List, LB_ADDSTRING, 0, (LPARAM)L"32");
        for (int count = 0; count < 3; count++) {
            int data = pow(2, count + 3);
            SendMessage(List, LB_SETITEMDATA, count, (LPARAM)data);

        }
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)CurrentWindowInfo);
    }

    case WM_SIZE: {
        RECT rect;
        GetClientRect(hWnd, &rect);
        cWindowInfo *CurrentWindowInfo = (cWindowInfo*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

        int sc = GetSystemMetrics(SM_CXVSCROLL);
        MoveWindow(CurrentWindowInfo->FindChildWindow((char*)"LeftTextBox"), 1, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc) - 5, ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(CurrentWindowInfo->FindChildWindow((char*)"LeftSearch"), ceil(rect.right / 2 - rect.right * 0.01 - sc) - 4, 1, ceil(rect.right * 0.01 + sc), ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(CurrentWindowInfo->FindChildWindow((char*)"RightTextBox"), rect.right / 2, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc), ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(CurrentWindowInfo->FindChildWindow((char*)"RightSearch"), ceil(rect.right - rect.right * 0.01 - sc) + 1, 1, ceil(rect.right * 0.01 + sc), ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(CurrentWindowInfo->FindChildWindow((char*)"ReadButton"), 1, ceil(rect.bottom * 0.33) + 1, ceil(rect.right * 0.2), ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(CurrentWindowInfo->FindChildWindow((char*)"ChangeFont"), 1, (ceil(rect.bottom * 0.33) + 1) * 2, ceil(rect.right * 0.2), ceil(rect.bottom * 0.3), TRUE);
        MoveWindow(CurrentWindowInfo->FindChildWindow((char*)"List"), ceil(rect.right * 0.2), ceil(rect.bottom * 0.33) + 1, ceil(rect.right * 0.2), ceil(rect.bottom * 0.66) + 1, TRUE);
        break;
    }
    case WM_COMMAND:
    {

        int wmId = LOWORD(wParam);
        cWindowInfo* CurrentWindowInfo = (cWindowInfo*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        // Разобрать выбор в меню:
        switch (wmId)
        {

        case IDB_SearchButton_Left: {
            OPENFILENAMEW File;
            File = buttonGetFile();
            wchar_t szFile[1000];
            wcscpy_s(szFile, File.lpstrFile);
            Edit_SetText(CurrentWindowInfo->FindChildWindow((char*)"LeftTextBox"), szFile);
            break;
        }
        case IDB_SearchButton_Right: {
            OPENFILENAMEW File;
            File = buttonGetFile();
            wchar_t szFile[1000];
            wcscpy_s(szFile, File.lpstrFile);
            Edit_SetText(CurrentWindowInfo->FindChildWindow((char*)"RightTextBox"), szFile);
            break;
        }
        case IDB_ReadButton: {

            wchar_t FirstFile[1000];
            wchar_t SecondFile[1000];
            Edit_GetText(CurrentWindowInfo->FindChildWindow((char*)"LeftTextBox"), FirstFile, 1000);
            Edit_GetText(CurrentWindowInfo->FindChildWindow((char*)"RightTextBox"), SecondFile, 1000);

            /*if (Comparator.m_LeftFile != NULL) Comparator.CloseFileL();

            if (Comparator.m_RightFile != NULL) Comparator.CloseFileR();*/

            Comparator.CloseMainWindowsOpenFiles();

            Comparator.EnableWindowScrollBar(NULL, TRUE, TRUE);

            HANDLE LeftFile_a = CreateFile(FirstFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (GetLastError() != 0) {
                MessageBox(hWnd, L" Левый файл не был открыт",
                    L"Внимание:", MB_OK | MB_ICONEXCLAMATION);
                CloseHandle(LeftFile_a);
                Comparator.EnableWindowScrollBar((char*)"LeftWindow", NULL, FALSE);
                Edit_SetText(CurrentWindowInfo->FindChildWindow((char*)"LeftTextBox"), L"");
            }
            HANDLE RightFile_a = CreateFile(SecondFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (GetLastError() != 0) {
                MessageBox(hWnd, L" Правый файл не был открыт",
                    L"Внимание: ", MB_OK | MB_ICONEXCLAMATION);
                CloseHandle(RightFile_a);
                Comparator.EnableWindowScrollBar((char*)"RightWindow", NULL, FALSE);
                Edit_SetText(CurrentWindowInfo->FindChildWindow((char*)"RightTextBox"), L"");
            }

            ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
            ScrolledFilesInfo.m_ScrollVerticalOffset = 0;


            SetVerticalScrollRange();

            /// Maping errors comparator (function)
             HANDLE LeftFileN = CreateFileMapping(LeftFile_a, NULL, PAGE_READONLY, 0, 0, NULL);
             
            if (GetLastError() != 0) {
                CloseHandle(LeftFileN);
                Comparator.UpdateKeyWindow((char*)"LeftWindow");
            }

            if(LeftFileN != NULL)
            Comparator.AddNewOpendFile((char*)"LeftWindow", LeftFileN, LeftFile_a);

            HANDLE RightFileN = CreateFileMapping(RightFile_a, NULL, PAGE_READONLY, 0, 0, NULL);
            if (GetLastError() != 0) {
                CloseHandle(RightFileN);
                Comparator.UpdateKeyWindow((char*)"RightWindow");
            }
            if (RightFileN != NULL)
            Comparator.AddNewOpendFile((char*)"RightWindow", RightFileN, RightFile_a);
            if (LeftFileN == NULL && RightFileN == NULL) { MessageBox(hWnd, L"Оба файла не были открыты", L" Ошибка ", MB_OK | MB_ICONERROR);  break; }

           

            Comparator.DrawNewFiles();

            //CloseHandle(LeftFile_a);
            //CloseHandle(RightFile_a);

            break;
        }

        case IDB_ListBox: {

            if (HIWORD(wParam) == LBN_SELCHANGE) {

                HWND List = CurrentWindowInfo->FindChildWindow((char*)"List");
                int I = (int)SendMessage(List, LB_GETCURSEL, 0, 0L);
                double PrewBytesOnString = ScrolledFilesInfo.m_BytesOnString;
                ScrolledFilesInfo.m_BytesOnString = SendMessage(List, LB_GETITEMDATA, I, 0L);
                int BytesOnfStringNumenator = max(PrewBytesOnString, ScrolledFilesInfo.m_BytesOnString);

                ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
                ScrolledFilesInfo.m_ScrollVerticalOffset = floor((ScrolledFilesInfo.m_ScrollVerticalOffset * (PrewBytesOnString / ScrolledFilesInfo.m_BytesOnString)));
                
                Comparator.HideScrollBars();

                LONG HorizontalMaxScroll = ScrolledFilesInfo.HorizontalOffset();
                if (HorizontalMaxScroll != 0)
                {
                    Comparator.SetNewHorizontalScrollRange(HorizontalMaxScroll);
                }
             
                Comparator.ResetHorizontalScroll();
                ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
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