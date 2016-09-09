/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "qcamcapture.h"
#include "qcamcapturethread.h"
#include "qstatecontainer.h"

#include <QTimer>

/*!
  \qmltype CamCapture
  \instantiates QCamCapture
  \inqmlmodule lcvcore
  \inherits MatDisplay
  \brief Captures frames from a connected camera

  An example of how to use the cam capture can be found in samples/core/camcapture.qml :

  \quotefile core/camcapture.qml
*/

/*!
   \class QCamCapture
   \inmodule lcvcore_cpp
   \brief Captures frames from a connected camera.
 */

/*!
 * \brief QCamCapture::QCamCapture
 * \a parent
 */
QCamCapture::QCamCapture(QQuickItem *parent) :
    QMatDisplay(parent),
    m_device(""),
    m_fps(25),
    m_resolution(0, 0),
    m_thread(0)
{
    m_restore = output();
}

/*!
  \property QCamCapture::device
  \sa CamCapture::device
 */

/*!
  \qmlproperty string CamCapture::device

  This property holds the url to the camera device to be accessed. It can be either a link or a device number.
  The device number should be given in string form. Ussually, an installed webcam can be accessed by setting
  the device to '0'.
 */
void QCamCapture::setDevice(const QString &device){
    if ( m_device != device ){
        m_device = device;
        if ( m_device  != "" ){

            QStateContainer<QCamCaptureThread>& stateCont =
                    QStateContainer<QCamCaptureThread>::instance(this);

            m_thread = stateCont.state(m_device);
            if ( m_thread == 0 ){
                m_thread = new QCamCaptureThread(device);
                stateCont.registerState(m_device, m_thread);
            }

            setOutput(m_thread->output());
            connect( m_thread, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));

            if ( m_thread->isCaptureOpened() ){
                m_resolution.setWidth(m_thread->captureWidth());
                m_resolution.setHeight(m_thread->captureHeight());
                setImplicitWidth (m_thread->captureWidth());
                setImplicitHeight(m_thread->captureHeight());

                if ( !m_thread->paused() ){
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
                qWarning("Open CV Error : Could not open capture : %s", qPrintable(m_device));
        }
        emit deviceChanged();
    }
}

/*!
  \property QCamCapture::fps
  \sa CamCapture::fps
 */

/*!
  \qmlproperty real CamCapture::fps

  By default, this is initialized with the camera's given fps. You can change this value if you want faster/slower
  capture playback, however, setting it faster than the actual camera permits will limit to the camera's maximum
  rate. This value is not absolute, in fact it depends on a lot of factors like processing time done by Live CV and
  speed by which frames are delivered.
 */
void QCamCapture::setFps(qreal fps){
    if ( fps != m_fps ){
        m_fps = fps;
        if ( m_thread ){
            if ( !m_thread->paused() && m_thread->isCaptureOpened() ){
                if ( m_thread->timer()->isActive() )
                    m_thread->timer()->stop();
                m_thread->timer()->start(1000 / m_fps);
            }
        }
        emit fpsChanged();
    }
}

void QCamCapture::reinitializeDevice(){
    bool isPaused = paused();
    QString device(m_device);
    QStateContainer<QCamCaptureThread>& stateCont =
            QStateContainer<QCamCaptureThread>::instance(this);

    delete m_thread;
    m_thread = 0;
    stateCont.registerState(m_device, m_thread);
    m_device = "";
    setDevice(device);
    setPaused(isPaused);
}

/*!
  \property QCamCapture::resolution
  \sa CamCapture::resolution
 */

/*!
  \qmlproperty size CamCapture::resolution

  By default, this is auto-detected based on the first received frame.
  Can be overridden if your camera supports resolutions other than its default resolution.

  Example with custom resolution set:
  \code
  CamCapture{
      device: '0'
      resolution: Qt.size(1280,720)
  }
  \endcode
 */
void QCamCapture::setResolution(const QSize& resolution){
    if ( resolution != m_resolution ){
        reinitializeDevice();
        m_thread->setCaptureResolution(resolution.width(), resolution.height());
        m_resolution.setWidth(m_thread->captureWidth());
        m_resolution.setHeight(m_thread->captureHeight());
        setImplicitWidth(m_resolution.width());
        setImplicitHeight(m_resolution.height());
        if ( resolution != m_resolution )
            qWarning() << "Attempted to set resolution to" << resolution
                       << "but OpenCV set it to" << m_resolution << "instead. "
                       << "Does the camera support the target resolution?";
        emit resolutionChanged();
    }
}

/*!
   \brief Switches buffers with it's associated thread
 */
void QCamCapture::switchMat(){
    if ( m_thread ){
        setOutput(m_thread->output());
        m_thread->processNextFrame();
        update();
	}
}


void QCamCapture::setPaused(bool paused){
    if ( m_thread->paused() != paused ){
        m_thread->setPaused(paused);
        if ( paused ){
            m_thread->timer()->stop();
        } else
            m_thread->timer()->start(1000 / m_fps);
        emit pausedChanged();
    }
}

/*!
  \property QCamCapture::paused
  \sa CamCapture::paused
 */

/*!
  \qmlproperty bool CamCapture::paused

  This property can be set to true or false, depending if you want to freeze or continue capturing frames from the camera.
 */
bool QCamCapture::paused() const{
    return m_thread->paused();
}

/*!
  \brief QCamCapture destructor.
 */
QCamCapture::~QCamCapture(){
    QStateContainer<QCamCaptureThread>& stateCont =
            QStateContainer<QCamCaptureThread>::instance(this);
    m_thread = stateCont.state(m_device);
    if (m_thread != 0)
        disconnect( m_thread, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));
    setOutput(m_restore);
}
