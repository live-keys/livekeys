/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVPACKAGE_H
#define LVPACKAGE_H

#include "live/lvbaseglobal.h"
#include "live/version.h"
#include "live/utf8.h"

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
    /** Const shared pointer to the Package */
    typedef std::shared_ptr<Package> ConstPtr;

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

        /** Reference name */
        Utf8    name;
        /** Reference version */
        Version version;
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

        /** Package name */
        Utf8 name;
        /** Package path */
        Utf8 path;
        /** Package version */
        Version version;
        /** Package flags */
        std::list<std::string> flags;
    };

    /**
     * \class lv::Package::Library
     * \brief A structure representing a project entry within this package
     * \ingroup lvbase
     */
    class LV_BASE_EXPORT ProjectEntry{

    public:
        ProjectEntry(const Utf8& path, const Utf8& label, const Utf8& category, const Utf8& description = "", const Utf8& icon = "")
            : m_path(path), m_label(label), m_category(category), m_description(description), m_icon(icon){}

        const Utf8& path() const{ return m_path; }
        const Utf8& label() const{ return m_label; }
        const Utf8& category() const{ return m_category; }
        const Utf8& description() const{ return m_description; }
        const Utf8& icon() const{ return m_icon; }

    private:
        Utf8 m_path;
        Utf8 m_label;
        Utf8 m_category;
        Utf8 m_description;
        Utf8 m_icon;
    };

    /** Default name of the package file */
    static const char* fileName;

public:
    ~Package();

    static bool existsIn(const std::string& path);
    static Package::Ptr createFromPath(const std::string& path);
    static Package::Ptr createFromNode(const std::string &path, const std::string& filePath, const MLNode& m);

    const std::string& name() const;
    const std::string& path() const;
    const std::string& filePath() const;
    const std::string& documentation() const;
    const std::string& release() const;
    const Version& version() const;
    const std::map<std::string, Package::Reference*>& dependencies() const;
    const std::map<std::string, Package::Library*>& libraries() const;
    const std::vector<std::string> internalLibraries() const;
    std::vector<std::string> allModules() const;

    void assignContext(PackageGraph* graph);
    PackageGraph* contextOwner();
    Context* context();

    bool hasWorkspace() const;
    const std::string& workspaceLabel() const;
    const std::vector<std::pair<std::string, std::string> >& workspaceTutorialsSections() const;
    const std::vector<ProjectEntry>& workspaceSamples();

private:
    static std::vector<std::string> findModules(const std::string& path);

    Package(const std::string& path, const std::string& filePath, const std::string& name, const Version& version);

    PackagePrivate* m_d;

};

} // namespace

#endif // PACKAGE_H
