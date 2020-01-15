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

#ifndef LVWORKERTHREAD_H
#define LVWORKERTHREAD_H

#include "live/lvviewglobal.h"
#include "live/qmlact.h"
#include <QObject>
#include <QEvent>
#include <functional>

namespace lv{

class WorkerThreadPrivate;

/// \private
class LV_VIEW_EXPORT WorkerThread : public QObject{

    Q_OBJECT

public:
    /// \private
    class CallEvent : public QEvent{

        friend class WorkerThread;
        friend class WorkerThreadPrivate;

    public:
        CallEvent(
            int callerIndex,
            const QVariant& args = QVariant(),
            const QList<Shared*>& transferObjects = QList<Shared*>()
        );

        CallEvent* callbackEvent(const QVariant& v);

    private:
        int            m_callerIndex;
        QVariant       m_args;
        QList<Shared*> m_transferObjects;
    };

public:
    WorkerThread(const QList<QString>& sources, QObject* parent = nullptr);
    virtual ~WorkerThread();

    void postWork(QmlAct* caller, const QVariantList &values, const QList<Shared*> objectTransfers);

    void start();

    QList<QmlAct*>& acts();

    bool event(QEvent * ev);

private:
    QJSEngine*           m_engine;
    QThread*             m_thread;
    QList<lv::QmlAct*>    m_acts;
    QList<QString>       m_actFunctionsSource;
    QList<QJSValue>      m_actFunctions;
    QMap<int, QPair<QObject*, QString> > m_specialFunctions;
    WorkerThreadPrivate* m_d;
};

inline QList<QmlAct *> &WorkerThread::acts(){
    return m_acts;
}

}// namespace

#endif // LVWORKERTHREAD_H
