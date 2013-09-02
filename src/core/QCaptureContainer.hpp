#ifndef QCAPTURECONTAINER_HPP
#define QCAPTURECONTAINER_HPP

#include <QObject>

class QCamCaptureThread;
class QVideoCaptureThread;

/**
 * @brief The QCaptureContainer class
 * @todo Add support for QQuickWindow::afterRendering()
 */
class QCaptureContainer : public QObject{

    Q_OBJECT

public:
    ~QCaptureContainer();

    QCamCaptureThread*   captureThread(int device);
    QVideoCaptureThread* captureThread(const QString& file);

    static QCaptureContainer *instance();
    static void destroyInstance();
    
signals:
    
public slots:

private:
    static QCaptureContainer* m_instance;

    explicit QCaptureContainer(QObject *parent = 0);
    QCaptureContainer(const QCaptureContainer& other);
    QCaptureContainer& operator= (const QCaptureContainer& other);

    QList<QCamCaptureThread*>   m_camCaptureThreads;
    QList<QVideoCaptureThread*> m_videoCaptureThreads;
    
};

inline QCaptureContainer *QCaptureContainer::instance(){
    if ( m_instance == 0 ){
        m_instance = new QCaptureContainer();
        std::atexit(&destroyInstance);
    }
    return m_instance;
}

inline void QCaptureContainer::destroyInstance(){
    delete m_instance;
}


#endif // QCAPTURECONTAINER_HPP
