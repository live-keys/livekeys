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

#ifndef LVSCRIPT_H
#define LVSCRIPT_H

#include "live/elements/lvelengineglobal.h"
#include "live/elements/value.h"
#include <memory>

namespace lv{ namespace el{

class Engine;
class ModuleFile;
class ScriptPrivate;

/**
 * @brief Holds a compiled script
 */
class LV_ELEMENTS_ENGINE_EXPORT Script{

    friend class Engine;

public:
    typedef std::shared_ptr<Script> Ptr;
    typedef std::shared_ptr<Script> ConstPtr;

    Value run();
    Object loadAsModule(ModuleFile* mf);


public:
    ~Script();

private:
    static const char* encloseStart;
    static const char* encloseEnd;

    static const char* moduleEncloseStart;
    static const char* moduleEncloseEnd;

private:
    DISABLE_COPY(Script);

    Object loadAsModuleImpl(ModuleFile* mf, const v8::Local<v8::Context>& context);

    Script(Engine* engine, const v8::Local<v8::Script>& value, const std::string& path = "");

    ScriptPrivate* m_d;
};

}} // namespace lv, el

#endif // LVSCRIPT_H
