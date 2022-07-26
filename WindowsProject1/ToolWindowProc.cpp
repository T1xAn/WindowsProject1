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

       HWND ReadButton = CreateWindowA("button", "Сравнить", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 750, 20, 30, 30, hWnd, (HMENU)IDB_ReadButton, WindowInfo.hInst, nullptr);
      CurrentWindowInfo->AddChildWindows(ReadButton, (char*)"ReadButton");

       HWND ChangeFont = CreateWindowA("button", "WORK IN PRIGRESS", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 0, 0, 0, 0,
            hWnd, (HMENU)IDB_Change_Font, WindowInfo.hInst, nullptr);
       CurrentWindowInfo->AddChildWindows(ChangeFont, (char*)"ChangeFont");

        HWND List = CreateWindowW(WC_LISTBOX, NULL, WS_CHILD | LBS_MULTICOLUMN | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, ceil(rect.right * 0.2),
            ceil(rect.bottom * 0.33) + 1, ceil(rect.right * 0.2), ceil(rect.bottom * 0.33) + 1, hWnd, (HMENU)IDB_ListBox, WindowInfo.hInst, nullptr);
        CurrentWindowInfo->AddChildWindows(List, (char*)"List");

        
        for (int count = 0; count < NUMBER_OF_WINDOWS; count++) {
            HWND TextBox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), NULL,
                WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, NULL, NULL, NULL);
            char* Key = (char*)calloc(50, sizeof(char));
            Comparator.GenerateKey(Key,TRUE);

            CurrentWindowInfo->AddChildWindows(TextBox, Key);
            Comparator.AddButtonsAndTextBox(Key, TextBox);

            char* KeyB = (char*)calloc(50, sizeof(char));

        Comparator.GenerateKey(KeyB, TRUE);

            HWND Search = CreateWindowA("button", ">>", WS_CHILD |
                WS_VISIBLE | WS_BORDER, 0, 0, 0,
                0, hWnd, NULL, WindowInfo.hInst, nullptr);
            CurrentWindowInfo->AddChildWindows(Search, KeyB);
            Comparator.AddButtonsAndTextBox(KeyB, Search);

        

        }

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

        std::vector <char*> ButtonsAndTextBox = Comparator.ReturnButtonTextBoxkeys();
        int left_offset = 1;

        for (int count = 0; count/2 < NUMBER_OF_WINDOWS; count+=2) {
            MoveWindow(CurrentWindowInfo->FindChildWindow(ButtonsAndTextBox[count]), left_offset, 1, (rect.right / NUMBER_OF_WINDOWS) - ((rect.right / NUMBER_OF_WINDOWS)*0.125), ceil(rect.bottom * 0.33), TRUE);
            left_offset += (rect.right / NUMBER_OF_WINDOWS) - ((rect.right / NUMBER_OF_WINDOWS) * 0.125)+1;
            MoveWindow(CurrentWindowInfo->FindChildWindow(ButtonsAndTextBox[count+1]), left_offset, 1, (rect.right / NUMBER_OF_WINDOWS) * 0.125, ceil(rect.bottom * 0.33), TRUE);
            left_offset += (rect.right / NUMBER_OF_WINDOWS) * 0.125+1;
        }

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


        case BN_CLICKED: {
            if (Comparator.CheckUpdatingWindows()) {
                int WindowKeyNum = (Comparator.FindWindowsWithHWND((HWND)lParam, TRUE));
                if  (WindowKeyNum % 2 != 0) {
                    OPENFILENAMEW File;
                    File = buttonGetFile();
                    wchar_t szFile[1000];
                    wcscpy_s(szFile, File.lpstrFile);
                    Edit_SetText(CurrentWindowInfo->ReturnChildWindows()[3+WindowKeyNum-1].second, szFile);
               }
            }
            break;
        }
        case IDB_ReadButton: {
            Comparator.ClearPages(ScrolledFilesInfo.ReturnLocalHeap());
            Comparator.CloseMainWindowsOpenFiles();
            Comparator.EnableWindowScrollBar(NULL, TRUE, TRUE);
            int TextBarNumber = 0;
            BOOL OpendFlag = FALSE;

            for (int count = 0; count < Comparator.ReturnUpdatingWindows().size(); count++) {
                wchar_t File[1000];
                Edit_GetText(CurrentWindowInfo->ReturnChildWindows()[3+TextBarNumber].second, File, 1000);

                HANDLE FileHandle = CreateFile(File, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                if (GetLastError() != 0) {
                    CloseHandle(FileHandle);
                    Comparator.EnableWindowScrollBar(Comparator.ReturnUpdatingWindowsKeys()[count], NULL, FALSE);
                    Edit_SetText(CurrentWindowInfo->ReturnChildWindows()[3 + TextBarNumber].second, L"");
                }

                HANDLE FileMap = CreateFileMapping(FileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
                if (GetLastError() != 0) {
                    CloseHandle(FileMap);
                    Comparator.UpdateKeyWindow(Comparator.ReturnUpdatingWindowsKeys()[count]);
                }

                if (FileMap != NULL) {
                    cWindowInfo* Window = Comparator.AddNewOpendFile(Comparator.ReturnUpdatingWindowsKeys()[count], FileMap, FileHandle);
                    ReadPage(FileMap, ScrolledFilesInfo.ReturnGranularity(), Window->ReturnFileSize().QuadPart, 0, ScrolledFilesInfo.m_BytesOnString, Window->ReturnWindowHWND());
                    OpendFlag = TRUE;
                }

                TextBarNumber += 2;
            }

            if (!OpendFlag) {
                MessageBox(hWnd, L"Ни один файл не был открыт", L" Ошибка ", MB_OK | MB_ICONERROR);
                break;
            }

            Comparator.ComparePages();

            ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
            ScrolledFilesInfo.m_ScrollVerticalOffset = 0;
            SetVerticalScrollRange();

           // CompareAllFiles();

            Comparator.DrawNewFiles();


            break;
        }

        case IDB_ListBox: {

            if (HIWORD(wParam) == LBN_SELCHANGE) {
                Comparator.ClearPages(ScrolledFilesInfo.ReturnLocalHeap());
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