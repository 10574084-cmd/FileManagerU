#include "file_ops.hpp"
#include <algorithm>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

std::string joinPath(const std::string& base, const std::string& name) {
    if (base.empty()) return name;
    if (base.back() == '/') return base + name;
    return base + "/" + name;
}

bool listDirectory(const std::string& path, std::vector<FileEntry>& entries, std::string& error) {
    entries.clear();
    DIR* dir = opendir(path.c_str());
    if (!dir) { error = std::string("Cannot open directory: ") + std::strerror(errno); return false; }
    while (dirent* item = readdir(dir)) {
        std::string name(item->d_name);
        if (name == "." || name == "..") continue;
        const std::string full = joinPath(path, name);
        struct stat st {};
        if (stat(full.c_str(), &st) != 0) continue;
        FileEntry entry;
        entry.name = name;
        entry.directory = S_ISDIR(st.st_mode);
        entry.size = entry.directory ? 0ULL : static_cast<unsigned long long>(st.st_size);
        entries.push_back(entry);
    }
    closedir(dir);
    std::sort(entries.begin(), entries.end(), [](const FileEntry& a, const FileEntry& b) {
        if (a.directory != b.directory) return a.directory > b.directory;
        return a.name < b.name;
    });
    return true;
}

bool copyFile(const std::string& from, const std::string& to, std::string& error) {
    struct stat st {};
    if (stat(from.c_str(), &st) != 0 || !S_ISREG(st.st_mode)) {
        error = "Copy currently supports regular files only."; return false;
    }
    FILE* in = std::fopen(from.c_str(), "rb");
    if (!in) { error = std::string("Open source failed: ") + std::strerror(errno); return false; }
    FILE* out = std::fopen(to.c_str(), "wb");
    if (!out) { error = std::string("Open destination failed: ") + std::strerror(errno); std::fclose(in); return false; }
    char buffer[32 * 1024];
    bool ok = true;
    for (;;) {
        size_t n = std::fread(buffer, 1, sizeof(buffer), in);
        if (n && std::fwrite(buffer, 1, n, out) != n) { ok = false; break; }
        if (n < sizeof(buffer)) { if (std::ferror(in)) ok = false; break; }
    }
    if (std::fclose(in) != 0) ok = false;
    if (std::fclose(out) != 0) ok = false;
    if (!ok) { error = "Copy failed during read/write."; std::remove(to.c_str()); }
    return ok;
}

bool moveFile(const std::string& from, const std::string& to, std::string& error) {
    if (std::rename(from.c_str(), to.c_str()) == 0) return true;
    // Cross-device fallback is safe for regular files only; remove source only after a full copy.
    if (!copyFile(from, to, error)) return false;
    if (std::remove(from.c_str()) != 0) {
        error = std::string("Copied, but source removal failed: ") + std::strerror(errno);
        return false;
    }
    return true;
}

bool renamePath(const std::string& from, const std::string& to, std::string& error) {
    if (std::rename(from.c_str(), to.c_str()) == 0) return true;
    error = std::string("Rename failed: ") + std::strerror(errno);
    return false;
}

bool deleteFile(const std::string& path, std::string& error) {
    struct stat st {};
    if (lstat(path.c_str(), &st) != 0) { error = std::string("Cannot inspect path: ") + std::strerror(errno); return false; }
    if (S_ISDIR(st.st_mode)) {
        error = "Directory deletion is disabled in this initial build.";
        return false;
    }
    if (std::remove(path.c_str()) != 0) { error = std::string("Delete failed: ") + std::strerror(errno); return false; }
    return true;
}
