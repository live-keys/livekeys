#ifndef LVQMLDIR_H
#define LVQMLDIR_H

#include <QObject>
#include <QJSValue>

namespace lv {

/// \private
class QmlDir : public QObject{

    Q_OBJECT
public:
    explicit QmlDir(QObject *parent = nullptr);

public slots:
    QStringList list(QJSValue path);
    QJSValue listDetail(QJSValue path, QJSValue opt = QJSValue());
    QJSValue detail(QJSValue path);
    bool mkDir(QJSValue path);
    bool mkPath(QJSValue path);
    bool remove(QJSValue path);
    bool rename(QJSValue old, QJSValue nu);
};

}

#endif // LVQMLDIR_H
