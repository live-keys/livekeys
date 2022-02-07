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

#include "qvideocapture.h"
#include "qvideocapturethread.h"
#include "qmatnode.h"
#include "qmatshader.h"
#include "live/staticcontainer.h"
#include <QSGSimpleMaterial>

#include <QTimer>

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

int QVideoCapture::currentFrame() const{
    if ( m_thread )
        return m_thread->currentFrame();
    return 0;
}

void QVideoCapture::seekTo(int frame){
    if ( m_thread && frame != m_thread->currentFrame() ){
        m_thread->seekTo(frame);
    }
}

void QVideoCapture::staticLoad(const QString &file){
    if ( m_file == file )
        return;
    if ( !QFile::exists(file) ){
        qCritical("File does not exist: %s", qPrintable(file));
        return;
    }
    if (m_thread != 0)
        disconnect( m_thread, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));

    lv::StaticContainer* container = lv::StaticContainer::grabFromContext(this);
    m_thread = container->get<QVideoCaptureThread>(file);
    if ( !m_thread ){
        m_thread = new QVideoCaptureThread(file);
        container->set<QVideoCaptureThread>(file, m_thread);
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
        qCritical("Open CV Error: Could not open capture : %s", qPrintable(m_file));
}

void QVideoCapture::staticOpen(const QString &file){
    staticLoad(file);
}

void QVideoCapture::switchMat(){
    if ( m_thread ){
        m_output = m_thread->output();
        emit outChanged();
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

void QVideoCapture::componentComplete(){
    QQuickItem::componentComplete();
    emit init();
}

QVideoCapture::~QVideoCapture(){
    if (m_thread != 0)
        disconnect( m_thread, SIGNAL(inactiveMatChanged()), this, SLOT(switchMat()));
}
