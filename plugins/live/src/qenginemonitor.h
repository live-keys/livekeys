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

#ifndef QENGINEMONITOR_H
#define QENGINEMONITOR_H

#include <QObject>
#include "qliveglobal.h"

class QQuickItem;

class Q_LIVE_EXPORT QEngineMonitor : public QObject{

    Q_OBJECT

public:
    explicit QEngineMonitor(QObject *parent = 0);

    static QEngineMonitor* grabFromContext(QQuickItem* item, const QString& contextProperty = "engineMonitor");

signals:
    void beforeCompile();
    void afterCompile();
    void targetChanged();

public slots:
    void emitBeforeCompile();
    void emitAfterCompile();
    void emitTargetChanged();

};

inline void QEngineMonitor::emitBeforeCompile(){
    emit beforeCompile();
}

inline void QEngineMonitor::emitAfterCompile(){
    emit afterCompile();
}

inline void QEngineMonitor::emitTargetChanged(){
    emit targetChanged();
}

#endif // QENGINEMONITOR_H
