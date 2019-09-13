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

#include "qmatstate.h"

/**
 * \class QMatState
 * \brief Matrix texture state
 * \ingroup plugin-lcvcore
 */

/**
 * \brief QMatState constructor.
 */
QMatState::QMatState()
    : mat(0)
    , linearFilter(true)
    , textureIndex(-1)
    , textureSync(false)
{
}

/**
 * \brief Compares 2 states.
 */
int QMatState::compare(const QMatState *other) const{
    if ( textureIndex == -1 && other->textureIndex == -1 ){
        if ( mat == other->mat )
            return 0;
        else if ( mat < other->mat )
            return -1;
        else
            return 1;
    } else {
        if ( textureIndex == other->textureIndex )
            return 0;
        else if ( textureIndex < other->textureIndex )
            return -1;
        else
            return 1;
    }
    return 0;
}
