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

#ifndef LVQMLCOMPLETIONCONTEXTFINDER_H
#define LVQMLCOMPLETIONCONTEXTFINDER_H

#include "live/qmlcompletioncontext.h"

class QTextCursor;

namespace lv{

/**
 * \brief Finds the completion context at a given cursor position.
 *
 * Simply call the getContext function to get the actual completion context:
 *
 * \code
 * QTextCursor cursor(document);
 * cursor.setPosition(100);
 * QmlCompletionContextFinder finder;
 *
 * QmlCompltionContext::ConstPtr context = finder.getContext(cursor);
 * \endcode
 */
class LV_EDITQMLJS_EXPORT QmlCompletionContextFinder{

public:
    QmlCompletionContextFinder();
    ~QmlCompletionContextFinder();

    QmlCompletionContext::ConstPtr getContext(const QTextCursor& cursor);
};

}// namespace

#endif // LVQMLCOMPLETIONCONTEXTFINDER_H
