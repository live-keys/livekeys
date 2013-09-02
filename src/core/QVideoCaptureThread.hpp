#ifndef QVideoCaptureTHREAD_HPP
#define QVideoCaptureTHREAD_HPP

#include <QThread>

class QMat;
class QTimer;
class QVideoCaptureThreadPrivate;
class QVideoCaptureThread: public QThread{

    Q_OBJECT

public:
    QVideoCaptureThread(const QString& file, QObject* parent = 0);
    ~QVideoCaptureThread();

    QMat*   output();
    QTimer* timer();
    const QString& file() const;
    int     captureWidth() const;
    int     captureHeight() const;
    double  captureFps() const;

    bool    isCaptureOpened();

    void    processNextFrame();

signals:
    void inactiveMatChanged();

public slots:
    void tick();

protected:
    void run();

private:
    void initializeMatSize();

    QVideoCaptureThread(const QVideoCaptureThread& other);
    QVideoCaptureThread& operator= (const QVideoCaptureThread& other);

    QString m_file;
    QMat*   m_activeMat;
    QTimer* m_timer;

    QVideoCaptureThreadPrivate* const d_ptr;

    Q_DECLARE_PRIVATE(QVideoCaptureThread)

};

inline QTimer *QVideoCaptureThread::timer(){
    return m_timer;
}

inline const QString& QVideoCaptureThread::file() const{
    return m_file;
}

inline QMat *QVideoCaptureThread::output(){
    return m_activeMat;
}

#endif // QVideoCaptureTHREAD_HPP
