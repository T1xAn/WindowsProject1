#include "framework.h"
#include "Resource.h"
#include "Functions_elements.h"

extern ScrollFileInfo ScrolledFilesInfo;
extern MainWindows WindowInfo;

// Функция GetEightBitsHex()
// Производит отрисовку содержания файла с учётов горизонтального и вретикального скролла
// Вывод произвоится в совместимый контекст устройства памяти который впоследстии возвращается в виде переменной 
HDC GetEightBitsHex(_In_ HWND Window, _In_ HANDLE File, _In_ DWORD Granularity, _In_ DWORDLONG FileSize, _In_ DWORDLONG OFFSET,
    _In_ LONG HorizontalOffset, _In_ LONG BytesOnString, _Inout_ HDC BlitHDC) {
    DWORD Block = Granularity, height = 1;
    char BufferString[256] = "";
    OFFSET *= BytesOnString;
    DWORDLONG i = OFFSET;
    int SecondOffset = 0;
    OFFSET = (OFFSET / Granularity);
    int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();
    TEXTMETRIC TextMetric = ScrolledFilesInfo.ReturnTextMetric();
    

    HFONT FONT = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);

    SelectObject(BlitHDC, FONT);

    OFFSET *= Granularity;
    DWORD HighOffset = ((OFFSET >> 32) & 0xFFFFFFFFul);
    DWORD LowOffset = static_cast<DWORD>(OFFSET & 0xFFFFFFFFul);

    if (Block > FileSize - OFFSET) Block = FileSize - OFFSET;
    PBYTE LRFILE = (PBYTE)MapViewOfFile(File, FILE_MAP_READ, HighOffset, LowOffset, Block);

    Block = Strings_On_Screen;

    int StrNum = snprintf(BufferString, sizeof(BufferString), "%llX", ScrolledFilesInfo.ReturnBiggestFile());


    int HexOffset = 0;
    i -= OFFSET;
    for (int count = 0; count < Block; count++)
    {
        int BufferOffset = snprintf(BufferString, sizeof(BufferString), "%0*llX :", StrNum, i + OFFSET);

        if (i + OFFSET >= FileSize) {
            i += BytesOnString;
            TextOutA(BlitHDC, 5, height, (LPCSTR)BufferString, strlen(BufferString));
            height += TextMetric.tmHeight;
            BufferString[0] = '\0';
            continue;
        }

        if (HorizontalOffset != 0) {
            if (HorizontalOffset > BufferOffset) {
                HexOffset = HorizontalOffset - BufferOffset;
                BufferOffset = 0;
            }
            else {
                for (int i = 0; i < BufferOffset - HorizontalOffset; i++)
                    BufferString[i] = BufferString[i + HorizontalOffset];
                BufferOffset = BufferOffset - HorizontalOffset;
            }
        }
        int StrNumOffset = BufferOffset;

        for (int j = HexOffset; j < BytesOnString; j++) {
            BufferOffset += snprintf(BufferString + BufferOffset, sizeof(BufferString) - BufferOffset, " %02X", LRFILE[i + j]);
        }
        BufferOffset += snprintf(BufferString + BufferOffset, sizeof(BufferString) - BufferOffset, " |");

        if (HorizontalOffset > BytesOnString + StrNum + 2) {
            HexOffset = HorizontalOffset - (BytesOnString + StrNum + 2);
            BufferOffset = 0;
        }
        else {
            HexOffset = 0;
            BufferOffset *= TextMetric.tmAveCharWidth;
            TextOutA(BlitHDC, 5, height, (LPCSTR)BufferString, strlen(BufferString));
        }

        int CharOffset = 0;
        for (int j = HexOffset; j < BytesOnString; j++) {
            if (LRFILE[i + j] == '\r' && LRFILE[i + j+1] == '\n') {
                CharOffset += snprintf(BufferString + CharOffset, sizeof(BufferString) - CharOffset, "  ");
                continue;
            }
            CharOffset += snprintf(BufferString + CharOffset, sizeof(BufferString) - CharOffset, " %C", LRFILE[i + j]);
        }
        TextOutA(BlitHDC, 5 + BufferOffset + 5, height, (LPCSTR)BufferString, strlen(BufferString));

        i += BytesOnString;

        if (i == Granularity) {
            UnmapViewOfFile(LRFILE);
            int temp = Block;
            Block = Granularity;
            OFFSET = (((OFFSET / Granularity) + 1) * Granularity);
            DWORD HighOffset = ((OFFSET >> 32) & 0xFFFFFFFFul);
            DWORD LowOffset = static_cast<DWORD>(OFFSET & 0xFFFFFFFFul);
            if (Block > FileSize - OFFSET) Block = FileSize - OFFSET;
            PBYTE LRFILE = (PBYTE)MapViewOfFile(File, FILE_MAP_READ, HighOffset, LowOffset, Block);
            i = 0;
            Block = temp;
        }
        height += TextMetric.tmHeight;
    }

    UnmapViewOfFile(LRFILE);

    return BlitHDC;
}

