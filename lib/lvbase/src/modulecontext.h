#ifndef LVMODULECONTEXT_H
#define LVMODULECONTEXT_H

#include "live/module.h"
#include "live/package.h"

namespace lv{

/**
  \class lv::Plugin::Context
  \brief Presents a context for the given plugin in relation to the package graph
  \ingroup lvbase
 */
class LV_BASE_EXPORT Module::Context{

public:
    /** Blank constructor */
    Context(){}

    std::vector<Utf8> importSegments() const;

public:
    /** Package graph */
    PackageGraph* packageGraph;
    /** Package */
    Package::Ptr package;

    /** Import id */
    Utf8 importId;
    /** Local dependencies */
    std::list<Module::Ptr> localDependencies;
    /** Local dependents */
    std::list<Module::Ptr> localDependents;

private:
    DISABLE_COPY(Context);
};

inline std::vector<Utf8> Module::Context::importSegments() const{
    return Utf8(importId).split(".");
}

} // namespace

#endif // LVMODULECONTEXT_H
