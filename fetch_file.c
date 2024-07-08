#include <windows.h>
#include <wininet.h>
#include <stdio.h>

void downloadFile(const char* url, const char* filename) {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    char buffer[1024];
    HANDLE hFile;
    DWORD bytesWritten;

    // Initialize WinINet
    hInternet = InternetOpen("HTTPGET", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        printf("InternetOpen error: %lu\n", GetLastError());
        return;
    }

    // Open URL
    hConnect = InternetOpenUrl(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);
    if (hConnect == NULL) {
        printf("InternetOpenUrl error: %lu\n", GetLastError());
        InternetCloseHandle(hInternet);
        return;
    }

    // Create or open the file using Windows API
    hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to open file for writing: %lu\n", GetLastError());
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return;
    }

    // Read data from URL and write directly to file
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
        if (!WriteFile(hFile, buffer, bytesRead, &bytesWritten, NULL)) {
            printf("Failed to write to file: %lu\n", GetLastError());
            CloseHandle(hFile);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hInternet);
            return;
        }
    }

    // Close file
    CloseHandle(hFile);

    // Cleanup
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}


int setup_download() {
    const char* url = "https://raw.githubusercontent.com/AxistormDuBled/Adventurer-X/main/auto_update.c";
    const char* filename = "up_to_date.txt";
    downloadFile(url, filename);
    return 0;
}
