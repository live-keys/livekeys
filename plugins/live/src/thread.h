#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QQmlListProperty>

namespace lv{

class Filter;
class FilterWorker;

/// \private
class Thread : public QObject{

    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> filters READ filters)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    explicit Thread(QObject *parent = nullptr);
    ~Thread();

    QQmlListProperty<QObject> filters();
    void appendFilter(QObject*);
    int filterCount() const;
    QObject *filter(int index) const;
    void clearFilters();

private:
    static void appendFilter(QQmlListProperty<QObject>*, QObject*);
    static int filterCount(QQmlListProperty<QObject>*);
    static QObject* filter(QQmlListProperty<QObject>*, int);
    static void clearFilters(QQmlListProperty<QObject>*);

private:
    QList<lv::Filter*> m_filters;
    FilterWorker*   m_filterWorker;

};

}// namespace

#endif // THREAD_H
