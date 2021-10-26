#ifndef LVQMLCONTAINER_H
#define LVQMLCONTAINER_H

#include <QObject>
#include <QVector>
#include <QQmlListProperty>

namespace lv{

/// \private
class QmlContainer : public QObject{

    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> childObjects READ childObjects)
    Q_CLASSINFO("DefaultProperty", "childObjects")

public:
    explicit QmlContainer(QObject *parent = 0);
    ~QmlContainer();

    QQmlListProperty<QObject> childObjects();
    void appendChildObject(QObject* obj);
    int childObjectCount() const;
    QObject *childObject(int) const;
    void clearChildObjects();

public slots:
    QObject* prevChild(QObject* child) const;
    QObject* nextChild(QObject* child) const;

private:
    static void appendChildObject(QQmlListProperty<QObject>*, QObject*);
    static int childObjectCount(QQmlListProperty<QObject>*);
    static QObject* childObject(QQmlListProperty<QObject>*, int);
    static void clearChildObjects(QQmlListProperty<QObject>*);

private:
    QVector<QObject*> m_childObjects;
};

}// namespace

#endif // LVQMLCONTAINER_H
