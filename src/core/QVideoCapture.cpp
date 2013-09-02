#include "QVideoCapture.hpp"
#include "QCaptureContainer.hpp"
#include "QVideoCaptureThread.hpp"

#include <QTimer>

QVideoCapture::QVideoCapture(QQuickItem *parent) :
    QMatDisplay(parent),
    m_file(""),
    m_fps(0),
    m_paused(false),
    m_thread(0)
{
    m_restore = output();
}

void QVideoCapture::setFile(const QString &file){
    if ( m_file != file ){
        m_file = file;
        if ( QFile::exists(file) ){

            m_thread = QCaptureContainer::instance()->captureThread(m_file);
            setOutput(m_thread->output());
            connect( m_thread, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));

            if ( m_thread->isCaptureOpened() ){
                setImplicitWidth (m_thread->captureWidth());
                setImplicitHeight(m_thread->captureHeight());
                if ( m_fps == 0 )
                    m_fps = m_thread->captureFps();

                if ( !m_paused ){
                    if ( m_thread->timer()->isActive() ){
                        if ( m_thread->timer()->interval() != (1000 / m_fps) ){
                            m_thread->timer()->stop();
                            m_thread->timer()->start(1000 / m_fps);
                        }
                    } else {
                        m_thread->timer()->start(1000 / m_fps);
                    }
                }

            } else
                qDebug() << "Open CV error : Could not open capture : " << m_file;
        }
        emit fileChanged();
    }
}

void QVideoCapture::setFps(qreal fps){
    if ( fps != m_fps ){
        m_fps = fps;
        if ( m_thread ){
            if ( !m_paused && m_thread->isCaptureOpened() ){
                if ( m_thread->timer()->isActive() )
                    m_thread->timer()->stop();
                m_thread->timer()->start(1000 / m_fps);
            }
        }
        emit fpsChanged();
    }
}

void QVideoCapture::switchMat(){
    if ( m_thread ){
        setOutput(m_thread->output());
        m_thread->processNextFrame();
        update();
    }
}

void QVideoCapture::setPaused(bool paused){
    if ( m_paused != paused ){
        m_paused = paused;
        if ( m_paused ){
            m_thread->timer()->stop();
        } else
            m_thread->timer()->start(1000 / m_fps);
        emit pausedChanged();
    }
}

QVideoCapture::~QVideoCapture(){
    disconnect( m_thread, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));
    setOutput(m_restore);
}
