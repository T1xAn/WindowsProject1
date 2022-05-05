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

// ������� ���������� ������� ���������� ������
	TEXTMETRIC ReturnTextMetric() 
	{
		return m_TextMetric;
	}

// ������� ���������� ������������� �������
	DWORD ReturnGranularity() 
	{
		return m_Granularity;
	}

// ������� ���������� ������������ ���������� �����, ������� ����� ����������� �� ������
	LONG ReturnStringsOnScreen() 
	{
		return m_Strings_on_screen;
	}

// ������� ���������� ������������ ����������� ��������, ������� ����� ����������� �� ������
	LONG ReturnCharsOnScreen() {
		return m_CharOnScreen;
	}

// ������� ���������� ������ �����, �������� � ����� ����
	LARGE_INTEGER ReturnLeftFileSize() 
	{
		return m_LeftFileSize;
	}

	DWORDLONG  m_ScrollVerticalOffset;
	LONG m_ScrollHorizontalOffset;

private:
	DWORD m_Granularity; // ������������� �������
	LARGE_INTEGER m_LeftFileSize; //������ ����� ��������� � ����� ����
	LARGE_INTEGER m_RightFileSize; //������ ����� ��������� � ������ ����
	TEXTMETRIC m_TextMetric; // ��������� ���������� ������
	LONG m_Strings_on_screen; // ����� ����������� ����� ������� ����� ����������� �� ������
	LONG m_CharOnScreen; // ����� ����������� �������� ������� ����� ����������� �� ������
};