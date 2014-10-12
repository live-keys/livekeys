/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#include "QMatState.hpp"

/*!
  \class QMatState
  \brief Mat texture state
  \inmodule lcvlib_cpp
 */

/*!
  \brief QMatState constructor.
 */
QMatState::QMatState():
    mat(0),
    textureIndex(-1),
    textureSync(false),
    linearFilter(true)
{
}

/*!
  \brief Compares 2 states.

  Parameters:
  \a other
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
