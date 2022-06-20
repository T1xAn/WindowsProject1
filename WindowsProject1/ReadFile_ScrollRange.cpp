#include "framework.h"
#include "Resource.h"
#include "Functions_elements.h"

extern ScrollFileInfo ScrolledFilesInfo;
extern MainWindows WindowInfo;

// Функция buttonGetFile(OPENFILENAME F) 
//  Позволяет произвести поиск нужного файла через провдник
//  return Возвращает структура OPENFILENAMEW в которой находится информация о рассположении файла на компьютере
OPENFILENAMEW buttonGetFile() {
    OPENFILENAMEW F;
    wchar_t szFile[1000];
    ZeroMemory(&F, sizeof(F));
    F.lStructSize = sizeof(OPENFILENAME);
    F.hwndOwner = NULL;
    F.lpstrFile = (LPWSTR)szFile;
    F.lpstrFile[0] = '\0';
    F.nMaxFile = sizeof(szFile);
    F.lpstrFilter = L"All Files\0*.*\0\0";
    F.nFilterIndex = 1;
    F.lpstrFileTitle = NULL;
    F.nMaxFileTitle = 0;
    F.lpstrInitialDir = NULL;
    F.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    GetOpenFileName(&F);

    return F;
}

// Функция SetVerticalScrollRange()
//  Позволяет произвести расчёт и установить диапазон полосы прокрутки, в зависимости от размера файла
void SetVerticalScrollRange() {
    int BiggestFileSize = ScrolledFilesInfo.ReturnBiggestFile() / (1024 * 1024 * 1024);
    int ScrollRange = 100;

    if (BiggestFileSize >= 1 && BiggestFileSize <= 5)
        ScrollRange = pow(10, 3);
    if (BiggestFileSize >= 5 && BiggestFileSize < 10)
        ScrollRange = pow(10, 4);
    if (BiggestFileSize >= 10 && BiggestFileSize < 20)
        ScrollRange = pow(10, 5);
    if (BiggestFileSize >= 20 && BiggestFileSize < 40)
        ScrollRange = pow(10, 6);
    if (BiggestFileSize >= 40 && BiggestFileSize < 70)
        ScrollRange = pow(10, 7);
    if (BiggestFileSize >= 70 && BiggestFileSize < 110)
        ScrollRange = pow(10, 8);
    if (BiggestFileSize >= 110)
        ScrollRange = pow(10, 9);

    for (int i = 0; i < 2; i++)
        SetScrollRange(WindowInfo.m_UpdatingWindows[i], SB_VERT, 0, ScrollRange, TRUE);

    return;
}