#pragma once
#include "framework.h"
#include "Resource.h"
//extern  ScrollFileInfo ScrolledFilesInfo;

class COMPARATOR {
public:
	COMPARATOR() {
		m_UpdatingWindows;
	}

	~COMPARATOR() {

	}

	HWND ReturnUpdatingWindow(int key) {
		return m_UpdatingWindows[key];
	}

	int FindWindowsWithHWND(HWND Window, BOOL FindTextBoxAndButtons) {
		int key = 0;
		std::vector <HWND> Windows = m_UpdatingWindows;
		if (FindTextBoxAndButtons)
			Windows = m_ButtonsAndTextBox;
		for (key; key < Windows.size(); key++) {
			if (Window == Windows[key])
				return key;
		}

		if (key == Windows.size())
			return -1;
	}

	int FindWindowWithKey(char* WindowKey, BOOL FindTextBoxAndButtons) {
		int key = 0;
		std::vector <char*> Windows = m_UpdatingWindowsKeys;
		if (FindTextBoxAndButtons)
			Windows = m_ButtonsAndTextBoxKeys;
		for (key; key < Windows.size(); key++) {
			if (WindowKey == Windows[key])
				return key;
		}

		if (key == Windows.size())
			return -1;
	}

	BOOL AddWindows(HWND Window) {
		m_allWindows.push_back(Window);
		return TRUE;
	}

	BOOL VerifyAndShowAllWindows(int nCmdShow) {
		
		for (int i = 0; i < m_allWindows.size(); i++) {
			if (!m_allWindows[i])
				return FALSE;
			ShowWindow(m_allWindows[i], nCmdShow);
			UpdateWindow(m_allWindows[i]);
		}
		return TRUE;
	}

