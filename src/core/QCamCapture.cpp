#include "QCamCapture.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

class QCamCapturePrivate{

public:
    VideoCapture capture;
    QMat*        inactiveMat;

};

QCamCapture::QCamCapture(QQuickItem *parent) :
    QMatDisplay(parent),
    m_device(-1),
    d_ptr(new QCamCapturePrivate)
{
    d_ptr->inactiveMat = new QMat();
}

void QCamCapture::setDevice(int device){
    if ( m_device != device ){
        m_device = device;
        if ( m_device  != -1 ){
            d_ptr->capture.open(m_device);
            if ( d_ptr->capture.isOpened() ){
                // close thread if reading

                // set reading thread
            } else
                qDebug() << "Open CV error : Could not open capture : " << m_device;
        }
        emit deviceChanged();
    }
}


void QCamCapture::setPaused(bool paused){
    if ( m_paused != paused ){
        m_paused = paused;

        emit pausedChanged();
    }
}

QCamCapture::~QCamCapture(){
    delete d_ptr->inactiveMat;
    delete d_ptr;
}
