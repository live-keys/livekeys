#ifndef LVPACKAGE_H
#define LVPACKAGE_H

#include "live/lvbaseglobal.h"
#include "live/version.h"

#include <memory>
#include <list>
#include <map>

namespace lv{

class MLNode;

class PackagePrivate;
class LV_BASE_EXPORT Package{

    DISABLE_COPY(Package);

public:
    typedef std::shared_ptr<Package> Ptr;

    class Dependency{
    public:
        Dependency(const std::string& n, Version v) : name(n), version(v){}

        std::string name;
        Version     version;
    };

    class Library{
    public:
        Library(const std::string& n, Version v) : name(n), version(v){}

        std::string            name;
        Version                version;
        std::list<std::string> flags;
    };

    static const char* fileName;

public:
    ~Package();

    static bool existsIn(const std::string& path);
    static Package::Ptr createFromPath(const std::string& path);
    static Package::Ptr createFromNode(const std::string &path, const std::string& filePath, const MLNode& m);

    const std::string& name() const;
    const std::string& path() const;
    const std::string& filePath() const;
    const Version& version() const;
    const std::string& extension() const;
    std::string extensionAbsolutePath() const;
    bool hasExtension() const;
    const std::map<std::string, Package::Dependency*>& dependencies() const;
    const std::map<std::string, Package::Library*>& libraries() const;

private:
    Package(const std::string& path, const std::string& filePath, const std::string& name, const Version& version);

    PackagePrivate* m_d;

};

} // namespace

#endif // PACKAGE_H
