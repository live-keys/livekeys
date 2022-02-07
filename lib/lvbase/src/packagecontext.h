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

#ifndef LVPACKAGECONTEXT_H
#define LVPACKAGECONTEXT_H

#include "live/lvbaseglobal.h"
#include "live/package.h"
#include "live/module.h"

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
    /** Modules */
    std::map<std::string, Module::Ptr> modules;

private:
    // disable copy
    Context(const Package::Context&);
    void operator=(const Package::Context&);
};

} // namespace

#endif // LVPACKAGECONTEXT_H
