#ifndef WORKSPACEIPC_H
#define WORKSPACEIPC_H

#include <QObject>

class WorkspaceIpc : public QObject{

    Q_OBJECT

public:
    explicit WorkspaceIpc(QObject *parent = nullptr);

signals:

public slots:
};

#endif // WORKSPACEIPC_H
