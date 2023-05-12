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

#ifndef LVCOMPONENT_H
#define LVCOMPONENT_H

#include "live/elements/lvelengineglobal.h"
#include "live/elements/value.h"
#include "live/elements/callable.h"

namespace lv{ namespace el{

/**
 * @brief Wraps a template for creating new elements
 */
class LV_ELEMENTS_ENGINE_EXPORT Component{

    friend class Callable;

public:
    Element* create(const Function::Parameters& params);

private:
    Component(Engine* engine, const Callable& c);

    Engine*  m_engine;
    Callable m_constructor;
};

}} // namespace lv, el

#endif // LVCOMPONENT_H
