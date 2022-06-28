#pragma once
#include "framework.h"
#include "Resource.h"

class COMPARATOR {
public:
	COMPARATOR() {
		m_UpdatingWindows;
	}

	BOOL AddUpdatingWindows(HWND Window, char* WindowKey) {
		m_UpdatingWindowsKeys.push_back(WindowKey);
		m_UpdatingWindows.push_back(Window);
		return TRUE;
	}

	BOOL UpdateKeyWindow(char* WindowKey) {
		int key = 0;
		for (key; key < m_UpdatingWindowsKeys.size(); key++) {
			if (WindowKey == m_UpdatingWindowsKeys[key])
				break;
		}

		if (key == m_UpdatingWindowsKeys.size())
			return FALSE;

		InvalidateRect(m_UpdatingWindows[key], NULL, TRUE);
		UpdateWindow(m_UpdatingWindows[key]);
		
		return TRUE;
	}

	std::vector <HWND> ReturnUpdatingWindows() {
		return m_UpdatingWindows;
	}

	BOOL SendVerticalScrollMessage() {
		for (int i = 0; i < m_UpdatingWindows.size(); i++) {
			SendNotifyMessage(m_UpdatingWindows[i], WM_VSCROLL, LOWORD(-1), NULL);
		}
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

	BOOL EnableScrollBar() {


	}

private:
	std::vector <char*> m_UpdatingWindowsKeys;
	std::vector <HWND> m_UpdatingWindows;
	HWND m_LeftWindow;
	HWND m_RightWindow;
};