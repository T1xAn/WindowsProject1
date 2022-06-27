#pragma once
#include "framework.h"
#include "Resource.h"

class COMPARATOR {
public:
	COMPARATOR() {
		m_UpdatingWindows;
	}

	BOOL AddUpdatingWindows(HWND Window) {

		m_UpdatingWindows.push_back(Window);
		return TRUE;
	}

	std::vector <HWND> ReturnUpdatingWindows() {
		return m_UpdatingWindows;
	}

private:
	std::vector <HWND> m_UpdatingWindows;
};