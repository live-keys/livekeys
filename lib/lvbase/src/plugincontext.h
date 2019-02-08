#ifndef LVPLUGINCONTEXT_H
#define LVPLUGINCONTEXT_H

#include "live/plugin.h"
#include "live/package.h"

namespace lv{

/**
  \class lv::Plugin::Context
  \brief Presents a context for the given plugin in relation to the package graph
  \ingroup lvbase
 */

class LV_BASE_EXPORT Plugin::Context{

public:
    /** Blank constructor */
    Context(){}

    PackageGraph* packageGraph;
    Package::Ptr package;

    std::string importId;
    std::list<Plugin::Ptr> localDependencies;
    std::list<Plugin::Ptr> localDependents;

private:
    // disable copy
    Context(const Plugin::Context&);
    void operator=(const Plugin::Context&);
};

} // namespace

#endif // LVPLUGINCONTEXT_H
