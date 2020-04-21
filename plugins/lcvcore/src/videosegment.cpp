#include "videosegment.h"

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

namespace lv{

VideoSegment::VideoSegment(QObject *parent)
    : Segment(parent)
    , m_surface(nullptr)
    , m_capture(new cv::VideoCapture)
{
}

void VideoSegment::openFile(){
    if ( m_file.isEmpty() )
        return;

    setLabel(m_file.mid(m_file.lastIndexOf('/') + 1));
    m_capture->open(m_file.toStdString());
}

void VideoSegment::cursorEnter(qint64 pos){
    if ( !m_surface )
        return;

    if ( pos == 0 ){
        if ( m_capture->grab() ){
            cv::Mat* frame = new cv::Mat;
            m_capture->retrieve(*frame);
            QMat* mat = new QMat(frame);
            m_surface->updateSurface(position() + pos, mat);
        }
    } else {
        m_capture->set(cv::CAP_PROP_POS_FRAMES, pos);
        if ( m_capture->grab() ){
            cv::Mat* frame = new cv::Mat;
            m_capture->retrieve(*frame);
            QMat* mat = new QMat(frame);
            m_surface->updateSurface(position() + pos, mat);
        }
    }
}

void VideoSegment::cursorExit(){
    if ( !m_surface )
        return;

    m_surface->output()->cvMat()->setTo(cv::Scalar(0));
    m_surface->update();

    //TODO: Will trigger surface reset
}

void VideoSegment::cursorNext(qint64 pos){
    if ( !m_surface )
        return;

    if ( m_capture->grab() ){
        cv::Mat* frame = new cv::Mat;
        m_capture->retrieve(*frame);
        QMat* mat = new QMat(frame);
        m_surface->updateSurface(position() + pos, mat);
    }
}

void VideoSegment::cursorMove(qint64 position){
    if ( !m_surface )
        return;

    m_capture->set(cv::CAP_PROP_POS_FRAMES, position);
    cv::Mat frame;
    if ( m_capture->grab() ){
        qDebug() << " RETRIEVE: " << m_capture->retrieve(frame);
        frame.copyTo(*m_surface->output()->cvMat());
        m_surface->update();
    }
    qDebug() << "MOVE:" << position;
}

}// namespace
