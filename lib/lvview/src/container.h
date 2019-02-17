#ifndef LVCONTAINER_H
#define LVCONTAINER_H

#include <QObject>
#include <QVector>
#include <QQmlListProperty>

namespace lv{

class Container : public QObject{

    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> childObjects READ childObjects)
    Q_CLASSINFO("DefaultProperty", "childObjects")

public:
    explicit Container(QObject *parent = 0);
    ~Container();

    QQmlListProperty<QObject> childObjects();
    void appendChildObject(QObject* obj);
    int childObjectCount() const;
    QObject *childObject(int) const;
    void clearChildObjects();

private:
    static void appendChildObject(QQmlListProperty<QObject>*, QObject*);
    static int childObjectCount(QQmlListProperty<QObject>*);
    static QObject* childObject(QQmlListProperty<QObject>*, int);
    static void clearChildObjects(QQmlListProperty<QObject>*);

private:
    QVector<QObject*> m_childObjects;
};

}// namespace

#endif // LVCONTAINER_H
