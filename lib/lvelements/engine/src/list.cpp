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

#include "list.h"
#include "v8nowarnings.h"

namespace lv{ namespace el{

List::List(Engine *engine)
    : Element(engine)
    , m_data(nullptr)
    , m_isObservable(false)
    , m_at(&List::atImpl)
    , m_length(&List::length)
{
}

List::List(Engine *engine,
        void *data,
        const std::function<ScopedValue (List *, int)> &at,
        const std::function<int (List *)> &length,
        bool isObservable)
    : Element(engine)
    , m_data(data)
    , m_isObservable(isObservable)
    , m_at(at)
    , m_length(length)
{
}

List::List(Engine *engine,
        void *data,
        const std::function<ScopedValue (List *, int)> &at,
        const std::function<int (List *)> &length,
        const std::function<void (List *, int, ScopedValue)> &assign,
        const std::function<void (List *, ScopedValue)> &append,
        const std::function<void (List *)> &clear,
        bool isObservable)
    : Element(engine)
    , m_data(data)
    , m_isObservable(isObservable)
    , m_at(at)
    , m_length(length)
    , m_assign(assign)
    , m_append(append)
    , m_clear(clear)
{
}

List::~List(){
    if ( m_dataDestructor )
        m_dataDestructor(this);
}

ScopedValue List::atImpl(List *l, int){
    lv::Exception e = CREATE_EXCEPTION(lv::Exception, "at function is unavailable for Lists", lv::Exception::toCode("~List"));
    l->engine()->throwError(&e, nullptr);
    return ScopedValue(l->engine());
}

int List::lengthImpl(List *l){
    lv::Exception e = CREATE_EXCEPTION(lv::Exception, "length function is unavailable for Lists", lv::Exception::toCode("~List"));
    l->engine()->throwError(&e, nullptr);
    return 0;
}

}} // namespace lv, el
