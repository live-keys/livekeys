#ifndef LVPACKAGECONTEXT_H
#define LVPACKAGECONTEXT_H

#include "live/lvbaseglobal.h"
#include "live/package.h"
#include "live/plugin.h"

namespace lv{

/**
  \class lv::Package::Context
  \brief Presents a context for the given package in relation to the package graph
  \ingroup lvbase
 */
class LV_BASE_EXPORT Package::Context{

public:
    /** Blank constructor */
    Context(){}

    /** Package graph */
    PackageGraph* packageGraph;
    /** Packages dependent on the current one*/
    std::list<Package::Ptr> dependents;
    /** Dependencies of the package */
    std::list<Package::Ptr> dependencies;
    /** Plugins */
    std::map<std::string, Plugin::Ptr> plugins;

private:
    // disable copy
    Context(const Package::Context&);
    void operator=(const Package::Context&);
};

} // namespace

#endif // LVPACKAGECONTEXT_H