// Функция CompareTwoFiles()
// Производит сравнение двух файлов, при обнаружении совпадений окрашивает совпадающие фрагменты в красный цвет
// Возвращет изменённый контекст устройства памяти для дальнейшей отрисовки на экран  
HDC CompareTwoFiles(_In_ HANDLE WindowFile, _In_ DWORDLONG WindowFileSize, _In_ HANDLE ComparableFile, _In_ DWORDLONG ComparableFileSize, _Inout_ HDC DrawDC,
    _In_ DWORDLONG Offset, _In_ LONG HorizontalOffset, _In_ LONG BytesOnString, _In_ DWORD Granularity) {

    DWORD Block = Granularity, height = 1;
    char BufferString[50] = "";
    Offset *= BytesOnString;

    DWORDLONG i = Offset;
    LONGLONG SmallestFile = min(WindowFileSize, ComparableFileSize);
    if (SmallestFile <= Offset) return DrawDC;

    Offset = (Offset / Granularity);
    int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();
    TEXTMETRIC TextMetric = ScrolledFilesInfo.ReturnTextMetric();
    int SingleChar = TextMetric.tmAveCharWidth;

    HFONT FONT = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
    SetTextColor(DrawDC, RGB(255, 0, 0));
    Offset *= Granularity;
    DWORD HighOffset = ((Offset >> 32) & 0xFFFFFFFFul);
    DWORD LowOffset = static_cast<DWORD>(Offset & 0xFFFFFFFFul);

    if (Block > WindowFileSize - Offset) Block = WindowFileSize - Offset;
    PBYTE WINDOWRFILE = (PBYTE)MapViewOfFile(WindowFile, FILE_MAP_READ, HighOffset, LowOffset, Block);

    if (Block > ComparableFileSize - Offset) Block = ComparableFileSize - Offset;
    PBYTE COMPARABLERFILE = (PBYTE)MapViewOfFile(ComparableFile, FILE_MAP_READ, HighOffset, LowOffset, Block);

    int HorizontalOffsetMain = HorizontalOffset;

    int StrNumOffset = snprintf(BufferString, sizeof(BufferString), "%llX :", ScrolledFilesInfo.ReturnBiggestFile());
    BufferString[0] = '\0';

    int HexOffset = 0, CharOffset = 0, PreCharOffset = 2 * TextMetric.tmAveCharWidth;
    if (HorizontalOffset > StrNumOffset) {
        HorizontalOffsetMain = HorizontalOffsetMain - StrNumOffset;

        if (HorizontalOffsetMain > BytesOnString) {
            CharOffset = HorizontalOffsetMain - BytesOnString;
            HexOffset = BytesOnString;
            PreCharOffset = 0;
            HorizontalOffsetMain = 0;
        }
        else
            HexOffset = HorizontalOffsetMain;
        HorizontalOffsetMain = 0;
    }

    else HorizontalOffsetMain = StrNumOffset - HorizontalOffset;
    i -= Offset;
    for (int count = 0; count < Strings_On_Screen; count++) {
        for (int HexCount = HexOffset; HexCount < BytesOnString; HexCount++) {
            if (WINDOWRFILE[i + HexCount] != COMPARABLERFILE[i + HexCount]) {
                snprintf(BufferString, sizeof(BufferString), " %02X", WINDOWRFILE[i + HexCount]);
                TextOutA(DrawDC, 5 + 3 * SingleChar * (HexCount - HexOffset) + HorizontalOffsetMain * SingleChar, height, BufferString, strlen(BufferString));
            }
        }
        BufferString[0] = '\0';
        for (int CharCount = CharOffset; CharCount < BytesOnString; CharCount++) {
            if (WINDOWRFILE[i + CharCount] != COMPARABLERFILE[i + CharCount]) {
                if (WINDOWRFILE[i + CharCount] == '\r' && WINDOWRFILE[i + CharCount + 1] == '\n') {
                    continue;
                }
                else {
                    snprintf(BufferString, sizeof(BufferString), " %C", WINDOWRFILE[i + CharCount]);
                    TextOutA(DrawDC, 5 + 5 + 2 * SingleChar * (CharCount - CharOffset) + HorizontalOffsetMain * SingleChar +
                        PreCharOffset + 3 * SingleChar * (BytesOnString - HexOffset), height, BufferString, strlen(BufferString));
                }
            }

        }

        i += BytesOnString;
        if (SmallestFile <= (i + Offset)) return DrawDC;
        height += TextMetric.tmHeight;

        if (i == Granularity) {
            UnmapViewOfFile(WINDOWRFILE);
            UnmapViewOfFile(COMPARABLERFILE);
            int temp = Block;
            Block = Granularity;
            Offset = (((Offset / Granularity) + 1) * Granularity);
            DWORD HighOffset = ((Offset >> 32) & 0xFFFFFFFFul);
            DWORD LowOffset = static_cast<DWORD>(Offset & 0xFFFFFFFFul);
            if (Block > WindowFileSize - Offset) Block = WindowFileSize - Offset;
            PBYTE WINDOWRFILE = (PBYTE)MapViewOfFile(WindowFile, FILE_MAP_READ, HighOffset, LowOffset, Block);

            Block = Granularity;
            if (Block > ComparableFileSize - Offset) Block = ComparableFileSize - Offset;
            PBYTE COMPARABLERFILE = (PBYTE)MapViewOfFile(ComparableFile, FILE_MAP_READ, HighOffset, LowOffset, Block);
            i = 0;
            Block = temp;
            height += TextMetric.tmHeight;
            if (SmallestFile <= Offset) return DrawDC;
        }

    }
    return DrawDC;
}

