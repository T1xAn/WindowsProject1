#pragma once
#include "framework.h"
#include "Resource.h"

extern HANDLE LeftFile;
extern HANDLE RightFile;
extern ScrollFileInfo ScrolledFilesInfo ;
extern MainWindows WindowInfo;

OPENFILENAMEW buttonGetFile();
BOOL SetVerticalScrollRange();
HDC GetEightBitsHex(_In_ HWND Window, _In_ HANDLE File, _In_ DWORD Granularity, _In_ DWORDLONG FileSize, _In_ DWORDLONG OFFSET,
    _In_ LONG HorizontalOffset, _In_ LONG BytesOnString, _Inout_ HDC BlitHDC);
HDC CompareTwoFiles(_In_ HANDLE WindowFile, _In_ DWORDLONG WindowFileSize, _In_ HANDLE ComparableFile, _In_ DWORDLONG ComparableFileSize, _Inout_ HDC DrawDC,
    _In_ DWORDLONG Offset, _In_ LONG HorizontalOffset, _In_ LONG BytesOnString, _In_ DWORD Granularity);