#include "framework.h"
#include "Resource.h"
#include "resource3.h"
BOOL Dlgproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{		
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case DialogButton:
			break;

			case ID_EXIT: {
		EndDialog(hWnd, LOWORD(wParam));
		return TRUE;
		break;
	}
		}
	default:
		break;
	}
}
