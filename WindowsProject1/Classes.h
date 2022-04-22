#pragma once
#include "framework.h"
struct TextParam {
		int max_char_width;
		SIZE SIZE_MAIN_STRING;
	};
class ScrollFileInfo{
private:
	DWORD Granularity;
	LARGE_INTEGER LeftFileSize;
	LARGE_INTEGER RightFileSize;
	//TEXTMETRIC TextMetric;
	int Strings_on_screen;
	TextParam Text_Param;
public:

 int  Scrolloffset;

	ScrollFileInfo() {
		Scrolloffset = 0;
		Granularity = 0;
		LeftFileSize;
		RightFileSize;
		Text_Param.max_char_width = 0;
		Text_Param.SIZE_MAIN_STRING;
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
	BOOL GetTextMetric(HWND Window, HFONT FONT) {
		HDC hdc = GetDC(Window);
		 SelectObject(hdc, FONT);
		 ABCFLOAT ABCDEF[6];
		 GetCharABCWidthsFloat(hdc, 0x41, 0x46, ABCDEF);
		 Text_Param.max_char_width = 0;
		 for (int i = 0; i < 6; i++) {
			 if (ABCDEF[i].abcfB > Text_Param.max_char_width) Text_Param.max_char_width = ABCDEF[i].abcfB;
		 }
		 if (Text_Param.max_char_width == 0) return false;
		 GetTextExtentPoint32A(hdc, " ", 1, &Text_Param.SIZE_MAIN_STRING);
		 Text_Param.SIZE_MAIN_STRING.cx = Text_Param.SIZE_MAIN_STRING.cx*8 + Text_Param.max_char_width * 17;
		 ReleaseDC(Window, hdc);
		 return true;
	}
	BOOL GetNumStringsOnScreen(HWND Window) {
		RECT rt;
		GetClientRect(Window, &rt);
		if  (Text_Param.SIZE_MAIN_STRING.cy == 0) return false;
		Strings_on_screen = (rt.bottom / Text_Param.SIZE_MAIN_STRING.cy);
		return true;
	}
	TextParam ReturnTextMetric() {
		return Text_Param;
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