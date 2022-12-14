#ifndef LVQMLDIR_H
#define LVQMLDIR_H

#include <QObject>
#include <QJSValue>
#include "live/viewengine.h"

#include "qmldirectorytostream.h"

namespace lv {

/// \private
class QmlDir : public QObject{

    Q_OBJECT

public:
    enum ReadFlags{
        NoFlags = 0,
        Subdirectories = 0x2,
        FollowSymlinks = 0x1
    };
    Q_ENUMS(Flags)

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

    lv::QmlStream* read(const QString& path, int flags);

private:
    ViewEngine* engine();
};

}

#endif // LVQMLDIR_H
