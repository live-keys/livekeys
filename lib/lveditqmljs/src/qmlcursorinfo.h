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

namespace lv{

/// \private
class QmlCursorInfo: public QObject{

    Q_OBJECT
    Q_PROPERTY(bool canBind   READ canBind   CONSTANT)
    Q_PROPERTY(bool canUnbind READ canUnbind CONSTANT)
    Q_PROPERTY(bool canEdit   READ canEdit   CONSTANT)
    Q_PROPERTY(bool canAdjust READ canAdjust CONSTANT)
    Q_PROPERTY(bool canShape  READ canShape  CONSTANT)

public:
    QmlCursorInfo(bool canBind, bool canUnbind, bool canEdit, bool canAdjust, bool canShape, QObject* parent = 0);
    QmlCursorInfo(QObject* parent = 0);
    ~QmlCursorInfo();

    bool canBind() const;
    bool canUnbind() const;
    bool canEdit() const;
    bool canAdjust() const;
    bool canShape() const;

private:
    bool m_canBind;
    bool m_canUnbind;
    bool m_canEdit;
    bool m_canAdjust;
    bool m_canShape;
};

inline bool QmlCursorInfo::canBind() const{
    return m_canBind;
}

inline bool QmlCursorInfo::canUnbind() const{
    return m_canUnbind;
}

inline bool QmlCursorInfo::canEdit() const{
    return m_canEdit;
}

inline bool QmlCursorInfo::canAdjust() const{
    return m_canAdjust;
}

inline bool QmlCursorInfo::canShape() const{
    return m_canShape;
}

}// namespace

#endif // LVDOCUMENTCURSORINFO_H
