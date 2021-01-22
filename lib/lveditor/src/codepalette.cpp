/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "live/codepalette.h"
#include <QFileInfo>

namespace lv{

/**
 * \class lv::CodePalette
 * \brief Palette display of a property
 * \ingroup lveditor
 */

/**
 * \fn lv::CodePalette::value()
 * \brief Property value corresponding to this palette
 */

/**
 * \fn QQuickItem* lv::CodePalette::item()
 * \brief Visual item for this palette
 */

/**
 * \fn lv::CodePalette::type
 * \brief Returns the palette type
 */

/**
 * \fn lv::CodePalette::name
 * \brief Palette name
 */

/**
 * \brief Default contructor of CodePalette
 */
CodePalette::CodePalette(QObject *parent)
    : QObject(parent)
    , m_isChangingValue(false)
    , m_bindingChange(false)
    , m_item(nullptr)
    , m_editFragment(nullptr)
{
}

/**
 * \brief Default destructor
 */
CodePalette::~CodePalette(){}

/**
 * \brief Assign property value (binding change)
 */
void CodePalette::setValueFromBinding(const QVariant &value){
    if ( m_isChangingValue )
        return;

    m_bindingChange = true;
    m_value = value;
    emit init(value);
    m_bindingChange = false;
}

void CodePalette::initViaSource(){
    emit sourceInit();
}

/**
 * \brief Value setter for palette
 */
void CodePalette::setValue(const QVariant &value){
    if ( (value.canConvert<QObject*>() || m_value != value) && !m_bindingChange ){
        m_isChangingValue = true;
        m_value = value;
        emit valueChanged();
        m_isChangingValue = false;
    }
}

QString CodePalette::name() const{
    return QFileInfo(m_path).baseName();
}

}// namespace
