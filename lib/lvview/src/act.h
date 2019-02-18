#ifndef LVACT_H
#define LVACT_H

#include "live/lvviewglobal.h"
#include "live/shared.h"
#include <functional>

#include <QObject>
#include <QVariant>
#include <QQmlParserStatus>

namespace lv{

class WorkerThread;

/// \private
class LV_VIEW_EXPORT Act : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QVariant result READ result WRITE setResult NOTIFY resultChanged)

public:
    Act(QObject* parent = nullptr);
    virtual ~Act();

    void setWorkerThread(WorkerThread* worker);
    WorkerThread* workerThread();

    bool isComponentComplete();

    const QVariant& result() const;

    virtual void process(){}

    void use(
        Shared::ReadScope* locker,
        const std::function<void()>& cb,
        const std::function<void()>& rs
    );

public slots:
    void setResult(const QVariant& result);

signals:
    void trigger();
    void complete();

    void resultChanged();

protected:
    void classBegin(){}
    void componentComplete();

private:
    bool          m_isComponentComplete;
    QVariant      m_result;
    WorkerThread* m_workerThread;
};

inline void Act::setWorkerThread(WorkerThread *worker){
    m_workerThread = worker;
}

inline WorkerThread *Act::workerThread(){
    return m_workerThread;
}

inline bool Act::isComponentComplete(){
    return m_isComponentComplete;
}

inline const QVariant &Act::result() const{
    return m_result;
}

inline void Act::setResult(const QVariant &result){
    if (m_result == result)
        return;

    m_result = result;
    emit resultChanged();
}

} // namespace

#endif // LVACT_H
