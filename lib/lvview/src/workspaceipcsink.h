#ifndef WORKSPACEIPCSINK_H
#define WORKSPACEIPCSINK_H

#include <QObject>

class WorkspaceIpcSink : public QObject{

    Q_OBJECT

public:
    explicit WorkspaceIpcSink(QObject *parent = nullptr);

signals:

public slots:

};

#endif // WORKSPACEIPCSINK_H
