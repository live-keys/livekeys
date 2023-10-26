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
