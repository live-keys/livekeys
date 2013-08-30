#ifndef QCAPTURECONTAINER_HPP
#define QCAPTURECONTAINER_HPP

#include <QObject>
#include "QCamCapture.hpp"

/**
 * @brief The QCaptureContainer class
 * @todo Add support for QQuickWindow::afterRendering()
 */
class QCaptureContainer : public QObject{

    Q_OBJECT

public:
    ~QCaptureContainer();

    QCamCaptureThread* findCapture(int device);

    static QCamCaptureThread *instance() const;
    static void destroyInstance();
    
signals:
    
public slots:

private:
    static QCamCaptureThread* m_instance;

    explicit QCaptureContainer(QObject *parent = 0);
    QCaptureContainer(const QCaptureContainer& other);
    QCaptureContainer& operator= (const QCaptureContainer& other);

    QList<QCamCaptureThread*> m_captureThreads;
    
};

inline QCamCaptureThread *QCaptureContainer::instance() const{
    if ( m_instance == 0 ){
        m_instance = new QCamCaptureThread();
        std::atexit(&destroyInstance);
    }
    return m_instance;
}

void QCaptureContainer::destroyInstance(){
    delete m_instance;
}


#endif // QCAPTURECONTAINER_HPP
