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

#include "tuple.h"

namespace lv{ namespace el{

Tuple::Tuple(Engine *engine)
    : Element(engine)
{
}

Tuple::~Tuple(){

}

InstanceProperty *Tuple::addProperty(
        const std::string &name,
        const std::string &type,
        ScopedValue value,
        bool isDefault,
        bool isWritable,
        const std::string &notifyEvent)
{
    InstanceProperty* ip = Element::addProperty(name, type, value, isDefault, isWritable, notifyEvent);
    if ( !notifyEvent.empty() ){
        try {
            on(notifyEvent, [this, name](const Function::Parameters&){
                propertyChanged(name);
            });
        } catch (lv::Exception exc){
            engine()->throwError(&exc, nullptr);
            return nullptr;
        }
    }
    return ip;
}

}} // namespace
