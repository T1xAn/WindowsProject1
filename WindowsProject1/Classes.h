#pragma once
#include "framework.h"
#include "Resource.h"

#define MAX_LOADSTRING 100

class ScrollFileInfo
{
public:

	ScrollFileInfo() {
		m_ScrollVerticalOffset = 0;
		m_Granularity = 0;
		m_CharOnScreen = 0;
		m_ScrollHorizontalOffset = 0;
		m_BytesOnString = 8;
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

	BOOL GetLeftFileSize(_In_ HANDLE LeftFile)
	{
		m_LeftFileSize.QuadPart = 0;
		GetFileSizeEx(LeftFile, &m_LeftFileSize);

		if (m_LeftFileSize.QuadPart == 0) 
			return false;

		return true;
	}

	BOOL GetRightFileSize(_In_ HANDLE RightFile)
	{
		m_RightFileSize.QuadPart = 0;
		GetFileSizeEx(RightFile, &m_RightFileSize);

		if (m_RightFileSize.QuadPart == 0)
			return false;

		return true;
	}
	
	LONGLONG ReturnBiggestFile() {
		return (max(m_LeftFileSize.QuadPart, m_RightFileSize.QuadPart));
	}

	LONGLONG ReturnSmallestFile() {
		return (min(m_LeftFileSize.QuadPart, m_RightFileSize.QuadPart));
	}

	BOOL GetTextMetric(_In_ HWND Window, _In_ HFONT FONT) {
		HDC hdc = GetDC(Window);
		SelectObject(hdc, FONT);
		 GetTextMetrics(hdc, &m_TextMetric);
		 ReleaseDC(Window, hdc);
		 return true;
	}

	// ������ ����������� ���������� ���������� ����� �� ������ � �������� � ������
	BOOL GetNumStringsAndCharOnScreen(_In_ HWND Window) {
		RECT rt;
		GetClientRect(Window, &rt);

		if  (m_TextMetric.tmHeight == 0) 
			return false;

		m_Strings_on_screen = (rt.bottom / m_TextMetric.tmHeight);
		m_CharOnScreen = (rt.right/m_TextMetric.tmAveCharWidth);

		return true;
	}

	// ������� ������������� ��� �������������� ������ ���������
	LONG HorizontalOffset() {
		char BufferString[100];
		if (ReturnBiggestFile() != 0 ) {
			int StrNum = snprintf(BufferString, sizeof(BufferString), "%llX", ReturnBiggestFile());
			int OutString = StrNum + 2 + 3 * m_BytesOnString + 2 + 2 * m_BytesOnString;
			int ReturnScroll = 0;
			if (OutString > m_CharOnScreen) {
				if ((ceil(OutString - m_CharOnScreen)) < (StrNum + 2)) 
					ReturnScroll = OutString - m_CharOnScreen+2;
				else 
					ReturnScroll = StrNum + 2 + (ceil((OutString - m_CharOnScreen - StrNum + 2) / 3));
				return ReturnScroll;
			}
			else
				return 0;
		}
		return 0;
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
// ������� ���������� ������ �����, ��������� � ������ ����
	LARGE_INTEGER ReturnRightFileSize() {
		return m_RightFileSize;
	}
	
	LONGLONG  m_ScrollVerticalOffset;
	LONG m_ScrollHorizontalOffset;
	LONG m_BytesOnString;

private:
	DWORD m_Granularity; // ������������� �������
	LARGE_INTEGER m_LeftFileSize; //������ ����� ��������� � ����� ����
	LARGE_INTEGER m_RightFileSize; //������ ����� ��������� � ������ ����
	TEXTMETRIC m_TextMetric; // ��������� ���������� ������
	LONG m_Strings_on_screen; // ����� ����������� ����� ������� ����� ����������� �� ������
	LONG m_CharOnScreen; // ����� ����������� �������� ������� ����� ����������� �� ������
};

LRESULT CALLBACK    LeftProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ToolProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK    RightProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class MainWindows {
public:
	MainWindows(){} // �������� �������������

	~MainWindows(){}

	// ������������� ���������� �����
	void RegisterStrings(_In_ HINSTANCE hInstance) {
		LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);

		LoadStringW(hInstance, IDS_LT_TITLE, ltTitle, MAX_LOADSTRING);
		LoadStringW(hInstance, IDC_LeftTextWindow, ltWindowClass, MAX_LOADSTRING);

		LoadStringW(hInstance, IDC_TOOLBARWINDOW, tbWindowClass, MAX_LOADSTRING);

		LoadStringW(hInstance, IDC_RIGHTTEXTWINDOW, rtWindowClass, MAX_LOADSTRING);
	}

	// ����������� ������ �������� ����
	ATOM MainWindowClass(_In_ HINSTANCE hInstance) {
		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = NULL;//CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW - 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		return RegisterClassExW(&wcex);
	}

	// ����������� ������ ��� ������ ���� ������
	ATOM LeftWindowClass(_In_ HINSTANCE hInstance) {
		WNDCLASSEXW wcex1;

		wcex1.cbSize = sizeof(WNDCLASSEX);

		wcex1.style = CS_NOCLOSE;
		wcex1.lpfnWndProc = LeftProc;
		wcex1.cbClsExtra = 0;
		wcex1.cbWndExtra = 0;
		wcex1.hInstance = hInstance;
		wcex1.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
		wcex1.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex1.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex1.lpszMenuName = NULL;//MAKEINTRESOURCEW(IDC_LeftTextWindow);
		wcex1.lpszClassName = ltWindowClass;
		wcex1.hIconSm = LoadIcon(wcex1.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		return RegisterClassExW(&wcex1);
	}

	// ����������� ������ ��� ���� ������������
	ATOM ToolBarWindowClass(_In_ HINSTANCE hInstance) {
		WNDCLASSEXW wcex1;

		wcex1.cbSize = sizeof(WNDCLASSEX);

		wcex1.style = CS_NOCLOSE;
		wcex1.lpfnWndProc = ToolProc;
		wcex1.cbClsExtra = 0;
		wcex1.cbWndExtra = 0;
		wcex1.hInstance = hInstance;
		wcex1.hIcon = NULL;/*LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));*/
		wcex1.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex1.hbrBackground = (HBRUSH)(COLOR_WINDOW - 1);
		wcex1.lpszMenuName = NULL;//MAKEINTRESOURCEW(IDC_LeftTextWindow);
		wcex1.lpszClassName = tbWindowClass;
		wcex1.hIconSm = NULL;/*LoadIcon(wcex1.hInstance, MAKEINTRESOURCE(IDI_SMALL));*/

		return RegisterClassExW(&wcex1);

	}

	// ����������� ������ ������� ���� ������
	ATOM RightWindowClass(_In_ HINSTANCE hInstance) {
		WNDCLASSEXW wcex1;

		wcex1.cbSize = sizeof(WNDCLASSEX);

		wcex1.style = CS_NOCLOSE;
		wcex1.lpfnWndProc = RightProc;
		wcex1.cbClsExtra = 0;
		wcex1.cbWndExtra = 0;
		wcex1.hInstance = hInstance;
		wcex1.hIcon = NULL;
		wcex1.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex1.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex1.lpszMenuName = NULL;//MAKEINTRESOURCEW(IDC_LeftTextWindow);
		wcex1.lpszClassName = rtWindowClass;
		wcex1.hIconSm = NULL;

		 return RegisterClassExW(&wcex1);
		
	}

	// ������� ����������� ������� ���� ���� ����������
	BOOL RegisterWindowClasses(_In_ HINSTANCE hInstance) {
		
		if (MainWindowClass(hInstance) == 0)
			return FALSE;
		if (LeftWindowClass(hInstance) == 0)
			return FALSE;
		if (ToolBarWindowClass(hInstance) == 0)
			return FALSE;
		if (RightWindowClass(hInstance) == 0)
			return FALSE;
		return TRUE;
	}

	BOOL CreateAllWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow ) {
		hInst = hInstance; // ��������� ������ ���������� � ���������� ����������

		HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

		if (!hWnd)
		{
			return FALSE;
		}

		if (!LeftTextWindow)
		{
			return FALSE;
		}

		if (!ToolBar)
		{
			return FALSE;
		}

		if (!RightTextWindow) {
			return FALSE;
		}

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);

		ShowWindow(LeftTextWindow, nCmdShow);
		UpdateWindow(LeftTextWindow);

		ShowWindow(ToolBar, nCmdShow);
		UpdateWindow(ToolBar);

		ShowWindow(RightTextWindow, nCmdShow);
		UpdateWindow(RightTextWindow);

		ShowWindow(List, nCmdShow);
		UpdateWindow(List);
		return TRUE;
	}

