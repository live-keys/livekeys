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

#ifndef LVFILTERWORKER_H
#define LVFILTERWORKER_H

#include "live/lvviewglobal.h"
#include "live/filter.h"
#include <QObject>
#include <QEvent>
#include <functional>

namespace lv{

class FilterWorkerPrivate;

/// @private
class LV_VIEW_EXPORT FilterWorker : public QObject{

    Q_OBJECT

public:
    class CallEvent : public QEvent{

    public:
        CallEvent(
            const std::function<void()>& filter,
            Shared::ReadScope* readScope = nullptr);
        CallEvent(
            std::function<void()>&& filter,
            Shared::ReadScope* readScope = nullptr);
        CallEvent(
            const std::function<void()>& filter,
            const std::function<void()>& callCallback,
            Shared::ReadScope* readScope = nullptr);
        CallEvent(
            std::function<void()>&& filter,
            std::function<void()>&& callCallback,
            Shared::ReadScope* readScope = nullptr);
        void callFilter();
        Shared::ReadScope *readScope();
        bool hasCallback();

        CallEvent* callbackEvent();

    private:
        std::function<void()> m_filter;
        std::function<void()> m_callback;
        Shared::ReadScope*    m_readScope;
    };

public:
    FilterWorker(QObject* parent = nullptr);
    virtual ~FilterWorker();

    void postWork(
        const std::function<void()>& fnc,
        Shared::ReadScope* locker = nullptr);
    void postWork(
        const std::function<void()>& fnc,
        const std::function<void()>& cbk,
        Shared::ReadScope* locker = nullptr
    );
    void start();

    bool event(QEvent * ev);

private:
    QThread*             m_thread;
    FilterWorkerPrivate* m_d;
};

}// namespace

#endif // LVFILTERWORKER_H
