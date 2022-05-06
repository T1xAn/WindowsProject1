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

LRESULT CALLBACK    LeftProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ToolBarProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ToolProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class MainWindows {
public:
	MainWindows(){} // �������� �������������

	~MainWindows(){}

	// ������������� ���������� �����
	void RegisterStrings(HINSTANCE hInstance) {
		LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);

		LoadStringW(hInstance, IDS_LT_TITLE, ltTitle, MAX_LOADSTRING);
		LoadStringW(hInstance, IDC_LeftTextWindow, ltWindowClass, MAX_LOADSTRING);

		LoadStringW(hInstance, IDC_TOOLBARWINDOW, tbWindowClass, MAX_LOADSTRING);
	}

	// ����������� ������ �������� ����
	ATOM MainWindowClass(HINSTANCE hInstance) {
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
	ATOM LeftWindowClass(HINSTANCE hInstance) {
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

	ATOM ToolBarWindowClass(HINSTANCE hInstance) {
		WNDCLASSEXW wcex1;

		wcex1.cbSize = sizeof(WNDCLASSEX);

		wcex1.style = CS_NOCLOSE;
		wcex1.lpfnWndProc = ToolProc;
		wcex1.cbClsExtra = 0;
		wcex1.cbWndExtra = 0;
		wcex1.hInstance = hInstance;
		wcex1.hIcon = NULL;/*LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));*/
		wcex1.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex1.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex1.lpszMenuName = NULL;//MAKEINTRESOURCEW(IDC_LeftTextWindow);
		wcex1.lpszClassName = tbWindowClass;
		wcex1.hIconSm = NULL;/*LoadIcon(wcex1.hInstance, MAKEINTRESOURCE(IDI_SMALL));*/

		return RegisterClassExW(&wcex1);

	}
	// ������� ����������� ������� ���� ���� ����������
	BOOL RegisterWindiwClasses(HINSTANCE hInstance) {
		
		if (MainWindowClass(hInstance) == 0)
			return FALSE;
		if (LeftWindowClass(hInstance) == 0)
			return FALSE;
		return TRUE;
	}

	BOOL CreateAllWindow(HINSTANCE hInstance, int nCmdShow ) {
		hInst = hInstance; // ��������� ������ ���������� � ���������� ����������

		HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
		search = CreateWindowA("button", ">>", WS_CHILD |
			WS_VISIBLE | WS_BORDER, 700, 20, 30, 30, hWnd, (HMENU)IDB_SearchButton_Left, hInstance, nullptr);
		textbox = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT("D:\\XP\\TurboXP\\TurboXP.vdi"),
			WS_CHILD | WS_VISIBLE, 30, 20, 650,
			30, hWnd, NULL, NULL, NULL);
		ReadButton = CreateWindowA("button", "o!", WS_CHILD |
			WS_VISIBLE | WS_BORDER, 750, 20, 30, 30, hWnd, (HMENU)IDB_ReadButton, hInstance, nullptr);
		ChangeBytesNumButton = CreateWindowA("button", "o?", WS_CHILD |
			WS_VISIBLE | WS_BORDER, 800, 20, 30, 30, hWnd, (HMENU)IDB_ChangeBytesNumButton, hInstance, nullptr);

		//int scroll = GetSystemMetrics(SM_CXVSCROLL); // 17
		if (!hWnd)
		{
			return FALSE;
		}

		if (!LeftTextWindow)
		{
			return FALSE;
		}

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);
		ShowWindow(LeftTextWindow, nCmdShow);
		UpdateWindow(LeftTextWindow);
		return TRUE;
	}

	// ������� ���������� ��� ������ ������ ���� ������
	WCHAR* GetLeftWindowClass() {
		return ltWindowClass;
	}
	
	WCHAR* GetLeftWindowTitle() {
		return ltTitle;
	}

	// ������� ���������� ��� ������ ���� ������������
	WCHAR* GetToolBarClass() {
		return tbWindowClass;
	}

	HINSTANCE hInst;
	HWND TopToolBar;
	HWND ChangeBytesNumButton;
	HWND search;
	HWND textbox;
	HWND ReadButton;
	HWND LeftTextWindow;    // ����� �������� ���� ������
	HWND ToolBar;			// ���� �����������

private:

	WCHAR szTitle[MAX_LOADSTRING];                  // ����� ������ ���������
	WCHAR szWindowClass[MAX_LOADSTRING];            // ��� ������ �������� ����
	                        
	WCHAR ltWindowClass[MAX_LOADSTRING];            // ��� ������ ������ ��������� ���� ������
	WCHAR ltTitle[MAX_LOADSTRING];                  // ����� ������ ��������� ������ ��������� ����

	WCHAR tbWindowClass[MAX_LOADSTRING];			// ��� ������ ���� ������������
};