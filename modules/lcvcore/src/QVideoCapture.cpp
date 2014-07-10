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

QVideoCapture::QVideoCapture(QQuickItem *parent) :
    QQuickItem(parent),
    m_file(""),
    m_fps(0),
    m_thread(0),
    m_output(new QMat()),
    m_linearFilter(true),
    m_loop(false)
{
    m_restore = output();
    setFlag(ItemHasContents, true);
}

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

            setOutput(m_thread->output());
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

void QVideoCapture::setLoop(bool loop){
    if ( m_loop != loop ){
        m_loop = loop;
        if ( m_thread ){
            m_thread->setLoop(loop);
        }
        emit loopChanged();
    }
}

int QVideoCapture::totalFrames() const{
    if ( m_thread )
        return m_thread->totalFrames();
    return 0;
}

void QVideoCapture::setTotalFrames(int){
    // TODO : Remove this catastrophy
}

int QVideoCapture::currentFrame() const{
    if ( m_thread )
        return m_thread->currentFrame();
    return 0;
}

void QVideoCapture::seekTo(int frame){
    if ( frame != m_thread->currentFrame() ){
        m_thread->seekTo(frame);
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

QVideoCapture::~QVideoCapture(){
    QStateContainer<QVideoCaptureThread>& stateCont =
        QStateContainer<QVideoCaptureThread>::instance(this);
    m_thread = stateCont.state(m_file);
    if (m_thread != 0)
        disconnect( m_thread, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));
    setOutput(m_restore);
    delete m_output;
}
