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

    /**
     * \class lv::Package::Reference
     * \brief Stores a reference to another package
     *
     * Contains just a name and version
     * \ingroup lvbase
     */
    class LV_BASE_EXPORT Reference{
    public:
        /** Default constructor */
        Reference(const std::string& n, Version v) : name(n), version(v){}

        std::string name;
        Version     version;
    };

    /**
     * \class lv::Package::Library
     * \brief A structure representing a package library
     *
     * When we import the package, LiveKeys will also load the library
     * We can have issues with different versions of the same library. This is manager through flags, strings which indicate
     * support of particular features. We compare flag sets or two libraries in order to provide maximum support of features.
     * \ingroup lvbase
     */
    class LV_BASE_EXPORT Library{

    public:
        /** Flag comparation results */
        enum FlagResult{
            /** Two libraries have same flags */
            Equal,
            /** The first library has less flags than the other one (it's a subset) */
            HasLess,
            /** The first library has more flags than the other one (it's a superset) */
            HasMore,
            /** Two sets of flags are not comparable */
            Different
        };

    public:
        /** Default constructor */
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
