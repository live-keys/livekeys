#ifndef LVPATH_H
#define LVPATH_H

#include "live/lvbaseglobal.h"
#include "live/utf8.h"
#include "live/datetime.h"

#include <vector>

namespace lv{

class LV_BASE_EXPORT Path{

public:
    enum CopyOptions{
        OverwriteExisting = 1,
        SkipExisting = 2,
        UpdateExisting = 4
    };

    static const char separator;

public:
    static std::string temporaryDirectory();
    static bool exists(const std::string& s);

    static bool createDirectory(const std::string& p);
    static bool createDirectories(const std::string& p);
    static bool isDir(const std::string& p);

    static bool remove(const std::string& p);
    static void rename(const std::string& from, const std::string& to);

    static bool copyFile(const std::string& from, const std::string& to, int options = Path::OverwriteExisting);
    static void copyRecursive(const std::string& from, const std::string& to, int options = Path::OverwriteExisting);

    static std::string join(const std::string& p1, const std::string& p2);
    template <typename... Paths>
    static std::string join(const std::string& p1, const std::string& p2, Paths... paths);

    static std::string joinExisting(const std::string& p1, const std::string& p2);
    template <typename... Paths>
    static std::string joinExisting(const std::string& p1, const std::string& p2, Paths... paths);


    static std::vector<std::string> split(const std::string p);
    static std::string name(const std::string& p);
    static std::string parent(const std::string& p);
    static std::string absolutePath(const std::string& p);
    static std::string resolve(const std::string& p);
    static std::string rootPath(const std::string& p);
    static std::string toUnixSeparator(const std::string& p);
    static bool isRelative(const std::string& p);
    static bool isAbsolute(const std::string& p);

    static bool isFile(const std::string& p);
    static std::string baseName(const std::string& p);
    static std::string extension(const std::string& p);
    static std::string suffix(const std::string& p);
    static std::string completeSuffix(const std::string& p);
    static bool hasExtensions(const std::string& p, const std::vector<std::string>& ext);

    static int permissions(const std::string& p);
    static void setPermissions(const std::string& p, int permissions);

    static void createSymlink(const std::string& linkPath, const std::string& path);
    static bool isSymlink(const std::string& p);
    static std::string followSymlink(const std::string& p);

    static std::string relativePath(const std::string& referencePath, const std::string& path);

    static DateTime lastModified(const std::string& path);

private:
    Path();
};

template <typename... Paths>
std::string Path::join(const std::string& p1, const std::string& p2, Paths... paths) {
    return Path::join(Path::join(p1, p2), paths...);
}

template <typename... Paths>
std::string Path::joinExisting(const std::string& p1, const std::string& p2, Paths... paths) {
    return Path::join(Path::join(p1, p2), paths...);
}

}// namespace

#endif // LVPATH_H
