#include <iostream>
#include <filesystem>
#include <windows.h>

namespace fs = std::filesystem;

// Print the current directory
void printCurrentDirectory(const std::wstring& message) {
    std::wcout << message << fs::current_path().wstring() << std::endl;
}

// Recursively search for a subdirectory inside the current directory
fs::path findSubdirectory(const std::string& dirName, const fs::path& root) {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied)) {
            if (entry.is_directory() && entry.path().filename() == dirName) {
                return entry.path(); // Return the first match found
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error searching for directory: " << e.what() << std::endl;
    }
    return ""; // Return empty if not found
}

// Search upwards (parent directories) for a directory
fs::path findInParentDirectories(const std::string& dirName, fs::path currentPath) {
    while (!currentPath.empty()) {
        fs::path possiblePath = currentPath / dirName;
        if (fs::exists(possiblePath) && fs::is_directory(possiblePath)) {
            return possiblePath;
        }
        currentPath = currentPath.parent_path(); // Move up one level
    }
    return "";
}

// Change directory globally using SetCurrentDirectoryW()
void changeDirectory(const std::string& path, bool quiet) {
    try {
        fs::path newPath = path;  // Keep it as relative

        // Check if path exists directly
        if (!fs::exists(newPath) || !fs::is_directory(newPath)) {
            newPath = findSubdirectory(path, fs::current_path());  // Search downwards
        }

        // If still not found, search upwards (parent folders)
        if (newPath.empty() || !fs::exists(newPath)) {
            newPath = findInParentDirectories(path, fs::current_path());
        }

        if (!newPath.empty() && fs::exists(newPath) && fs::is_directory(newPath)) {
            fs::current_path(newPath);  // Change for the program
            SetCurrentDirectoryW(newPath.wstring().c_str());  // Change globally in Windows
            if (!quiet) {
                std::cout << "Changed to directory: " << newPath << std::endl;
            }
        }
        else {
            std::cerr << "Error: Directory not found: " << path << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error changing directory: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    printCurrentDirectory(L"Current directory: ");

    if (argc < 2) {
        std::cerr << "Usage: NativeFive <path> [/q]" << std::endl;
        return 1;
    }

    std::string path = argv[1];
    bool quiet = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "/q") quiet = true;
    }

    changeDirectory(path, quiet);
    printCurrentDirectory(L"New directory: ");

    return 0;
}
