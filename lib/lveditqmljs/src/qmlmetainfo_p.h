#ifndef QMLMEMORYINFO_H
#define QMLMEMORYINFO_H

#include <QObject>

namespace lv {

class QmlMetaInfo : public QObject
{
    Q_OBJECT
public:
    explicit QmlMetaInfo(QObject *parent = nullptr);

public slots:
    static bool isCppOwned(QObject* obj);
    static bool isJsOwned(QObject* obj);
    static QObject* cppParent(QObject* obj);
    static bool isWritable(QObject* obj, QString name);
};

}

#endif // QMLMEMORYINFO_H
