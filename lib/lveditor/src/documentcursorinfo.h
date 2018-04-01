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

#ifndef LVDOCUMENTCURSORINFO_H
#define LVDOCUMENTCURSORINFO_H

#include <QObject>

#include "live/lveditorglobal.h"

namespace lv{

class LV_EDITOR_EXPORT DocumentCursorInfo: public QObject{

    Q_OBJECT
    Q_PROPERTY(bool canBind   READ canBind   CONSTANT)
    Q_PROPERTY(bool canUnbind READ canUnbind CONSTANT)
    Q_PROPERTY(bool canEdit   READ canEdit   CONSTANT)
    Q_PROPERTY(bool canAdjust READ canAdjust CONSTANT)

public:
    DocumentCursorInfo(bool canBind, bool canUnbind, bool canEdit, bool canAdjust, QObject* parent = 0);
    DocumentCursorInfo(QObject* parent = 0);
    ~DocumentCursorInfo();

    bool canBind() const;
    bool canUnbind() const;
    bool canEdit() const;
    bool canAdjust() const;

private:
    bool m_canBind;
    bool m_canUnbind;
    bool m_canEdit;
    bool m_canAdjust;
};

inline bool DocumentCursorInfo::canBind() const{
    return m_canBind;
}

inline bool DocumentCursorInfo::canUnbind() const{
    return m_canUnbind;
}

inline bool DocumentCursorInfo::canEdit() const{
    return m_canEdit;
}

inline bool DocumentCursorInfo::canAdjust() const{
    return m_canAdjust;
}

}// namespace

#endif // LVDOCUMENTCURSORINFO_H