	// ������� ���������� ��� ������ ������ ���� ������
	WCHAR* GetLeftWindowClass() {
		return ltWindowClass;
	}
	
	// ������� ���������� ��� ������ ������ ���� ������
	WCHAR* GetLeftWindowTitle() {
		return ltTitle;
	}

	// ������� ���������� ��� ������ ���� ������������
	WCHAR* GetToolBarClass() {
		return tbWindowClass;
	}

	// ������� ���������� ��� ������ ������� ���� ������
	WCHAR* GetRightWindowClass() {
		return rtWindowClass;
	}

	HWND m_UpdatingWindows[2];
	HINSTANCE hInst;
	HWND ChangeBytesNumButton;
	HWND LeftSearch;
	HWND LeftTextbox;
	HWND RightSearch;
	HWND RightTextbox;
	HWND ReadButton;
	HWND ChangeFont;
	HWND List;
	HWND LeftTextWindow;    // ����� �������� ���� ������
	HWND RightTextWindow;	// ������ �������� ���� ������
	HWND ToolBar;			// ���� �����������
	HWND LeftFileSizeBar;
	HWND RightFileSizeBar;

private:

	WCHAR szTitle[MAX_LOADSTRING];                  // ����� ������ ���������
	WCHAR szWindowClass[MAX_LOADSTRING];            // ��� ������ �������� ����
	                        
	WCHAR ltWindowClass[MAX_LOADSTRING];            // ��� ������ ������ ��������� ���� ������
	WCHAR ltTitle[MAX_LOADSTRING];                  // ����� ������ ��������� ������ ��������� ����

	WCHAR tbWindowClass[MAX_LOADSTRING];			// ��� ������ ���� ������������

	WCHAR rtWindowClass[MAX_LOADSTRING];			// ��� ������ ������� ��������� ����
};