#pragma once
#include "framework.h"
#include "Resource.h"

extern COMPARATOR Comparator;
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
		m_TextMetric;
	}

	~ScrollFileInfo() {};

	// Функция получает информацию о гранулярности системы и сохраняет её в переменной класса
	BOOL GetSystemGranularity()
	{
		SYSTEM_INFO SYSINF;
		GetSystemInfo(&SYSINF);
		m_Granularity = SYSINF.dwAllocationGranularity;
		if (m_Granularity == 0) 
			return false;
		return true;
	}

	// Функция возвращает размер наибольшего, запущенного в данный момент файла
	LONGLONG ReturnBiggestFile() {
		std::vector <std::pair <int, std::pair<HANDLE, HANDLE>>> OpenFiles = Comparator.ReturnOpenFileHandles();
		if (OpenFiles.size() == 0) return 0;
		if (OpenFiles.size() == 1) {
			LARGE_INTEGER File;
			GetFileSizeEx(OpenFiles[0].second.second, &File);
			return File.QuadPart;
		}
		LONGLONG MaxSize = LLONG_MIN;
		for (int i = 0; i < OpenFiles.size() - 1; i++) {
			LARGE_INTEGER FirstFile, SecondFile;
			GetFileSizeEx(OpenFiles[i].second.second, &FirstFile);
			GetFileSizeEx(OpenFiles[i+1].second.second, &SecondFile);
			LONGLONG NewMax = max(FirstFile.QuadPart, SecondFile.QuadPart);
			if (NewMax > MaxSize) MaxSize = NewMax;

		}
		return MaxSize;
	}

	// Функция возвращает размер наименьшего, запущенного в данный момент файла
	LONGLONG ReturnSmallestFile() {
		std::vector <std::pair <int, std::pair<HANDLE, HANDLE>>> OpenFiles = Comparator.ReturnOpenFileHandles();
		LONGLONG MinSize = LLONG_MAX;
		if (OpenFiles.size() == 0) return 0;
		if (OpenFiles.size() == 1) {
			LARGE_INTEGER File;
			GetFileSizeEx(OpenFiles[0].second.second, &File);
			return File.QuadPart;
		}
		for (int i = 0; i < OpenFiles.size() - 1; i++) {
			LARGE_INTEGER FirstFile, SecondFile;
			GetFileSizeEx(OpenFiles[i].second.second, &FirstFile);
			GetFileSizeEx(OpenFiles[i + 1].second.second, &SecondFile);
			LONGLONG NewMin = min(FirstFile.QuadPart, SecondFile.QuadPart);
			if (NewMin < MinSize) MinSize = NewMin;

		}
		return MinSize;
	}

	// Функция обновляет параметры текущего шрифта и сохраняет их в переменной класса
	BOOL GetTextMetric(_In_ HWND Window, _In_ HFONT FONT) {
		HDC hdc = GetDC(Window);
		SelectObject(hdc, FONT);
		 GetTextMetrics(hdc, &m_TextMetric);
		 ReleaseDC(Window, hdc);
		 return true;
	}

	// Расчёт максимально возможного количества строк на экране и символов в строке
	BOOL GetNumStringsAndCharOnScreen(_In_ HWND Window) {
		RECT rt;
		GetClientRect(Window, &rt);

		if  (m_TextMetric.tmHeight == 0) 
			return false;

		m_Strings_on_screen = (rt.bottom / m_TextMetric.tmHeight);
		m_CharOnScreen = (rt.right/m_TextMetric.tmAveCharWidth);

		return true;
	}

	// Подсчёт ограничителей для горизонтальной полосы прокрутки
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

	
	LONGLONG  m_ScrollVerticalOffset; // Текущий оффсет вертикальной полосы прокрутки
	LONG m_ScrollHorizontalOffset;	  // Текущий оффсет горизонтальной полосы прокрутки
	LONG m_BytesOnString;			  // Текущее колличество байт, отображаемых в одной строке

private:
	DWORD m_Granularity; // Гранулярность системы
	TEXTMETRIC m_TextMetric; // Параметры выбранного шрифта
	LONG m_Strings_on_screen; // Целое колличество строк которое может поместиться на экране
	LONG m_CharOnScreen; // Целое колличество символов которое может поместиться на экране
};

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ToolProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK	OutWindowsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

class MainWindows {
public:
	MainWindows(){} // ДОБАВИТЬ ИНИЦИАЛИЗАЦИЮ

	~MainWindows(){}

	// Инициализация глобальных строк
	void RegisterStrings(_In_ HINSTANCE hInstance) {

		LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);

		LoadStringW(hInstance, IDC_OutTextWindow, ltWindowClass, MAX_LOADSTRING);

		LoadStringW(hInstance, IDC_TOOLBARWINDOW, tbWindowClass, MAX_LOADSTRING);

	}

	// Регестрация класса главного окна
	ATOM MainWindowClass(_In_ HINSTANCE hInstance) {
		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = NULL;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		//SetWindowLongPtr(hwnd,)
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

	// Регистрация класса для окон вывода
	ATOM OutWindowClass(_In_ HINSTANCE hInstance) {
		WNDCLASSEXW wcex1;

		wcex1.cbSize = sizeof(WNDCLASSEX);

		wcex1.style = CS_NOCLOSE;
		wcex1.lpfnWndProc = OutWindowsProc;
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

	// Регистрация класса для окна инструментов
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

	// Функция регистрации классов всех окон приложения
	BOOL RegisterWindowClasses(_In_ HINSTANCE hInstance) {
		
		if (MainWindowClass(hInstance) == 0)
			return FALSE;
		if (OutWindowClass(hInstance) == 0)
			return FALSE;
		if (ToolBarWindowClass(hInstance) == 0)
			return FALSE;
		return TRUE;
	}

	// Функция запускает процесс инициализации всех окон в программе
	BOOL CreateAllWindow(_In_ HINSTANCE hInstance, _In_ int nCmdShow ) {
		hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

		HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_MAXIMIZE | WS_CLIPCHILDREN,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

		return Comparator.VerifyAndShowAllWindows(nCmdShow);
	}

	// Функция возвращает имю класса окон вывода
	WCHAR* GetOutWindowClass() {
		return ltWindowClass;
	}
	

	// Функция возвращает имя класса окна инструментов
	WCHAR* GetToolBarClass() {
		return tbWindowClass;
	}

	HINSTANCE hInst;

private:

	WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
	WCHAR szWindowClass[MAX_LOADSTRING];            // Имя класса главного окна
	                        
	WCHAR ltWindowClass[MAX_LOADSTRING];            // Имя класса левого дочернего окна вывода

	WCHAR tbWindowClass[MAX_LOADSTRING];			// Имя класса окна инструментов

};