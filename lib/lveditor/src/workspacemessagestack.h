#ifndef WORKSPACEMESSAGESTACK_H
#define WORKSPACEMESSAGESTACK_H

#include <QAbstractListModel>

namespace lv {

class WorkspaceMessageStack : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles{
        Message = Qt::UserRole + 1,
        Code
    };

    WorkspaceMessageStack(QObject* parent = nullptr);
    ~WorkspaceMessageStack(){}

    class WorkspaceMessage
    {
    public:
        QString message;
        int code;

        WorkspaceMessage(QString m = "", int c = 0):
            message(m), code(c) {}
    };

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &) const;
    QHash<int, QByteArray> roleNames() const;
public slots:
    void pushMessage(QString m, int c);
    void removeAtIndex(int idx);
    void clear();
private:

    std::vector<WorkspaceMessage> m_messages;
    QHash<int, QByteArray> m_roles;


    Q_DISABLE_COPY(WorkspaceMessageStack)
};

}
#endif // WORKSPACEMESSAGESTACK_H
