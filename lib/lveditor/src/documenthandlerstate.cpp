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

#include "live/documenthandlerstate.h"
#include "live/documenteditfragment.h"

namespace lv{

DocumentHandlerState::DocumentHandlerState()
    : m_editingFragment(0)
{
}

DocumentHandlerState::~DocumentHandlerState(){
    delete m_editingFragment;
}

void DocumentHandlerState::setEditingFragment(DocumentEditFragment *fragment){
    clearEditingFragment();
    m_editingFragment = fragment;
}

void DocumentHandlerState::clearEditingFragment(){
    if ( m_editingFragment ){
        delete m_editingFragment;
        m_editingFragment = 0;
    }
}

}// namespace
