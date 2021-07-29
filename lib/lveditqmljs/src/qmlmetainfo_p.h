#ifndef LVQMLMEMORYINFO_H
#define LVQMLMEMORYINFO_H

#include <QObject>
#include <QJSValue>

namespace lv {

class QmlMetaInfo : public QObject{

    Q_OBJECT

public:
    explicit QmlMetaInfo(QObject *parent = nullptr);
    ~QmlMetaInfo();

public slots:
    bool isCppOwned(QObject* obj);
    bool isJsOwned(QObject* obj);
    QObject* cppParent(QObject* obj);
    bool isWritable(QObject* obj, QString name);
    QJSValue listProperties(QObject* obj);
};

}

#endif // LVQMLMEMORYINFO_H
