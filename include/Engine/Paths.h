#pragma once
#include <string>

std::string GetExecutableDir();
std::string GetAssetsDir();
std::string GetAssetPath(const std::string& relativePath);