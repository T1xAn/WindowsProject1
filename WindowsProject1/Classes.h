#pragma once
#include "framework.h"

class ScrollFileInfo
{
public:

	ScrollFileInfo() {
		m_ScrollVerticalOffset = 0;
		m_Granularity = 0;
		m_CharOnScreen = 0;
		m_ScrollHorizontalOffset = 0;
		m_LeftFileSize;
		m_RightFileSize;
		m_TextMetric;
	}

	~ScrollFileInfo() {};

	BOOL GetSystemGranularity()
	{
		SYSTEM_INFO SYSINF;
		GetSystemInfo(&SYSINF);
		m_Granularity = SYSINF.dwAllocationGranularity;
		if (m_Granularity == 0) 
			return false;
		return true;
	}

	BOOL GetLeftFileSize(HANDLE LeftFile) 
	{
		m_LeftFileSize.QuadPart = 0;
		GetFileSizeEx(LeftFile, &m_LeftFileSize);

		if (m_LeftFileSize.QuadPart == 0) 
			return false;

		return true;
	}

	BOOL GetTextMetric(HWND Window, HFONT FONT) {
		HDC hdc = GetDC(Window);
		SelectObject(hdc, FONT);
		//int indexHDC = SaveDC(hdc);
		 GetTextMetrics(hdc, &m_TextMetric);
		 ReleaseDC(Window, hdc);
		/* HDC HDC = GetDC(Window);*/
		 //RestoreDC(HDC,-1);
		 return true;
	}

	BOOL GetNumStringsAndCharOnScreen(HWND Window) {
		RECT rt;
		GetClientRect(Window, &rt);

		if  (m_TextMetric.tmHeight == 0) 
			return false;

		m_Strings_on_screen = (rt.bottom / m_TextMetric.tmHeight);
		m_CharOnScreen = (rt.right*m_TextMetric.tmAveCharWidth);

		return true;
	}

// Функция возвращает метрики выбранного шрифта
	TEXTMETRIC ReturnTextMetric() 
	{
		return m_TextMetric;
	}

// Функция возвращает гранулярность системы
	DWORD ReturnGranularity() 
	{
		return m_Granularity;
	}

// Функция возвращает максимальное колличство строк, которое может поместиться на экране
	LONG ReturnStringsOnScreen() 
	{
		return m_Strings_on_screen;
	}

// Функция возвращает максимальное колличество символов, которое может поместиться на экране
	LONG ReturnCharsOnScreen() {
		return m_CharOnScreen;
	}

// Функция возвращает размер файла, открытго в левом окне
	LARGE_INTEGER ReturnLeftFileSize() 
	{
		return m_LeftFileSize;
	}

	DWORDLONG  m_ScrollVerticalOffset;
	LONG m_ScrollHorizontalOffset;

private:
	DWORD m_Granularity; // Гранулярность системы
	LARGE_INTEGER m_LeftFileSize; //Размер файла открытого в левом окне
	LARGE_INTEGER m_RightFileSize; //Размер файла открытого в правом окне
	TEXTMETRIC m_TextMetric; // Параметры выбранного шрифта
	LONG m_Strings_on_screen; // Целое колличество строк которое может поместиться на экране
	LONG m_CharOnScreen; // Целое колличество символов которое может поместиться на экране
};