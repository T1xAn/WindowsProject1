#include "framework.h"
#include "Resource.h"
#include "Functions_elements.h"

extern HANDLE LeftFile;
extern HANDLE RightFile;
extern ScrollFileInfo ScrolledFilesInfo;
extern MainWindows WindowInfo;


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
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 1, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc) - 5, 20, hWnd, NULL, NULL, NULL);
        WindowInfo.LeftSearch = CreateWindowA("button", ">>", WS_CHILD |
            WS_VISIBLE | WS_BORDER, ceil(rect.right / 2 - rect.right * 0.01 - sc) - 4, 1, ceil(rect.right * 0.01 + sc),
            20, hWnd, (HMENU)IDB_SearchButton_Left, WindowInfo.hInst, nullptr);
        WindowInfo.ReadButton = CreateWindowA("button", "Сравнить", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 750, 20, 30, 30, hWnd, (HMENU)IDB_ReadButton, WindowInfo.hInst, nullptr);
        WindowInfo.RightTextbox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("D:\\XP\\TurboServer\\TurboServer.vdi"),
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, rect.right / 2, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc), 20, hWnd, NULL, NULL, NULL);
        WindowInfo.RightSearch = CreateWindowA("button", ">>", WS_CHILD |
            WS_VISIBLE | WS_BORDER, ceil(rect.right - rect.right * 0.01 - sc) + 1, 1, ceil(rect.right * 0.01 + sc), 20,
            hWnd, (HMENU)IDB_SearchButton_Right, WindowInfo.hInst, nullptr);
        WindowInfo.ChangeFont = CreateWindowA("button", "WORK IN PRIGRESS", WS_CHILD |
            WS_VISIBLE | WS_BORDER, 0, 0, 0, 0,
            hWnd, (HMENU)IDB_Change_Font, WindowInfo.hInst, nullptr);

        WindowInfo.List = CreateWindowW(WC_LISTBOX, NULL, WS_CHILD | LBS_MULTICOLUMN | WS_BORDER | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT, ceil(rect.right * 0.2),
            ceil(rect.bottom * 0.33) + 1, ceil(rect.right * 0.2), ceil(rect.bottom * 0.33) + 1, hWnd, (HMENU)IDB_ListBox, WindowInfo.hInst, nullptr);

        SendMessage(WindowInfo.List, LB_ADDSTRING, 0, (LPARAM)L"8");
        SendMessage(WindowInfo.List, LB_ADDSTRING, 0, (LPARAM)L"16");
        SendMessage(WindowInfo.List, LB_ADDSTRING, 0, (LPARAM)L"32");
        for (int count = 0; count < 3; count++) {
            int data = pow(2, count + 3);
            SendMessage(WindowInfo.List, LB_SETITEMDATA, count, (LPARAM)data);

        }
        //SendMessage(WindowInfo.List, LB_SETCURSEL, 0, 0L)
    }

    case WM_SIZE: {
        RECT rect;
        GetClientRect(hWnd, &rect);

        int sc = GetSystemMetrics(SM_CXVSCROLL);
        MoveWindow(WindowInfo.LeftTextbox, 1, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc) - 5, ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(WindowInfo.LeftSearch, ceil(rect.right / 2 - rect.right * 0.01 - sc) - 4, 1, ceil(rect.right * 0.01 + sc), ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(WindowInfo.RightTextbox, rect.right / 2, 1, ceil(rect.right / 2 - rect.right * 0.01 - sc), ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(WindowInfo.RightSearch, ceil(rect.right - rect.right * 0.01 - sc) + 1, 1, ceil(rect.right * 0.01 + sc), ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(WindowInfo.ReadButton, 1, ceil(rect.bottom * 0.33) + 1, ceil(rect.right * 0.2), ceil(rect.bottom * 0.33), TRUE);
        MoveWindow(WindowInfo.ChangeFont, 1, (ceil(rect.bottom * 0.33) + 1) * 2, ceil(rect.right * 0.2), ceil(rect.bottom * 0.3), TRUE);
        MoveWindow(WindowInfo.List, ceil(rect.right * 0.2), ceil(rect.bottom * 0.33) + 1, ceil(rect.right * 0.2), ceil(rect.bottom * 0.66) + 1, TRUE);
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


            wchar_t FirstFile[1000];
            wchar_t SecondFile[1000];
            Edit_GetText(WindowInfo.LeftTextbox, FirstFile, 1000);
            Edit_GetText(WindowInfo.RightTextbox, SecondFile, 1000);
            if (LeftFile != NULL) CloseHandle(LeftFile);

            if (RightFile != NULL) CloseHandle(RightFile);

            EnableScrollBar(WindowInfo.LeftTextWindow, SB_VERT, ESB_ENABLE_BOTH);
            EnableScrollBar(WindowInfo.RightTextWindow, SB_VERT, ESB_ENABLE_BOTH);
            HANDLE LeftFile_a = CreateFile(FirstFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (GetLastError() != 0) {
                MessageBox(WindowInfo.ToolBar, L" Левый файл не был открыт",
                    L"Внимание:", MB_OK | MB_ICONEXCLAMATION);
                CloseHandle(LeftFile_a);
                EnableScrollBar(WindowInfo.LeftTextWindow, SB_VERT, ESB_DISABLE_BOTH);
                Edit_SetText(WindowInfo.LeftTextbox, L"");
            }
            HANDLE RightFile_a = CreateFile(SecondFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if (GetLastError() != 0) {
                MessageBox(WindowInfo.ToolBar, L" Правый файл не был открыт",
                    L"Внимание: ", MB_OK | MB_ICONEXCLAMATION);
                CloseHandle(RightFile_a);
                EnableScrollBar(WindowInfo.RightTextWindow, SB_VERT, ESB_DISABLE_BOTH);
                Edit_SetText(WindowInfo.RightTextbox, L"");
            }

            ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
            ScrolledFilesInfo.m_ScrollVerticalOffset = 0;

            ScrolledFilesInfo.GetLeftFileSize(LeftFile_a);
            ScrolledFilesInfo.GetRightFileSize(RightFile_a);

            SetVerticalScrollRange();

            LeftFile = CreateFileMapping(LeftFile_a, NULL, PAGE_READONLY, 0, 0, NULL);
            if (GetLastError() != 0) {
                CloseHandle(LeftFile);
                InvalidateRect(WindowInfo.LeftTextWindow, NULL, TRUE);
                UpdateWindow(WindowInfo.LeftTextWindow);
            }
            RightFile = CreateFileMapping(RightFile_a, NULL, PAGE_READONLY, 0, 0, NULL);
            if (GetLastError() != 0) {
                CloseHandle(RightFile);
                InvalidateRect(WindowInfo.RightTextWindow, NULL, TRUE);
                UpdateWindow(WindowInfo.RightTextWindow);
            }
            if (LeftFile == NULL && RightFile == NULL) { MessageBox(WindowInfo.ToolBar, L"Оба файла не были открыты", L" Ошибка ", MB_OK | MB_ICONERROR);  break; }

            for (int i = 0; i < 2; i++) {
                SetScrollPos(WindowInfo.m_UpdatingWindows[i], SB_VERT, 0, TRUE);
                SetScrollPos(WindowInfo.m_UpdatingWindows[i], SB_HORZ, 0, TRUE);
                SendMessage(WindowInfo.m_UpdatingWindows[i], WM_PAINT, 0, 0);
            }

            CloseHandle(LeftFile_a);
            CloseHandle(RightFile_a);

            break;
        }

        case IDB_ListBox: {

            if (HIWORD(wParam) == LBN_SELCHANGE) {

                int I = (int)SendMessage(WindowInfo.List, LB_GETCURSEL, 0, 0L);
                double PrewBytesOnString = ScrolledFilesInfo.m_BytesOnString;
                ScrolledFilesInfo.m_BytesOnString = SendMessage(WindowInfo.List, LB_GETITEMDATA, I, 0L);
                int BytesOnfStringNumenator = max(PrewBytesOnString, ScrolledFilesInfo.m_BytesOnString);
                LARGE_INTEGER LeftFileSize = ScrolledFilesInfo.ReturnLeftFileSize();

                ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
                ScrolledFilesInfo.m_ScrollVerticalOffset = floor((ScrolledFilesInfo.m_ScrollVerticalOffset * (PrewBytesOnString / ScrolledFilesInfo.m_BytesOnString)));
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