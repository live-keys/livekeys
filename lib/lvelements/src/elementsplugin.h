#ifndef LVELEMENTSPLUGIN_H
#define LVELEMENTSPLUGIN_H

#include "live/elements/lvelementsglobal.h"
#include "live/plugin.h"

#include <memory>

namespace lv{ namespace el{

class Engine;
class ModuleFile;
class ModuleLibrary;

class ElementsPluginPrivate;
class LV_ELEMENTS_EXPORT ElementsPlugin{

    DISABLE_COPY(ElementsPlugin);

public:
    typedef std::shared_ptr<ElementsPlugin> Ptr;

public:
    ~ElementsPlugin();

    static ElementsPlugin::Ptr create(Plugin::Ptr plugin, Engine* engine);

    static ModuleFile *addModuleFile(ElementsPlugin::Ptr& epl, const std::string& name);

    ModuleFile* load(const ElementsPlugin::Ptr& epl, const std::string& name);
    ModuleLibrary* loadLibrary(const ElementsPlugin::Ptr& epl, const std::string& name);
    const Plugin::Ptr &plugin() const;

    const std::map<std::string, ModuleFile*>& fileExports() const;

private:
    ElementsPlugin(Plugin::Ptr plugin, Engine* engine);

    ElementsPluginPrivate* m_d;

};

}} // namespace lv, el

#endif // LVELEMENTSPLUGIN_H
