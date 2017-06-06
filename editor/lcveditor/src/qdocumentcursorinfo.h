/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef QDOCUMENTCURSORINFO_H
#define QDOCUMENTCURSORINFO_H

#include "qlcveditorglobal.h"
#include <QObject>

namespace lcv{

class Q_LCVEDITOR_EXPORT QDocumentCursorInfo: public QObject{

    Q_OBJECT
    Q_PROPERTY(bool canBind   READ canBind   CONSTANT)
    Q_PROPERTY(bool canUnbind READ canUnbind CONSTANT)
    Q_PROPERTY(bool canEdit   READ canEdit   CONSTANT)
    Q_PROPERTY(bool canAdjust READ canAdjust CONSTANT)

public:
    QDocumentCursorInfo(bool canBind, bool canUnbind, bool canEdit, bool canAdjust, QObject* parent = 0);
    QDocumentCursorInfo(QObject* parent = 0);
    ~QDocumentCursorInfo();

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

inline bool QDocumentCursorInfo::canBind() const{
    return m_canBind;
}

inline bool QDocumentCursorInfo::canUnbind() const{
    return m_canUnbind;
}

inline bool QDocumentCursorInfo::canEdit() const{
    return m_canEdit;
}

inline bool QDocumentCursorInfo::canAdjust() const{
    return m_canAdjust;
}

}// namespace

#endif // QDOCUMENTCURSORINFO_H
