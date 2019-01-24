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

#include "live/codepalette.h"
#include <QFileInfo>

namespace lv{

/**
 * \class lv::CodePalette
 * \brief Palette display of a property
 * \ingroup lveditor
 */

/**
 * \brief Default contructor of CodePalette
 */
CodePalette::CodePalette(QObject *parent)
    : QObject(parent)
    , m_bindingChange(false)
    , m_item(nullptr)
    , m_extension(nullptr)
    , m_ownExtension(false)
{
}

/**
 * \brief Default destructor
 */
CodePalette::~CodePalette(){
    if ( m_ownExtension && m_extension )
        delete m_extension;
}

/**
 * \brief Assign property value (binding change)
 */
void CodePalette::setValueFromBinding(const QVariant &value){
    m_bindingChange = true;
    m_value = value;
    emit init(value);
    m_bindingChange = false;
}

/**
 * \brief Palette name
 */
QString CodePalette::name() const{
    return QFileInfo(m_path).baseName();
}

}// namespace