void ReadPage(HANDLE File, DWORD Granularity, LONGLONG FileSize, DWORDLONG OFFSET, _In_ LONG BytesOnString, HWND Window) {
     int Strings_On_Screen = ScrolledFilesInfo.ReturnStringsOnScreen();

     size_t HeapPartSize = BytesOnString * Strings_On_Screen + 1;

    char* BufferString = (char*)HeapAlloc(ScrolledFilesInfo.ReturnLocalHeap(), 0, HeapPartSize);

    DWORD Block = Granularity, height = 1;
    OFFSET *= BytesOnString;
    DWORDLONG i = OFFSET;
    OFFSET = (OFFSET / Granularity);
    
    TEXTMETRIC TextMetric = ScrolledFilesInfo.ReturnTextMetric();

    OFFSET *= Granularity;
    DWORD HighOffset = ((OFFSET >> 32) & 0xFFFFFFFFul);
    DWORD LowOffset = static_cast<DWORD>(OFFSET & 0xFFFFFFFFul);

    if (Block > FileSize - OFFSET) Block = FileSize - OFFSET;
    PBYTE LRFILE = (PBYTE)MapViewOfFile(File, FILE_MAP_READ, HighOffset, LowOffset, Block);
    int CharOffset = 0;
    Block = Strings_On_Screen;
    i -= OFFSET;

    for (int count = 0; count < Block; count++) {
       
        for (int j = 0; j < BytesOnString; j++) {
            CharOffset += snprintf (BufferString + CharOffset, HeapPartSize - CharOffset, "%C", LRFILE[i + j]);
        }


        i += BytesOnString;

        if (i == Granularity) {
            UnmapViewOfFile(LRFILE);
            int temp = Block;
            Block = Granularity;
            OFFSET = (((OFFSET / Granularity) + 1) * Granularity);
            DWORD HighOffset = ((OFFSET >> 32) & 0xFFFFFFFFul);
            DWORD LowOffset = static_cast<DWORD>(OFFSET & 0xFFFFFFFFul);
            if (Block > FileSize - OFFSET) Block = FileSize - OFFSET;
            PBYTE LRFILE = (PBYTE)MapViewOfFile(File, FILE_MAP_READ, HighOffset, LowOffset, Block);
            i = 0;
            Block = temp;
        }
       
    }
    Comparator.AddPage(BufferString, Window);
        return;
}

