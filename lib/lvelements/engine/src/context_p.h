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

#ifndef LVCONTEXT_H
#define LVCONTEXT_H

#include "live/elements/lvelengineglobal.h"
#include "live/elements/engine.h"

#include "v8nowarnings.h"

namespace lv{ namespace el{

/**
 * @brief Wraps an engine context
 */
class Context{

public:
    Context(Engine* engine, const v8::Local<v8::Context>& context);
    ~Context();

    v8::Local<v8::Context> asLocal();
    Engine* engine() const;

private:
    Engine*                     m_engine;
    v8::Persistent<v8::Context> m_data;
};

inline v8::Local<v8::Context> Context::asLocal(){
    return m_data.Get(m_engine->isolate());
}

inline Engine *Context::engine() const{
    return m_engine;
}

}} // namespace lv, el

#endif // LVCONTEXT_H
