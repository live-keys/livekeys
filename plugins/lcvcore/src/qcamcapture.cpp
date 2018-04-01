/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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
#include "qstaticcontainer.h"

#include <QTimer>

/*!
  \qmltype CamCapture
  \instantiates QCamCapture
  \inqmlmodule lcvcore
  \inherits MatDisplay
  \brief Captures frames from a connected camera. This is a \b {static item}.

  An example of how to use the cam capture can be found in samples/core/camcapture.qml :

  \quotefile core/camcapture.qml
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
  \qmlproperty string CamCapture::device

  This property holds the url to the camera device to be accessed.
 */

/*!
  \qmlproperty size CamCapture::resolution

  Stores the resolution for this cam capture.
 */

/*!
  \qmlproperty real CamCapture::fps

  By default, this is initialized with the camera's given fps. You can change this value if you want faster/slower
  capture playback, however, setting it faster than the actual camera permits will limit to the camera's maximum
  rate. This value is not absolute, in fact it depends on a lot of factors like processing time done by Live CV and
  speed by which frames are delivered.
 */
void QCamCapture::setFps(qreal fps){
    if ( m_thread && fps != m_fps ){
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

void QCamCapture::switchMat(){
    if ( m_thread ){
        setOutput(m_thread->output());
        m_thread->processNextFrame();
        update();
    }
}


/*!
  \qmlmethod CamCapture::staticOpen(string device, size resolution)

  This is an overloaded method for CamCapture::staticLoad
 */


/*!
  \qmlmethod CamCapture::staticLoad(string device, size resolution)

  Loads the CamCaptures state. \a device can be either a link or a device number. The device number should be given
  in string form. Usually a default webcam can be accesed by the '0'. \a resolution is optional and stores the
  resolution at which to open the capture.
 */

void QCamCapture::staticLoad(const QString &device, const QSize &resolution){
    if ( m_device == device || device == "" )
        return;
    if (m_thread != 0)
        disconnect( m_thread, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));

    QStaticContainer* container = QStaticContainer::grabFromContext(this);
    m_thread = container->get<QCamCaptureThread>(device);
    if ( !m_thread ){
        m_thread = new QCamCaptureThread(device);
        container->set<QCamCaptureThread>(device, m_thread);
    }

    setOutput(m_thread->output());
    connect(m_thread, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));

    if ( m_thread->isCaptureOpened() ){
        if ( resolution.isValid()){
            if ( m_thread->captureWidth() != resolution.width() || m_thread->captureHeight() != resolution.height() ){
                m_thread->setCaptureResolution(resolution.width(), resolution.height());
            }
        }
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

    emit deviceChanged();
}

void QCamCapture::componentComplete(){
    QQuickItem::componentComplete();
    emit init();
}

void QCamCapture::setPaused(bool paused){
    if ( !m_thread)
        return;

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
  \qmlproperty bool CamCapture::paused

  This property can be set to true or false, depending if you want to freeze or continue capturing frames from the camera.
 */

bool QCamCapture::paused() const{
    return m_thread->paused();
}

QCamCapture::~QCamCapture(){
    setOutput(m_restore);
}
