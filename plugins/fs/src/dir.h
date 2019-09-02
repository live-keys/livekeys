#ifndef DIR_H
#define DIR_H

#include <QObject>
#include <QJSValue>

namespace lv {

class Dir : public QObject
{
    Q_OBJECT
public:
    explicit Dir(QObject *parent = nullptr);

public slots:
    QStringList list(QJSValue path);
    bool mkDir(QJSValue path);
    bool mkPath(QJSValue path);
    bool remove(QJSValue path);
    bool rename(QJSValue old, QJSValue nu);
};

}

#endif // DIR_H
