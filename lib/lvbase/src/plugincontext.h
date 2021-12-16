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

    /** Package graph */
    PackageGraph* packageGraph;
    /** Package */
    Package::Ptr package;

    /** Import id */
    std::string importId;
    /** Local dependencies */
    std::list<Plugin::Ptr> localDependencies;
    /** Local dependents */
    std::list<Plugin::Ptr> localDependents;

private:
    DISABLE_COPY(Context);
};

} // namespace

#endif // LVPLUGINCONTEXT_H
