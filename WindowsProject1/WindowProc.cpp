#include "framework.h"
#include "Resource.h"
#include "Functions_elements.h"

extern HANDLE LeftFile;
extern HANDLE RightFile;
extern ScrollFileInfo ScrolledFilesInfo;
extern MainWindows WindowInfo;

//
//  ФУНКЦИЯ: OutWindowsProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в окнах вывода.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK OutWindowsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case WM_MOUSEWHEEL: {
        if (int Wheel = GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
            if (ScrolledFilesInfo.m_ScrollVerticalOffset <= 5)
                ScrolledFilesInfo.m_ScrollVerticalOffset = 0;
            else
                ScrolledFilesInfo.m_ScrollVerticalOffset -= 5;
        }
        else
            ScrolledFilesInfo.m_ScrollVerticalOffset += 5;

        for (int i = 0; i < 2; i++) {
            SendMessage(WindowInfo.m_UpdatingWindows[i], WM_VSCROLL, -1L, 0L);
        }

        break;
    }

    case WM_SIZE: {
        int PrewStringsOnScreen = ScrolledFilesInfo.ReturnStringsOnScreen();
        ScrolledFilesInfo.GetNumStringsAndCharOnScreen(hWnd);
        if (ScrolledFilesInfo.m_ScrollVerticalOffset ==
            (ceil(ScrolledFilesInfo.ReturnBiggestFile() / (double)ScrolledFilesInfo.m_BytesOnString) - PrewStringsOnScreen)) {
            ScrolledFilesInfo.m_ScrollVerticalOffset =
                (ceil(ScrolledFilesInfo.ReturnBiggestFile() / (double)ScrolledFilesInfo.m_BytesOnString) - ScrolledFilesInfo.ReturnStringsOnScreen());
        }
        LONG HorizontalMaxScroll = ScrolledFilesInfo.HorizontalOffset();
        if (HorizontalMaxScroll != 0)
        {
            SetScrollRange(hWnd, SB_HORZ, 0, HorizontalMaxScroll, TRUE);
            ShowScrollBar(hWnd, SB_HORZ, TRUE);
            SetScrollPos(hWnd, SB_HORZ, 0, TRUE);

        }
        else {
            ShowScrollBar(hWnd, SB_HORZ, FALSE);
            SetScrollPos(hWnd, SB_HORZ, 0, TRUE);
            ScrolledFilesInfo.m_ScrollHorizontalOffset = 0;
        }

        break;
    }
    case WM_VSCROLL: {
        HWND OtherWindow;
        HANDLE CurrentFile;
        if (hWnd == WindowInfo.LeftTextWindow) {
            OtherWindow = WindowInfo.RightTextWindow;
            CurrentFile = LeftFile;
        }
        else {
            OtherWindow = WindowInfo.LeftTextWindow;
            CurrentFile = RightFile;
        }
        SCROLLINFO Scrollinfo;
        Scrollinfo.cbSize = sizeof(SCROLLBARINFO);
        Scrollinfo.fMask = SIF_RANGE;
        GetScrollInfo(hWnd, SB_VERT, &Scrollinfo);
        DWORD ScrollRange = Scrollinfo.nMax;
        Scrollinfo.fMask = SIF_TRACKPOS;
        GetScrollInfo(hWnd, SB_VERT, &Scrollinfo);
        LONGLONG ScrollButtonPos = 0;
        LONGLONG BiggestFileSize = ScrolledFilesInfo.ReturnBiggestFile();
        int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();

        switch (LOWORD(wParam)) {

        case SB_LINEUP:
            if (ScrolledFilesInfo.m_ScrollVerticalOffset != 0)
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
        case SB_THUMBTRACK:
            ScrollButtonPos = Scrollinfo.nTrackPos;
            ScrolledFilesInfo.m_ScrollVerticalOffset = ceil(ceil(BiggestFileSize / (double)ScrolledFilesInfo.m_BytesOnString) - Strings_On_Screen) * ((double)ScrollButtonPos / ScrollRange);
            break;
        default:
            break;
        }
        

        if (CurrentFile != NULL)
        {

            if (wParam != -1)
                SendNotifyMessage(OtherWindow, WM_VSCROLL, -1L, -1L);


            ScrollButtonPos = floor(((ScrolledFilesInfo.m_ScrollVerticalOffset * ScrollRange) / (ceil(BiggestFileSize / (double)ScrolledFilesInfo.m_BytesOnString) - Strings_On_Screen)));
            ScrollButtonPos = min(ScrollRange, ScrollButtonPos);
            ScrolledFilesInfo.m_ScrollVerticalOffset = max(0, (min(ceil(BiggestFileSize / (double)ScrolledFilesInfo.m_BytesOnString) - Strings_On_Screen,
                ScrolledFilesInfo.m_ScrollVerticalOffset)));
            SetScrollPos(hWnd, SB_VERT, ScrollButtonPos, TRUE);
            SendMessage(hWnd, WM_PAINT, 0, 0);

        }
        break;
    }
    case WM_HSCROLL: {

        HWND OtherWindow;
        HANDLE CurrentFile;
        if (hWnd == WindowInfo.LeftTextWindow) {
            OtherWindow = WindowInfo.RightTextWindow;
            CurrentFile = LeftFile;
        }
        else {
            OtherWindow = WindowInfo.LeftTextWindow;
            CurrentFile = RightFile;
        }

        DWORDLONG ScrollHorizontalButtonPos = 0;
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

        if (CurrentFile != NULL)
        {


            if (wParam != -1)
                SendNotifyMessage(OtherWindow, WM_HSCROLL, -1L, -1L);

            int MaxRange = 0, MinRange = 0;
            GetScrollRange(hWnd, SB_HORZ, &MinRange, &MaxRange);
            ScrolledFilesInfo.m_ScrollHorizontalOffset = min(MaxRange, ScrolledFilesInfo.m_ScrollHorizontalOffset);
            ScrolledFilesInfo.m_ScrollHorizontalOffset = max(MinRange, ScrolledFilesInfo.m_ScrollHorizontalOffset);
            ScrollHorizontalButtonPos = ScrolledFilesInfo.m_ScrollHorizontalOffset;
            ScrollHorizontalButtonPos = min(50, ScrollHorizontalButtonPos);

            SetScrollPos(hWnd, SB_HORZ, ScrollHorizontalButtonPos, TRUE);

            SendMessage(hWnd, WM_PAINT, 0, 0);

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

        HANDLE OtherFile;
        HANDLE CurrentFile;
        LARGE_INTEGER CurrentFileSize, OtherFileSize;
        if (hWnd == WindowInfo.LeftTextWindow) {
            OtherFile = RightFile;
            CurrentFileSize = ScrolledFilesInfo.ReturnLeftFileSize();
            OtherFileSize = ScrolledFilesInfo.ReturnRightFileSize();
            CurrentFile = LeftFile;
        }
        else {
            OtherFile = LeftFile;
            CurrentFileSize = ScrolledFilesInfo.ReturnRightFileSize();
            OtherFileSize = ScrolledFilesInfo.ReturnLeftFileSize();
            CurrentFile = RightFile;
        }

        if (CurrentFile != NULL) {
            RECT rc;
            GetClientRect(hWnd, &rc);
            hdc = GetDC(hWnd);
            HDC BlitHDC = CreateCompatibleDC(hdc);
            HBITMAP BitMap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            SelectObject(BlitHDC, BitMap);

            DeleteObject(BitMap);
            BitBlt(BlitHDC, 0, 0, rc.right, rc.bottom, BlitHDC, 0, 0, WHITENESS);

            BlitHDC = GetEightBitsHex(hWnd, CurrentFile, ScrolledFilesInfo.ReturnGranularity(), CurrentFileSize.QuadPart,
                ScrolledFilesInfo.m_ScrollVerticalOffset, ScrolledFilesInfo.m_ScrollHorizontalOffset, ScrolledFilesInfo.m_BytesOnString, BlitHDC);
            if (OtherFile != NULL) {
                BlitHDC = CompareTwoFiles(CurrentFile, CurrentFileSize.QuadPart, OtherFile, OtherFileSize.QuadPart, BlitHDC, ScrolledFilesInfo.m_ScrollVerticalOffset,
                    ScrolledFilesInfo.m_ScrollHorizontalOffset, ScrolledFilesInfo.m_BytesOnString, ScrolledFilesInfo.ReturnGranularity());
            }

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, BlitHDC, 0, 0, SRCCOPY);
            DeleteDC(BlitHDC);
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