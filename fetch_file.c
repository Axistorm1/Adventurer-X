#include <windows.h>
#include <wininet.h>
#include <stdio.h>
#pragma comment(lib, "wininet.lib")

void downloadFile(const char* url, const char* filename) {
    HINTERNET hInternet, hConnect;
    DWORD bytesRead;
    char buffer[1024];

    // Initialize WinINet
    hInternet = InternetOpen("HTTPGET", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        printf("InternetOpen error: %lu\n", GetLastError());
        return;
    }

    // Open URL
    hConnect = InternetOpenUrl(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        printf("InternetOpenUrl error: %lu\n", GetLastError());
        InternetCloseHandle(hInternet);
        return;
    }

    // Open file for writing
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Failed to open file for writing\n");
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return;
    }

    // Read data and write to file
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
        fwrite(buffer, 1, bytesRead, file);
    }

    // Close file
    fclose(file);

    // Cleanup
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    printf("File downloaded and saved as %s\n", filename);
}

int setup_download() {
    const char* url = "https://raw.githubusercontent.com/AxistormDuBled/Adventurer-X/main/auto_update.c";
    const char* filename = "up_to_date.txt";
    downloadFile(url, filename);
    return 0;
}
