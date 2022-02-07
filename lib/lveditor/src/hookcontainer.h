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

#ifndef LVHOOKCONTAINER_H
#define LVHOOKCONTAINER_H

#include <QObject>
#include <QMap>
#include <QList>

#include "live/lveditorglobal.h"

namespace lv{

class Runnable;
class LV_EDITOR_EXPORT HookContainer : public QObject{

    Q_OBJECT

private:
    class Entry{
    public:
        QString builderId;
        QList<QObject*> builders;
    };

public:
    HookContainer(const QString& projectUrl, Runnable* r, QObject *parent = nullptr);
    ~HookContainer() override;

    void insertKey(const QString& sourceFile, const QString& id, QObject* obj);
    void removeEntry(const QString& sourceFile, const QString& id, QObject* obj);
    QMap<QString, QList<QObject*> > entriesForFile(const QString& sourceFile);
    QList<QObject*> entriesFor(const QString& file, const QString& id);

signals:
    void entryAdded(Runnable* runnable, const QString& file, const QString& id, QObject* object);
    void entryRemoved(const QString& file, const QString& id, QObject* object);

private:
    Runnable* m_runnable;
    QString   m_projectPath;
    QMap<QString, QMap<QString, QList<QObject*> > > m_entries;
};

}// namespace

#endif // LVHOOKCONTAINER_H