BOOL CompareAllFiles() {

    std::vector <std::pair <int, std::pair <HANDLE, HANDLE>>> ComparableFiles = Comparator.ReturnOpenFileHandles();

    DWORD Block = ScrolledFilesInfo.ReturnGranularity();
    for (int FileCount = 0; FileCount < Comparator.ReturnOpenFileHandles().size(); FileCount++) {

        HANDLE FirstFile = ComparableFiles[FileCount].second.first;

        for (int ComparableFileCount = 0; ComparableFileCount < Comparator.ReturnOpenFileHandles().size(); ComparableFileCount++) {

            if (FirstFile == ComparableFiles[ComparableFileCount].second.first) continue;

            HANDLE SecondFile = ComparableFiles[ComparableFileCount].second.first;
            

           
           cWindowInfo* FirstWindow = (cWindowInfo*)GetWindowLongPtr(Comparator.ReturnUpdatingWindow(ComparableFiles[FileCount].first), GWLP_USERDATA);
           LARGE_INTEGER FirstFileSize = FirstWindow->ReturnFileSize();
           cWindowInfo* SecondWindow = (cWindowInfo*)GetWindowLongPtr(Comparator.ReturnUpdatingWindow(ComparableFiles[ComparableFileCount].first), GWLP_USERDATA);
           LARGE_INTEGER SecondFileSize = SecondWindow->ReturnFileSize();


           PBYTE FRFILE = (PBYTE)MapViewOfFile(FirstFile, FILE_MAP_READ,0, 0, Block);
           PBYTE SRFILE = (PBYTE)MapViewOfFile(SecondFile, FILE_MAP_READ, 0, 0, Block);

           LONGLONG MainOffset = 0;

           DWORD GranOffset = 0;
           while (MainOffset + GranOffset < min(FirstFileSize.QuadPart, SecondFileSize.QuadPart)) {
               if (FRFILE[GranOffset] != SRFILE[GranOffset])
                   if(Comparator.FindDifferences((MainOffset + GranOffset)))
                   Comparator.AddDifferences((MainOffset + GranOffset));
               GranOffset++;
                   if (GranOffset == Block) {
                       UnmapViewOfFile(FRFILE);
                       UnmapViewOfFile(SRFILE);
                       int temp = Block;
                       DWORD Granularity = ScrolledFilesInfo.ReturnGranularity();
                       Block = Granularity;
                       MainOffset = (((MainOffset / Granularity) + 1) * Granularity);
                       DWORD HighOffset = ((MainOffset >> 32) & 0xFFFFFFFFul);
                       DWORD LowOffset = static_cast<DWORD>(MainOffset & 0xFFFFFFFFul);
                       if (Block > FirstFileSize.QuadPart - MainOffset) Block = FirstFileSize.QuadPart - MainOffset;
                       PBYTE FRFILE = (PBYTE)MapViewOfFile(FirstFile, FILE_MAP_READ, HighOffset, LowOffset, Block);

                       Block = Granularity;
                       if (Block > SecondFileSize.QuadPart - MainOffset) Block = SecondFileSize.QuadPart - MainOffset;
                       PBYTE SRFILE = (PBYTE)MapViewOfFile(SecondFile, FILE_MAP_READ, HighOffset, LowOffset, Block);
                       GranOffset = 0;
                       Block = temp;
                   }
           }
               
        }
    }
    return TRUE;
}