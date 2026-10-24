#pragma once
#include <string>
#include <vector>
struct FileEntry { std::string name; bool directory=false; unsigned long long size=0; };
bool listDirectory(const std::string&,std::vector<FileEntry>&,std::string&);
bool copyFile(const std::string&,const std::string&,std::string&);
bool moveFile(const std::string&,const std::string&,std::string&);
bool renamePath(const std::string&,const std::string&,std::string&);
bool deleteFile(const std::string&,std::string&);
std::string joinPath(const std::string&,const std::string&);
