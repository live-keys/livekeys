/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#include "QVideoCapture.hpp"
#include "QVideoCaptureThread.hpp"
#include "QMatNode.hpp"
#include "QMatShader.hpp"
#include "QStateContainer.hpp"
#include <QSGSimpleMaterial>

#include <QTimer>

/*!
  \qmltype VideoCapture
  \instantiates QVideoCapture
  \inqmlmodule lcvcore
  \inherits Item
  \brief Captures frames from video files.

  The VideoCapture constantly grabes frames from a video file. The frames are captured at a speed equal to the video's
  fps, but that is not necessarily to be considered as an absolute value. The speed can be altered manually by
  configuring the fps parameter. A progress bar and a play/pause button can be attached by using the VideoControls type
  in the lcvcontrols module.

  The first example in core/videocapture_simple.qml shows a simple video frame grabber, while the second one in
  core/videocapture_controls.qml shows the grabber with the VideoControls attached.

  \quotefile core/videocapture_simple.qml

  \quotefile core/videocapture_controls.qml
*/

/*!
   \class QVideoCapture
   \inmodule lcvcore_cpp
   \brief Captures frames from a video file.
 */

/*!
  \brief QVideoCapture constructor

  Parameters:
  \a parent
 */
QVideoCapture::QVideoCapture(QQuickItem *parent)
    : QQuickItem(parent)
    , m_file("")
    , m_fps(0)
    , m_output(QMat::nullMat())
    , m_linearFilter(true)
    , m_loop(false)
    , m_thread(0)
{
    setFlag(ItemHasContents, true);
}


/*!
  \property QVideoCapture::file
  \sa VideoCapture::file
 */

/*!
  \qmlproperty string VideoCapture::file

  The path to the video file to open.
 */
void QVideoCapture::setFile(const QString &file){
    if ( m_file != file ){
        m_file = file;
        if ( QFile::exists(file) ){

            QStateContainer<QVideoCaptureThread>& stateCont =
                    QStateContainer<QVideoCaptureThread>::instance(this);

            m_thread = stateCont.state(m_file);
            if ( m_thread == 0 ){
                m_thread = new QVideoCaptureThread(m_file);
                stateCont.registerState(m_file, m_thread);
            }

            m_output = m_thread->output();
            emit outChanged();

            connect( m_thread, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));

            if ( m_thread->isCaptureOpened() ){

                setImplicitWidth (m_thread->captureWidth());
                setImplicitHeight(m_thread->captureHeight());

                m_thread->setLoop(m_loop);
                if ( m_fps == 0 )
                    m_fps = m_thread->captureFps();

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
                emit pausedChanged();
                emit totalFramesChanged();

            } else
                qDebug() << "Open CV error : Could not open capture : " << m_file;
        } else {
            qDebug() << "File does not exist : " << file;
        }
        emit fileChanged();
    }
}

/*!
  \property QVideoCapture::output
  \sa VideoCapture::output
 */

/*!
  \qmlproperty Mat VideoCapture::output

  Output frame.
 */


/*!
  \property QVideoCapture::linearFilter
  \sa VideoCapture::linearFilter
 */

/*!
  \qmlproperty bool VideoCapture::linearFilter

  Perform linear filtering when scaling the matrix to be displayed. The default value is true.
 */


/*!
  \property QVideoCapture::fps
  \sa VideoCapture::fps
 */

/*!
  \qmlproperty float VideoCapture::fps

  By default, this is initialized with the video files fps. You can change this value if you want faster/slower
  capture playback.
 */

void QVideoCapture::setFps(qreal fps){
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


/*!
  \property QVideoCapture::loop
  \sa VideoCapture::loop
 */

/*!
  \qmlproperty bool VideoCapture::loop

  If enabled, the video will start over once it's reach the end. By default, this value is disabled.
 */

void QVideoCapture::setLoop(bool loop){
    if ( m_loop != loop ){
        m_loop = loop;
        if ( m_thread ){
            m_thread->setLoop(loop);
        }
        emit loopChanged();
    }
}

/*!
  \property QVideoCapture::totalFrames
  \sa VideoCapture::totalFrames
 */

/*!
  \qmlproperty int VideoCapture::totalFrames

  This \b{read-only} property holds the total frames available in the video.
 */

int QVideoCapture::totalFrames() const{
    if ( m_thread )
        return m_thread->totalFrames();
    return 0;
}

/*!
  \property QVideoCapture::currentFrame
  \sa VideoCapture::currentFrame
 */

/*!
  \qmlproperty int VideoCapture::currentFrame

  This property holds the current frame number. If you set this manually, you perform a seek to the specified frame
  number in the video.
 */


int QVideoCapture::currentFrame() const{
    if ( m_thread )
        return m_thread->currentFrame();
    return 0;
}

/*!
  \qmlmethod VideoCapture::seek(int frame)

  Perform a seek to the specified \a frame number.
 */

void QVideoCapture::seekTo(int frame){
    if ( frame != m_thread->currentFrame() ){
        m_thread->seekTo(frame);
    }
}

/*!
  \brief Internal function used to perform a video swap with the video capture thread.
 */
void QVideoCapture::switchMat(){
    if ( m_thread ){
        m_output = m_thread->output();
        emit outChanged();
        m_thread->processNextFrame();
        update();
    }
}

/*!
  \property QVideoCapture::paused
  \sa VideoCapture::paused
 */

/*!
  \qmlproperty bool VideoCapture::paused

  Pause / play the video by setting this property to true or false.
 */

void QVideoCapture::setPaused(bool paused){
    if ( m_thread ){
        if ( m_thread->paused() != paused ){
            m_thread->setPaused(paused);
            if ( paused ){
                m_thread->timer()->stop();
            } else
                m_thread->timer()->start(1000 / m_fps);
            emit pausedChanged();
        }
    }
}

bool QVideoCapture::paused() const{
    if ( m_thread )
        return m_thread->paused();
    return false;
}

/*!
  \fn virtual QSGNode* QVideoCapture::updatePaintNode(QSGNode*, UpdatePaintNodeData*)

  \brief Updates the scene graph node with the frames matrix.

  Parameters :
  \a node
  \a nodeData
 */

QSGNode *QVideoCapture::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *){
    QMatNode *n = static_cast<QMatNode*>(node);
    if (!node)
        n = new QMatNode();

    QSGGeometry::updateTexturedRectGeometry(n->geometry(), boundingRect(), QRectF(0, 0, 1, 1));
    QMatState* state = static_cast<QSGSimpleMaterial<QMatState>*>(n->material())->state();
    state->mat          = m_output;
    state->textureSync  = false;
    state->linearFilter = m_linearFilter;
    n->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

    return n;
}

/*!
  \brief QVideoCapture destructor
 */
QVideoCapture::~QVideoCapture(){
    QStateContainer<QVideoCaptureThread>& stateCont =
        QStateContainer<QVideoCaptureThread>::instance(this);
    m_thread = stateCont.state(m_file);
    if (m_thread != 0)
        disconnect( m_thread, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));
}
