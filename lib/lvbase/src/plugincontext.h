#ifndef LVPLUGINCONTEXT_H
#define LVPLUGINCONTEXT_H

#include "live/plugin.h"
#include "live/package.h"

namespace lv{

class LV_BASE_EXPORT Plugin::Context{

public:
    Context(){}

    PackageGraph* packageGraph;
    Package::Ptr package;

    std::string importId;
    std::list<Plugin::Ptr> localDependencies;
    std::list<Plugin::Ptr> localDependents;

private:
    Context(const Plugin::Context&);
    void operator=(const Plugin::Context&);
};

} // namespace

#endif // LVPLUGINCONTEXT_H