	BOOL GenerateKey(char* key, BOOL WindowsType) {
		BOOL flag = FALSE;
		 char alphanum[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";

		while (1) {

			for (int i = 0; i < 10; i++) {
				key[i] = alphanum[rand() % strlen((char*)alphanum)];
			}

			if (FindWindowWithKey(key, WindowsType) == -1)
				return TRUE;
		}
	}

	BOOL AddUpdatingWindows(HWND Window, char* WindowKey) {
		m_UpdatingWindowsKeys.push_back(WindowKey);
		m_UpdatingWindows.push_back(Window);
		return TRUE;
	}

	BOOL UpdateKeyWindow(char* WindowKey) {
		int key = FindWindowWithKey(WindowKey, FALSE);
		if (key == -1)
			return FALSE;

		InvalidateRect(m_UpdatingWindows[key], NULL, TRUE);
		UpdateWindow(m_UpdatingWindows[key]);
		
		return TRUE;
	}

	std::vector <HWND> ReturnUpdatingWindows() {
		return m_UpdatingWindows;
	}

	BOOL SendVerticalScrollMessage(HWND CurrentWindow) {
		for (int i = 0; i < m_UpdatingWindows.size(); i++) {
			if(m_UpdatingWindows[i] != CurrentWindow)
			SendNotifyMessage(m_UpdatingWindows[i], WM_VSCROLL, -1L, -1L);
		}

		ComparePages();
		return  TRUE;
	}

	BOOL SendHorizontalScrollMessage(HWND CurrentWindow) {
		for (int i = 0; i < m_UpdatingWindows.size(); i++) {
			if (m_UpdatingWindows[i] != CurrentWindow)
				SendNotifyMessage(m_UpdatingWindows[i], WM_HSCROLL, -1L, -1L);
		}
		return  TRUE;
	}

	BOOL HideScrollBars() {
		for(int i = 0; i < m_UpdatingWindows.size(); i++ ){
		SendNotifyMessage(m_UpdatingWindows[i], WM_VSCROLL, LOWORD(-1), NULL);
		ShowScrollBar(m_UpdatingWindows[i], SB_HORZ, FALSE);
		}
		return TRUE;
	}

	void SetNewHorizontalScrollRange(int NewRange) {
		for (int i = 0; i < m_UpdatingWindows.size(); i++) {
			SetScrollRange(m_UpdatingWindows[i], SB_HORZ, 0, NewRange, TRUE);
			ShowScrollBar(m_UpdatingWindows[i], SB_HORZ, TRUE);
		}
	}

	void SetNewVerticalScrollRange(int newRange) {
		for (int i = 0; i < m_UpdatingWindows.size(); i++)
			SetScrollRange(m_UpdatingWindows[i], SB_VERT, 0, newRange, TRUE);
	}

	void ResetHorizontalScroll() {
		for (int i = 0; i < m_UpdatingWindows.size(); i++) {
			SetScrollPos(m_UpdatingWindows[i], SB_HORZ, 0, TRUE);
		}
	}

	BOOL DrawNewFiles() {
		for (int i = 0; i < m_UpdatingWindows.size(); i++) {
			SetScrollPos(m_UpdatingWindows[i], SB_VERT, 0, TRUE);
			SetScrollPos(m_UpdatingWindows[i], SB_HORZ, 0, TRUE);
			SendMessage(m_UpdatingWindows[i], WM_PAINT, 0, 0);
		}
		return TRUE;
	}

	BOOL EnableWindowScrollBar(char* WindowKey, BOOL EnableAllWindowsScrollBar, BOOL EnableScrollBars) {
		int EnableMod = ESB_ENABLE_BOTH;
		if (!EnableScrollBars)
			EnableMod = ESB_DISABLE_BOTH;

		if (EnableAllWindowsScrollBar == FALSE && WindowKey == NULL)
			return FALSE;
		else if (EnableAllWindowsScrollBar == TRUE) {
			for (int i = 0; i < m_UpdatingWindows.size(); i++) {
				EnableScrollBar(m_UpdatingWindows[i], SB_VERT, EnableMod);
			}
			return TRUE;
		}
		else if (WindowKey != NULL) {
			int key = FindWindowWithKey(WindowKey, FALSE);
			if (key == -1)
				return FALSE;

			EnableScrollBar(m_UpdatingWindows[key], SB_VERT, EnableMod);
			return TRUE;
		}
		
	}

	void SendWMPaintMessage(HWND Window) {
		for (int key = 0; key < m_UpdatingWindows.size(); key++) {
			if (m_UpdatingWindows[key] == Window)
				SendNotifyMessage(m_UpdatingWindows[key], WM_PAINT, 0, 0);
		}
	}

	cWindowInfo* AddNewOpendFile(char* WindowKey, HANDLE FileMap, HANDLE File) {

		int key = FindWindowWithKey(WindowKey,FALSE);
		if (key == -1)
			return FALSE;

		m_OpendFiles.push_back({ key,{FileMap, File }});

		cWindowInfo* Window = (cWindowInfo*)GetWindowLongPtr(m_UpdatingWindows[key], GWLP_USERDATA);
		Window->SetWindowFileHandle(FileMap, File);
		return Window;

	}

	std::vector <std::pair <int, std::pair<HANDLE, HANDLE>>> ReturnOpenFileHandles() {
		return m_OpendFiles;
	}

	BOOL DeleteOpenFileHandle(char* WindowKey) {

		int key = FindWindowWithKey(WindowKey,FALSE);
		if (key == -1)
			return FALSE;

		for (int i = 0; i < m_OpendFiles.size(); i++) {
			if (m_OpendFiles[i].first == key) {
				std::vector < std::pair <int, std::pair<HANDLE, HANDLE>>>::iterator iterator = m_OpendFiles.begin();
				CloseHandle(m_OpendFiles[i].second.first);
				CloseHandle(m_OpendFiles[i].second.second);
				m_OpendFiles.erase(iterator + i);
				cWindowInfo* Window = (cWindowInfo*)GetWindowLongPtr(m_UpdatingWindows[key], GWLP_USERDATA);
				Window->CloseFileHandle();
				return TRUE;
			}
		}
		return FALSE;
	}
	
	cWindowInfo* FindOtherComparableFile(HWND CurrentWindow, int TryNumber) {
		
			if (CurrentWindow != m_UpdatingWindows[TryNumber])
				return (cWindowInfo*)GetWindowLongPtr(m_UpdatingWindows[TryNumber], GWLP_USERDATA);

		return NULL;
	}

	BOOL CloseMainWindowsOpenFiles() {

		if (m_OpendFiles.size() == 0)
			return FALSE;
		int key = 0;
		while(1) {
			
				if (m_OpendFiles[key].second.first != NULL) {
					CloseHandle(m_OpendFiles[key].second.first);
					cWindowInfo* Window = (cWindowInfo*)GetWindowLongPtr(m_UpdatingWindows[m_OpendFiles[key].first], GWLP_USERDATA);
					Window->CloseFileHandle();
				}

				if (m_OpendFiles[key].second.second != NULL)
					CloseHandle(m_OpendFiles[key].second.second);


				std::vector < std::pair <int, std::pair<HANDLE, HANDLE>>>::iterator iterator = m_OpendFiles.begin() + key;
				m_OpendFiles.erase(iterator);

				if (m_OpendFiles.size() == 0) return TRUE;
				//key++;
		}

		return TRUE;
	}

	BOOL CheckUpdatingWindows() {
		if (m_UpdatingWindows.size() == 0) return FALSE;
			return TRUE;
	}

	BOOL AddButtonsAndTextBox(char* Key, HWND Window) {
		m_ButtonsAndTextBox.push_back( Window );
		m_ButtonsAndTextBoxKeys.push_back(Key);
		return TRUE;
	}
	
	std::vector <char*> ReturnButtonTextBoxkeys() {
		return m_ButtonsAndTextBoxKeys;
	}

	std::vector <char*> ReturnUpdatingWindowsKeys() {
		return m_UpdatingWindowsKeys;
	}

	BOOL AddPage(char* page, DWORD PageNumChar,HWND Window){
		 cWindowInfo* WindowInfo = (cWindowInfo*)GetWindowLongPtr(Window, GWLP_USERDATA);
		 WindowInfo->AddPage(page, PageNumChar);
		Pages.push_back({Window,{PageNumChar, page}});
		return TRUE;
	}

	BOOL ClearPages(HANDLE LocalHeap) {
		for (int i = 0; i < Pages.size(); i++) {
			HeapFree(LocalHeap, NULL, Pages[i].second.second);
			cWindowInfo* WindowInfo = (cWindowInfo*)GetWindowLongPtr(Pages[i].first, GWLP_USERDATA);
			WindowInfo->ClearPage();
		}
		Pages.clear();
		return TRUE;
	}

	BOOL ComparePages() {
		Differences.clear();
		if (Pages.size() == 0)
			return FALSE;
		for (int count = 0; count < Pages.size()-1; count++) {
			char* FirstPage = Pages[count].second.second;
			char* SecondPage = Pages[count + 1].second.second;
			int CharCount = min(Pages[count].second.first, Pages[count+1].second.first);
			for (int i = 0; i < CharCount; i++) {
				if (FirstPage[i] != SecondPage[i] && Differences.end() == std::find(Differences.begin(), Differences.end(), i))
					Differences.push_back(i);
				}
		}
		return TRUE;
	}

	void AddDifferences(LONGLONG num) {
		Differences.push_back(num);
	}

	BOOL FindDifferences(LONGLONG num) {
		if (Differences.end() == std::find(Differences.begin(), Differences.end(), num))
			return TRUE;
		return FALSE;

	}

	BOOL FindPage(HWND Window) {
		for (int i = 0; i < Pages.size(); i++) {
			if (Pages[i].first == Window)
				return TRUE;
		}
		return FALSE;
	}

	std::vector <LONGLONG> ReturnDifferences() {
		return Differences;
	}


private:
	std::vector <char*> m_UpdatingWindowsKeys;
	std::vector <HWND> m_UpdatingWindows;
	std::vector <std::pair <int, std::pair<HANDLE, HANDLE>>> m_OpendFiles;
	std::vector <HWND> m_allWindows;
	std::vector <HWND> m_ButtonsAndTextBox;
	std::vector <char*> m_ButtonsAndTextBoxKeys;
	std::vector <std::pair <HWND,std::pair<DWORD, char*>>> Pages;
	std::vector <LONGLONG> Differences;
};