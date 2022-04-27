#pragma once
#include "framework.h"

class ScrollFileInfo
{
public:

	ScrollFileInfo() {
		Scrolloffset = 0;
		Granularity = 0;
		LeftFileSize;
		RightFileSize;
		TextMetric;
	}

	~ScrollFileInfo() {};

	BOOL GetSystemGranularity()
	{
		SYSTEM_INFO SYSINF;
		GetSystemInfo(&SYSINF);
		Granularity = SYSINF.dwAllocationGranularity;
		if (Granularity == 0) 
			return false;
		return true;
	}

	BOOL GetLeftFileSize(HANDLE LeftFile) 
	{
		LeftFileSize.QuadPart = 0;
		GetFileSizeEx(LeftFile, &LeftFileSize);
		if (LeftFileSize.QuadPart == 0) 
			return false;
		return true;
	}

	BOOL GetTextMetric(HWND Window, HFONT FONT) {
		HDC hdc = GetDC(Window);
		SelectObject(hdc, FONT);
		//int indexHDC = SaveDC(hdc);
		 GetTextMetrics(hdc, &TextMetric);
		 ReleaseDC(Window, hdc);
		 HDC HDC = GetDC(Window);
		 //RestoreDC(HDC,-1);
		 return true;
	}
	BOOL GetNumStringsOnScreen(HWND Window) {
		RECT rt;
		GetClientRect(Window, &rt);

		if  (TextMetric.tmHeight == 0) 
			return false;

		Strings_on_screen = (rt.bottom / TextMetric.tmHeight);
		return true;
	}
	TEXTMETRIC ReturnTextMetric() 
	{
		return TextMetric;
	}

	DWORD ReturnGranularity() 
	{
		return Granularity;
	}

	int ReturnStringsOnScreen() 
	{
		return Strings_on_screen;
	}

	LARGE_INTEGER ReturnLeftFileSize() 
	{
		return LeftFileSize;
	}

	DWORDLONG  Scrolloffset;

private:
	DWORD Granularity;
	LARGE_INTEGER LeftFileSize;
	LARGE_INTEGER RightFileSize;
	TEXTMETRIC TextMetric;
	LONG Strings_on_screen;
};