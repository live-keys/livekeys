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

#ifndef COMMANDS_H
#define COMMANDS_H

#include <QObject>
#include <QMap>
#include <QJSValue>

namespace lv{

class Commands : public QObject{

    Q_OBJECT

    class Node{
    public:
        Node() : object(0){}
        ~Node();

        Node* find(QObject* object);
        Node* add(QObject* object);
        void  remove(QObject* object);
        QList<QPair<Node *, bool> > recurseFind(
                const QStringList &list,
                QStringList::const_iterator it,
                bool hasFocus = false
        );
        QString recurseDump(QString prefix = "");

        QObject* object;
        QMap<QString, QJSValue>   functions;
        QMultiMap<QString, Node*> nodes;
    };

public:
    explicit Commands(QObject *parent = 0);
    ~Commands();

    QString dump();

public slots:
    QString add(QObject* object, const QJSValue& command);
    void removeCommandsFor(QObject* object);
    void execute(const QString& command);

private:
    QObjectList getCommandChain(QObject* object);

    Node* m_root;

};

}// namespace

#endif // COMMANDS_H
