#ifndef LVWORKER_H
#define LVWORKER_H

#include <QObject>
#include <QQmlListProperty>

namespace lv{

class Act;
class WorkerThread;

/// \private
class Worker : public QObject{

    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> acts READ acts)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

    QQmlListProperty<QObject> acts();
    void appendAct(QObject*);
    int actCount() const;
    QObject *act(int index) const;
    void clearActs();

private:
    static void appendAct(QQmlListProperty<QObject>*, QObject*);
    static int actCount(QQmlListProperty<QObject>*);
    static QObject* act(QQmlListProperty<QObject>*, int);
    static void clearActs(QQmlListProperty<QObject>*);

private:
    QList<lv::Act*> m_acts;
    WorkerThread*   m_filterWorker;

};

}// namespace

#endif // LVWORKER_H
