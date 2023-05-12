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

#ifndef LVTUPLE_H
#define LVTUPLE_H

#include "live/elements/element.h"

namespace lv{ namespace el{

class Tuple : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(Tuple)
            .base<Element>()
            .constructor()
            .scriptEvent("propertyChanged", &Tuple::propertyChanged)
        META_OBJECT_CLOSE
    }

public:
    Tuple(el::Engine* engine);
    ~Tuple();

    virtual InstanceProperty *addProperty(
        const std::string& name,
        const std::string& type,
        ScopedValue value,
        bool isDefault,
        bool isWritable,
        const std::string& notifyEvent
    ) override;

public:
    Event propertyChanged(const std::string& name){
        static Event::Id eid = eventId(&Tuple::propertyChanged); return notify(eid, name);
    }

private:

};

}}// namespace

#endif // LVTUPLE_H
