#include "Engine/Paths.h"
#include <iostream>
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

std::string GetExecutableDir() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path().string();
#else
    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer)-1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::filesystem::path(buffer).parent_path().string();
    }
    return "";
#endif
}

std::string GetAssetsDir() {
    return GetExecutableDir() + "\\assets\\";
}

std::string GetAssetPath(const std::string& relativePath) {
    std::cout << "Chemin GetAssetPath: " << GetAssetsDir() + relativePath << std::endl;
    return GetAssetsDir() + relativePath;
}