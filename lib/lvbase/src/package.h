#ifndef LVPACKAGE_H
#define LVPACKAGE_H

#include "live/lvbaseglobal.h"
#include "live/version.h"

#include <memory>
#include <list>
#include <map>

namespace lv{

class MLNode;
class PackageGraph;

class PackagePrivate;
class LV_BASE_EXPORT Package{

    DISABLE_COPY(Package);

public:
    /** Shared pointer to Package */
    typedef std::shared_ptr<Package> Ptr;

    class Context;

    class LV_BASE_EXPORT Reference{
    public:
        Reference(const std::string& n, Version v) : name(n), version(v){}

        std::string name;
        Version     version;
    };

    class LV_BASE_EXPORT Library{

    public:
        enum FlagResult{
            Equal,
            HasLess,
            HasMore,
            Different
        };

    public:
        Library(const std::string& n, Version v) : name(n), version(v){}

        FlagResult compareFlags(const Library& other);

        std::string            name;
        std::string            path;
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
    const std::map<std::string, Package::Reference*>& dependencies() const;
    const std::map<std::string, Package::Library*>& libraries() const;

    void assignContext(PackageGraph* graph);
    PackageGraph* contextOwner();
    Context* context();

private:
    Package(const std::string& path, const std::string& filePath, const std::string& name, const Version& version);

    PackagePrivate* m_d;

};

} // namespace

#endif // PACKAGE_H
