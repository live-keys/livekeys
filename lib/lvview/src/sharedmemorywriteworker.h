#ifndef LVSHAREDMEMORYWRITEWORKER_H
#define LVSHAREDMEMORYWRITEWORKER_H

#include "live/lvviewglobal.h"
#include <QObject>
#include <QSharedMemory>

namespace lv{

class LV_VIEW_EXPORT SharedMemoryWriteWorker : public QObject{

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
    explicit SharedMemoryWriteWorker(const QString& sharedMemoryKey, QObject *parent = 0);
    ~SharedMemoryWriteWorker();

    void start();
    void exit();
    void terminate();
    bool wait(unsigned long ms);
    void write(const QByteArray& message, int type, int id = 0);

    bool isReady() const;

    QString key() const;

signals:
    void statusUpdate(int status);
    void error(int code, const QString& message);
    void requestWrite(QByteArray mesage, int type, int id);

public slots:
    void onInitialize();
    void onFinish();
    void onRequestWrite(QByteArray message, int type, int id);

private:
    bool          m_isReady;
    QSharedMemory m_memory;
    QThread*      m_thread;
};

inline bool SharedMemoryWriteWorker::isReady() const{
    return m_isReady;
}

inline QString SharedMemoryWriteWorker::key() const{
    return m_memory.key();
}

}// namespace

#endif // LVSHAREDMEMORYWRITEWORKER_H
