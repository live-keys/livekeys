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

#ifndef WORKSPACEMESSAGESTACK_H
#define WORKSPACEMESSAGESTACK_H

#include <QAbstractListModel>

namespace lv {

class WorkspaceMessageStack : public QAbstractListModel{

    Q_OBJECT
    Q_ENUMS(Type)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Type{
        Info = 0,
        Warning = 1,
        Error = 2
    };

public:
    enum Roles{
        Message = Qt::UserRole + 1,
        Code,
        MessageType
    };

    WorkspaceMessageStack(QObject* parent = nullptr);
    ~WorkspaceMessageStack(){}

    class WorkspaceMessage{
    public:
        QString message;
        int     code;
        Type    type;

        WorkspaceMessage(Type t = WorkspaceMessageStack::Info, const QString& m = "", int c = 0) : message(m), code(c), type(t){}
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &) const;
    QHash<int, QByteArray> roleNames() const;

public slots:
    void pushInfo(const QString& message, int code);
    void pushWarning(const QString& message, int code);
    void pushError(const QString& message, int code);
    void removeAt(int idx);
    void clear();
    int count();
signals:
    void messageAdded(int type, QString message, int code);
    void countChanged();
private:

    QList<WorkspaceMessage> m_messages;
    QHash<int, QByteArray> m_roles;


    Q_DISABLE_COPY(WorkspaceMessageStack)
};

}
#endif // WORKSPACEMESSAGESTACK_H
