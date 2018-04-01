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

#include "live/documenteditfragment.h"
#include "live/codedeclaration.h"

namespace lv{

DocumentEditFragment::DocumentEditFragment(CodeDeclaration::Ptr declaration, CodeConverter *converter)
    : m_declaration(declaration)
    , m_converter(converter)
    , m_binding(0)
    , m_actionType(DocumentEditFragment::Edit)
{

}

DocumentEditFragment::~DocumentEditFragment(){
}

int DocumentEditFragment::valuePosition() const{
    return m_declaration->valuePosition();
}

int DocumentEditFragment::valueLength() const{
    return m_declaration->valueLength();
}

}// namespace
