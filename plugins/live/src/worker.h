#ifndef LVWORKER_H
#define LVWORKER_H

#include <QObject>
#include <QQmlListProperty>
#include <QQmlParserStatus>

namespace lv{

class Project;
class QmlAct;
class WorkerThread;

/// \private
class Worker : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlListProperty<QObject> acts READ acts)
    Q_CLASSINFO("DefaultProperty", "acts")

public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

    QQmlListProperty<QObject> acts();
    void appendAct(QObject*);
    int actCount() const;
    QObject *act(int index) const;
    void clearActs();

    void componentComplete();
    void classBegin(){}

private:
    void extractSource();

    static void appendAct(QQmlListProperty<QObject>*, QObject*);
    static int actCount(QQmlListProperty<QObject>*);
    static QObject* act(QQmlListProperty<QObject>*, int);
    static void clearActs(QQmlListProperty<QObject>*);

private:
    Project*          m_project;
    bool              m_componentComplete;
    QList<QString>    m_actSource;
    WorkerThread*     m_filterWorker;

};

}// namespace

#endif // LVWORKER_H
