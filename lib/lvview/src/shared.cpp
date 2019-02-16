/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "live/shared.h"
#include "live/filter.h"

namespace lv{

Shared::Shared(QObject *parent)
    : QObject(parent)
    , m_isValid(true)
    , m_readers(nullptr)
{
}

Shared::~Shared(){
    delete m_readers;
}

bool Shared::read(Filter *call, Shared *data){
    if ( !Shared::isValid(data) )
        return false;

    if ( call->workerThread() )
        data->readers()->reserved.insert(call);

    return true;
}

void Shared::release(Filter *call, Shared *data){
    if ( data && data->m_readers ){
        data->m_readers->reserved.remove(call);
        if ( data->m_readers->observer )
            data->m_readers->observer->process();
    }
}

bool Shared::isValid(const Shared *data){
    return (data && data->m_isValid);
}

void Shared::invalidate(Shared *data){
    data->m_isValid = false;
}

Shared *Shared::reloc(){
    Shared* sd = new Shared();
    Shared::invalidate(this);
    return sd;
}

Shared::Readers *Shared::readers(){
    if ( !m_readers ){
        m_readers = new Shared::Readers;
        m_readers->observer = nullptr;
    }
    return m_readers;
}

}// namespace
