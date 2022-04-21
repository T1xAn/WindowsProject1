#pragma once
#include "framework.h"
class ScrollFileInfo{
private:
	DWORD Granularity;
	LARGE_INTEGER LeftFileSize;
	LARGE_INTEGER RightFileSize;
	TEXTMETRIC TextMetric;
	int Strings_on_screen;
public:

 int  Scrolloffset;

	ScrollFileInfo() {
		Scrolloffset = 0;
		Granularity = 0;
		LeftFileSize;
		RightFileSize;
	}
	BOOL GetSystemGranularity(){
		SYSTEM_INFO SYSINF;
		GetSystemInfo(&SYSINF);
		Granularity = SYSINF.dwAllocationGranularity;
		if (Granularity == 0) return false;
		return true;
	}
	BOOL GetLeftFileSize(HANDLE LeftFile) {
		LeftFileSize.QuadPart = 0;
		GetFileSizeEx(LeftFile, &LeftFileSize);
		if (LeftFileSize.QuadPart == 0) return false;
		return true;
	}
	BOOL GetTextMetric(HWND Window) {
		HDC hdc = GetDC(Window);
		//if (GetLastError() != 0) return false;
		GetTextMetrics(hdc, &TextMetric);
		ReleaseDC(Window, hdc);
		return true;
	}
	BOOL GetNumStringsOnScreen(HWND Window) {
		RECT rt;
		GetClientRect(Window, &rt);
		if  (TextMetric.tmHeight == 0) return false;
		Strings_on_screen = (rt.bottom / TextMetric.tmHeight);
		return true;
	}
	TEXTMETRIC ReturnTextMetric() {
		return TextMetric;
	}
	DWORD ReturnGranularity() {
		return Granularity;
	}
	int ReturnStringsOnScreen() {
		return Strings_on_screen;
	}
	LARGE_INTEGER ReturnLeftFileSize() {
		return LeftFileSize;
	}
	~ScrollFileInfo() {};
};