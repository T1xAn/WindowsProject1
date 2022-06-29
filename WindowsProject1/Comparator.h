#pragma once
#include "framework.h"
#include "Resource.h"

class COMPARATOR {
public:
	COMPARATOR() {
		m_UpdatingWindows;
	}

	~COMPARATOR() {

	}

	int FindWindowWithKey(char* WindowKey) {
		int key = 0;
		for (key; key < m_UpdatingWindowsKeys.size(); key++) {
			if (WindowKey == m_UpdatingWindowsKeys[key])
				return key;
		}

		if (key == m_UpdatingWindowsKeys.size())
			return -1;
	}

	BOOL AddUpdatingWindows(HWND Window, char* WindowKey) {
		m_UpdatingWindowsKeys.push_back(WindowKey);
		m_UpdatingWindows.push_back(Window);
		return TRUE;
	}

	BOOL UpdateKeyWindow(char* WindowKey) {
		int key = FindWindowWithKey(WindowKey);
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
			int key = FindWindowWithKey(WindowKey);
			if (key == -1)
				return FALSE;

			EnableScrollBar(m_UpdatingWindows[key], SB_VERT, EnableMod);
			return TRUE;
		}
		
	}

	/*BOOL AddNewOpendFile(char* WindowKey, HANDLE File) {

		int key = FindWindowWithKey(WindowKey);
		if (key == -1)
			return FALSE;

		m_OpendFiles.push_back({ key,File });

		cWindowInfo* Window = (cWindowInfo*)GetWindowLongPtr(m_UpdatingWindows[key], GWLP_USERDATA);
		Window->SetWindowFileHandle(File);
		return TRUE;

	}*/

	HANDLE ReturnFileHandle(HWND WindowKey) {

	}

	BOOL DeleteOpenFileHandle(char* WindowKey) {

		int key = FindWindowWithKey(WindowKey);
		if (key == -1)
			return FALSE;
		for (int i = 0; i < m_OpendFiles.size(); i++) {
			if (m_OpendFiles[i].first == key) {
				std::vector < std::pair <int, HANDLE>>::iterator iterator = m_OpendFiles.begin();
				m_OpendFiles.erase(iterator + i);
				cWindowInfo* Window = (cWindowInfo*)GetWindowLongPtr(m_UpdatingWindows[key], GWLP_USERDATA);
				Window->CloseFileHandle();
				return TRUE;
			}
		}
		return FALSE;
	}
	

/////////////////////////////////////////////////////////////////////////
	HANDLE m_RightFile, m_LeftFile;

	BOOL AddNewOpendFile(char* WindowKey, HANDLE File) {

		int key = FindWindowWithKey(WindowKey);
		if (key == -1)
			return FALSE;

		if (m_UpdatingWindowsKeys[key] == (char*)"LeftWindow")
			m_LeftFile = File;
		else
			m_RightFile = File;

		cWindowInfo* Window = (cWindowInfo*)GetWindowLongPtr(m_UpdatingWindows[key], GWLP_USERDATA);
		Window->SetWindowFileHandle(File);
		return TRUE;

	}

	void CloseFileR() {
		CloseHandle(m_RightFile);
		//for (int i = 0; i < m_UpdatingWindowsKeys.size(); i++) {
		//	if (m_UpdatingWindowsKeys[i] == (char*)"RightWindow") {
		//		cWindowInfo* Window = (cWindowInfo*)GetWindowLongPtr(m_UpdatingWindows[i], GWLP_USERDATA);
		//		Window->CloseFileHandle();
		//	}
		//}
	}

	void CloseFileL() {
		CloseHandle(m_LeftFile);
		/*for (int i = 0; i < m_UpdatingWindowsKeys.size(); i++) {
			if (m_UpdatingWindowsKeys[i] == (char*)"LeftWindow") {
				cWindowInfo* Window = (cWindowInfo*)GetWindowLongPtr(m_UpdatingWindows[i], GWLP_USERDATA);
				Window->CloseFileHandle();
			}
		}*/
	}

/////////////////////////////////////////////////////////////////////////////////
private:
	std::vector <char*> m_UpdatingWindowsKeys;
	std::vector <HWND> m_UpdatingWindows;
	std::vector < std::pair <int, HANDLE>> m_OpendFiles;
};