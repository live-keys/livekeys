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

#include "component.h"

namespace lv{ namespace el{


Component::Component(Engine *engine, const Callable &c)
    : m_engine(engine)
    , m_constructor(c)
{
}

Element *Component::create(const Function::Parameters &params){
    return m_constructor.callAsConstructor(m_engine, params).toElement(m_engine);
}

}} // namespace lv, el
