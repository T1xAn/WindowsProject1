#pragma once
#include "framework.h"
#include "Resource.h"

	class cWindowInfo {
	public:

		cWindowInfo() {
			m_Window = NULL;
			m_File = NULL;
			m_FileSize.QuadPart = 0;
			m_ChildWindows;
		}
	
		~cWindowInfo() {
		}

		BOOL SetWindowHandle(_In_ HWND Window) {

			m_Window = Window;
			return TRUE;
		}

		BOOL SetWindowFileHandle(_In_ HANDLE newFile) {
			
			m_File = newFile;
	/*		if (m_File == )
				return FALSE;*/
			return TRUE;
		}

		BOOL SetWindowFileSize() {
				m_FileSize.QuadPart = 0;
				GetFileSizeEx(m_File, &m_FileSize);

				if (m_FileSize.QuadPart == 0)
					return false;

				return true;
		}

		BOOL AddChildWindows(HWND Window ,char* WindowName) {

			m_ChildWindows.push_back({WindowName, Window});
			return TRUE;
		}

		std::vector <std::pair<char*, HWND>>  ReturnChildWindows() {
			return m_ChildWindows;
		}

		HWND ReturnWindowHWND() {
			return m_Window;
		}

		HANDLE ReturnFileHANDLE() {
			return m_File;
		}

		LARGE_INTEGER ReturnFileSize() {
			return m_FileSize;
		}

	private:
		HWND m_Window;
		HANDLE m_File;
		LARGE_INTEGER m_FileSize;
		std::vector <std::pair<char*, HWND>> m_ChildWindows;
	};