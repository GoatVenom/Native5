#include <windows.h> 
#include <aclapi.h>
#include <iostream>
#include <string>

bool showAllFiles = false;   // /A (Show all files, including hidden/system)
bool recursiveMode = false;  // /S (List subdirectories)
bool showOwner = false;      // /Q (Show file owner)

//  Convert Relative Path to Absolute Path
std::wstring GetFullPath(const std::wstring& relativePath) {
    wchar_t fullPath[MAX_PATH];
    if (GetFullPathNameW(relativePath.c_str(), MAX_PATH, fullPath, NULL)) {
        return std::wstring(fullPath);
    }
    return relativePath;  // If conversion fails, return the original path
}

// Print File Creation Time
void PrintFileTime(const FILETIME& fileTime) {
    FILETIME localFileTime;
    SYSTEMTIME systemTime;

    FileTimeToLocalFileTime(&fileTime, &localFileTime);
    FileTimeToSystemTime(&localFileTime, &systemTime);

    wchar_t formattedTime[20];
    swprintf(formattedTime, 20, L"%04d-%02d-%02d %02d:%02d:%02d",
        systemTime.wYear, systemTime.wMonth, systemTime.wDay,
        systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

    std::wcout << formattedTime << L"  ";
}

// Get File Owner (Only if /Q is used)
void GetFileOwner(const std::wstring& filePath) {
    if (!showOwner) return;

    PSID pOwnerSID = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;

    if (GetNamedSecurityInfoW(filePath.c_str(), SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION,
        &pOwnerSID, NULL, NULL, NULL, &pSD) == ERROR_SUCCESS) {
        WCHAR ownerName[256], domainName[256];
        DWORD ownerSize = 256, domainSize = 256;
        SID_NAME_USE sidType;

        if (LookupAccountSidW(NULL, pOwnerSID, ownerName, &ownerSize, domainName, &domainSize, &sidType)) {
            std::wcout << L"[" << domainName << L"\\" << ownerName << L"] ";
        }
    }
    if (pSD) LocalFree(pSD);
}

// List Directory Contents (Handles Relative Paths)
void ListDirectoryContents(std::wstring directory) {
    directory = GetFullPath(directory);  // Convert to absolute path

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW((directory + L"\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        std::wcerr << L"Error opening directory: \"" << directory << L"\" (Error Code: " << error << L")" << std::endl;
        return;
    }

    do {
        std::wstring fileName = findFileData.cFileName;
        std::wstring filePath = directory + L"\\" + fileName;

        // Skip "." and ".."
        if (fileName == L"." || fileName == L"..") continue;

        bool isDir = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
        bool isHidden = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN);
        bool isSystem = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM);

        // Hide system/hidden files unless /A is used
        if (!showAllFiles && (isHidden || isSystem)) continue;

        std::wcout << (isDir ? L"[D] " : L"   ");  // Mark directories

        PrintFileTime(findFileData.ftCreationTime);
        GetFileOwner(filePath);

        if (!isDir) {  // Show file size for regular files
            LARGE_INTEGER fileSize;
            fileSize.LowPart = findFileData.nFileSizeLow;
            fileSize.HighPart = findFileData.nFileSizeHigh;
            std::wcout << fileSize.QuadPart << L" bytes  ";
        }

        std::wcout << L"\"" << fileName << L"\"" << std::endl;

        // Recursively list subdirectories if /S is used
        if (isDir && recursiveMode) {
            ListDirectoryContents(filePath);
        }

    } while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);
}

// Main Function (Processes Command-Line Arguments)
int wmain(int argc, wchar_t* argv[]) {
    std::wstring directory = L"."; // Default to current directory

    // Process command-line arguments
    for (int i = 1; i < argc; i++) {
        std::wstring arg = argv[i];

        if (arg[0] == L'/') {  // Check for switches
            if (arg == L"/A") showAllFiles = true;
            if (arg == L"/S") recursiveMode = true;
            if (arg == L"/Q") showOwner = true;
        }
        else {
            directory = arg;  // Set directory if not a switch
        }
    }

    ListDirectoryContents(directory);
    return 0;
}