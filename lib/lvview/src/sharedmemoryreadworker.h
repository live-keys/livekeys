#ifndef LVSHAREDMEMORYREADWORKER_H
#define LVSHAREDMEMORYREADWORKER_H

#include <QThread>
#include <QSharedMemory>

#include "live/linecapture.h"

namespace lv{

class LV_VIEW_EXPORT SharedMemoryReadWorker : public QThread{

    Q_OBJECT

public:
    enum Status{
        Initializing = 0,
        Attached = 1,
        Created = 2
    };
    Q_ENUMS(Status)

public:
    static const int SHARED_MEMORY_SIZE = 1000000;

public:
    SharedMemoryReadWorker(const QString& sharedMemoryKey, QObject* parent = nullptr);
    ~SharedMemoryReadWorker();

    static void receiveMessage(const LineMessage& message, void* data);

    QString key() const;

    bool isReady() const;

signals:
    void statusUpdate(int action);
    void message(const QByteArray& mesage, int type, int id);
    void error(int code, const QString& message);

protected:
    void run() Q_DECL_OVERRIDE;

private:
    void onMessage(const LineMessage& message);
    void readMemory();

    bool          m_isReady;
    QSharedMemory m_memory;
    LineCapture   m_lineCapture;
};

inline QString SharedMemoryReadWorker::key() const{
    return m_memory.key();
}

inline bool SharedMemoryReadWorker::isReady() const{
    return m_isReady;
}

}// namespace

#endif // LVSHAREDMEMORYREADWORKER_H
