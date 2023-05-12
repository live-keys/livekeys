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

#ifndef LVMODULEFILEJSDATA_H
#define LVMODULEFILEJSDATA_H

#include "live/elements/lvelengineglobal.h"
#include "live/elements/compiler/modulefile.h"
#include "live/elements/jsmodule.h"

namespace lv{ namespace el{

class LV_ELEMENTS_ENGINE_EXPORT ModuleFileJsData : public ModuleFile::CompilationData{

public:
    ModuleFileJsData(const JsModule::Ptr& jsModule);

private:
    JsModule::Ptr m_jsModule;
};

}}// namespace lv, el

#endif // LVMODULEFILEJSDATA_H
