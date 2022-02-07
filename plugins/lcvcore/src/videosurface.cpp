/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "videosurface.h"
#include "cvextras.h"
#include <QQmlEngine>

#include "live/stacktrace.h"

namespace lv{

VideoSurface::VideoSurface(QObject*parent)
    : TimelineObjectProperty(parent)
    , m_lastProcessedPosition(-1)
    , m_imageWidth(0)
    , m_imageHeight(0)
    , m_currentPosition(-1)
    , m_buffer(nullptr)
    , m_current(nullptr)
    , m_writer(nullptr)
{
}

VideoSurface::~VideoSurface(){
    Shared::unref(m_current);
}

void VideoSurface::swapSurface(qint64 position){
    if ( m_currentPosition != position ){
        m_currentPosition = position;
        if ( !m_buffer ){
            m_buffer = new QMat(m_imageWidth, m_imageHeight, QMat::CV8U, 3);
            m_buffer->internal().setTo(cv::Scalar(0));
        }
        m_current = m_buffer;
        if ( m_writer )
            m_writer->write(m_buffer);

        m_buffer = nullptr;
        emit imageChanged();
    }
}

void VideoSurface::updateSurface(qint64 cursorPosition, QMat *frame){
    if ( m_imageWidth <= 0 || m_imageHeight <= 0 )
        return;

    m_lastProcessedPosition = cursorPosition;
    if ( !m_buffer ){
        m_buffer = new QMat(m_imageWidth, m_imageHeight, QMat::CV8U, 3);
        m_buffer->internal().setTo(cv::Scalar(0));
        Shared::ref(m_buffer);
    }

    if ( frame && frame->internal().cols > 0 && frame->internal().rows > 0){
        m_currentPosition = true;
        int frameWidth = frame->internal().cols;
        int frameHeight = frame->internal().rows;
        int bufferWidth = m_buffer->internal().cols;
        int bufferHeight = m_buffer->internal().rows;

        if ( frame->internal().cols == m_buffer->internal().cols &&
             frame->internal().rows == m_buffer->internal().rows )
        {
            CvExtras::blendTo3Channels(frame->internal(), m_buffer->internal());
        } else {
            int bufferRoiWidth, bufferRoiHeight, frameRoiWidth, frameRoiHeight;
            if ( bufferWidth > frameWidth ){
                bufferRoiWidth = frameWidth;
                frameRoiWidth  = frameWidth;
            } else {
                bufferRoiWidth = bufferWidth;
                frameRoiWidth  = bufferWidth;
            }
            if  ( bufferHeight > frameHeight ){
                bufferRoiHeight = frameHeight;
                frameRoiHeight = frameHeight;
            } else {
                bufferRoiHeight = bufferHeight;
                frameRoiHeight = bufferHeight;
            }

            cv::Mat frameRoi = frame->internal()(cv::Rect(0, 0, frameRoiWidth, frameRoiHeight));
            cv::Mat bufferRoi = m_buffer->internal()(cv::Rect(0, 0, bufferRoiWidth, bufferRoiHeight));

            CvExtras::blendTo3Channels(frameRoi, bufferRoi);
        }
    }
    m_currentPosition = true;
}

void VideoSurface::serialize(ViewEngine *, MLNode &node){
    node["width"]   = m_imageWidth;
    node["height"]  = m_imageHeight;
    node["type"]    = "lcvcore#VideoSurface";
    node["factory"] = "lcvcore/VideoSurfaceFactory.qml";
}

void VideoSurface::deserialize(ViewEngine *, const MLNode &node){
    m_imageWidth  = node["width"].asInt();
    m_imageHeight = node["height"].asInt();
}

void VideoSurface::createRecorder(){
    if ( m_writer && m_writer->framesWritten() > 0 ){
        delete m_writer;
        m_writer = nullptr;
    }

    if ( !m_writer ){
        m_writer = new QVideoWriter(this);

        if ( !m_writerOptions.isObject() ){
            THROW_EXCEPTION(lv::Exception, "VideoSurface: VideoWriter options have not been specified.", lv::Exception::toCode("~Options"));
        }

        m_writerOptions.setProperty("frameWidth", m_imageWidth);
        m_writerOptions.setProperty("frameHeight", m_imageHeight);
        m_writer->setOptions(m_writerOptions);
    }
}

void VideoSurface::closeRecorder(){
    if ( m_writer ){
        m_writer->save();
        m_writer = nullptr;
//        delete m_writer;
//        m_writer = nullptr;
    }
}

void VideoSurface::setWriterOptions(const QJSValue &options){
    m_writerOptions = options;
}

}// namespace
