/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef LVWORKER_H
#define LVWORKER_H

#include <QObject>
#include <QQmlListProperty>
#include <QQmlParserStatus>

namespace lv{

class Project;
class QmlAct;
class WorkerThread;

/// \private
class Worker : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlListProperty<QObject> acts READ acts)
    Q_CLASSINFO("DefaultProperty", "acts")

public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

    QQmlListProperty<QObject> acts();
    void appendAct(QObject*);
    int actCount() const;
    QObject *act(int index) const;
    void clearActs();

    void componentComplete();
    void classBegin(){}

private:
    void extractSource();

    static void appendAct(QQmlListProperty<QObject>*, QObject*);
    static int actCount(QQmlListProperty<QObject>*);
    static QObject* act(QQmlListProperty<QObject>*, int);
    static void clearActs(QQmlListProperty<QObject>*);

private:
    Project*          m_project;
    bool              m_componentComplete;
    QList<QString>    m_actSource;
    WorkerThread*     m_filterWorker;

};

}// namespace

#endif // LVWORKER_H
