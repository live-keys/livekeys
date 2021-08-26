#ifndef LVPLUGIN_H
#define LVPLUGIN_H

#include "live/lvbaseglobal.h"

#include <string>
#include <memory>
#include <map>
#include <list>

namespace lv{

class MLNode;
class PackageGraph;

class PluginPrivate;
class LV_BASE_EXPORT Plugin{

    DISABLE_COPY(Plugin);

public:
    /** Shared pointer to plugin */
    typedef std::shared_ptr<Plugin> Ptr;

    class Context;

public:
    /** Default name for a plugin file */
    static const char* fileName;

public:
    ~Plugin();

    static bool existsIn(const std::string& path);
    static Plugin::Ptr createFromPath(const std::string& path);
    static Plugin::Ptr createFromNode(const std::string &path, const std::string& filePath, const MLNode& m);
    static Plugin::Ptr createEmpty(const std::string& name);

    const std::string& name() const;
    const std::string& path() const;
    const std::string& filePath() const;
    /** Package getter */
    const std::string& package() const;
    const std::list<std::string>& dependencies() const;
    const std::list<std::string>& modules() const;
    const std::list<std::string>& libraryModules() const;
    const std::list<std::pair<std::string, std::string> >& palettes() const;

    void assignContext(PackageGraph* graph);
    Context* context();

private:
    Plugin(const std::string& path, const std::string& filePath, const std::string& name, const std::string& package);

    void addPalette(const std::string& type, const std::string& path);

    PluginPrivate* m_d;
};

} // namespace

#endif // PLUGIN_H
