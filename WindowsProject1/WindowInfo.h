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

		BOOL SetWindowKey(char* key) {
			m_key = key;
			return TRUE;
		}

		BOOL SetWindowFileHandle(_In_ HANDLE newFileMap, _In_ HANDLE File) {
			
			m_File = newFileMap;
			SetWindowFileSize(File);
			return TRUE;
		}

		BOOL SetWindowFileSize(HANDLE File) {
			m_FileSize.QuadPart = 0;
			GetFileSizeEx(File, &m_FileSize);

			DWORD error = GetLastError();

			if (m_FileSize.QuadPart == 0)
				return false;

			return true;
		}

		BOOL CloseFileHandle() {
			m_File = NULL;
			return TRUE;
		}



		BOOL AddChildWindows(HWND Window ,char* WindowName) {

			m_ChildWindows.push_back({WindowName, Window});
			return TRUE;
		}

		/////////////////////////////////////////////////////////////////////
		std::vector <std::pair<char*, HWND>>  ReturnChildWindows() {
			return m_ChildWindows;
		}
		/////////////////////////////////////////////////////////////////////

		HWND FindChildWindow(char* WindowName) {
			for (int i = 0; i < m_ChildWindows.size(); i++) {
				if (m_ChildWindows[i].first == WindowName)
					return m_ChildWindows[i].second;
			}
			return NULL;
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
		char* m_key;
	};