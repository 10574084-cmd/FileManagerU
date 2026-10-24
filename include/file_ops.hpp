#pragma once
#include <string>
#include <vector>

struct FileEntry {
    std::string name;
    bool directory = false;
    unsigned long long size = 0;
};

bool listDirectory(const std::string& path, std::vector<FileEntry>& entries, std::string& error);
bool copyFile(const std::string& from, const std::string& to, std::string& error);
bool moveFile(const std::string& from, const std::string& to, std::string& error);
bool renamePath(const std::string& from, const std::string& to, std::string& error);
bool deleteFile(const std::string& path, std::string& error);
std::string joinPath(const std::string& base, const std::string& name);
