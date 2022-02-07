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

#include "constructor.h"
#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "v8nowarnings.h"


namespace lv{ namespace el{

void Constructor::bindLifeTimeWithObject(Element *e, const v8::FunctionCallbackInfo<v8::Value> &info){
    e->setLifeTimeWithObject(info.This());
}

void Constructor::assignSelfReturnValue(const v8::FunctionCallbackInfo<v8::Value> &info){
    info.GetReturnValue().Set(info.This());
}

void Constructor::nullImplementation(const v8::FunctionCallbackInfo<v8::Value> &info){
    Engine* engine = reinterpret_cast<Engine*>(info.GetIsolate()->GetData(0));
    Exception e = CREATE_EXCEPTION(lv::Exception, "Class is not creatable.", Exception::toCode("~Constructor"));
    engine->throwError(&e, nullptr);
}

}} // namespace lv, el
