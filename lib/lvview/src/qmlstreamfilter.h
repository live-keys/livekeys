#ifndef LVQMLSTREAMFILTER_H
#define LVQMLSTREAMFILTER_H

#include <QObject>
#include <QJSValue>
#include "live/qmlstream.h"

namespace lv{

class WorkerThread;

/// \private
class LV_VIEW_EXPORT QmlStreamFilter : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::QmlStream* pull   READ pull   WRITE setPull NOTIFY pullChanged)
    Q_PROPERTY(QJSValue run          READ run    WRITE setRun  NOTIFY runChanged)
    Q_PROPERTY(lv::QmlStream* result READ result NOTIFY resultChanged)

public:
    QmlStreamFilter(QObject* parent = nullptr);
    ~QmlStreamFilter();

    lv::QmlStream* pull() const;
    const QJSValue& run() const;
    lv::QmlStream* result() const;

    static void streamHandler(QObject* that, const QJSValue& val);

    void setPull(lv::QmlStream* pull);
    void setRun(const QJSValue& run);

    void setWorkerThread(WorkerThread* worker);
    WorkerThread* workerThread();

    void triggerRun();
    void triggerRun(const QJSValue& arg);

    void pushResult(const QVariant& v);

signals:
    void pullChanged();
    void runChanged();
    void resultChanged();

private:
    lv::QmlStream* m_pull;
    QJSValue       m_run;
    lv::QmlStream* m_result;

    QJSValue       m_lastValue;
    WorkerThread*  m_workerThread;
};

inline QmlStream *QmlStreamFilter::pull() const{
    return m_pull;
}

inline const QJSValue& QmlStreamFilter::run() const{
    return m_run;
}

inline QmlStream *QmlStreamFilter::result() const{
    return m_result;
}

inline void QmlStreamFilter::setWorkerThread(WorkerThread *worker){
    m_workerThread = worker;
}

inline WorkerThread *QmlStreamFilter::workerThread(){
    return m_workerThread;
}

}// namespace

#endif // LVQMLSTREAMFILTER_H
