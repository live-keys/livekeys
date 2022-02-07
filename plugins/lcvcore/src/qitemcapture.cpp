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

#include "qitemcapture.h"

#include <QSGTextureProvider>
#include <QQuickWindow>
#include <QThread>

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

#include "qmatio.h"

QItemCapture::QItemCapture(QObject *parent)
    : QObject(parent)
    , m_captureSource(nullptr)
    , m_scheduleCapture(false)
    , m_result(nullptr)
    , m_glFunctions(nullptr)
{
}

QItemCapture::~QItemCapture(){
    lv::Shared::unref(m_result);
    delete m_glFunctions;
}

void QItemCapture::setCaptureSource(QQuickItem *captureSource){
    if (m_captureSource == captureSource)
        return;

    if ( m_scheduleCapture ){
        m_scheduleCapture = false;
        disconnect(m_captureSource, &QQuickItem::windowChanged, this, &QItemCapture::__itemWindowAttached);
        if ( m_captureSource->window() ){
            disconnect(m_captureSource->window(), &QQuickWindow::afterRendering, this, &QItemCapture::__windowRendered);
        }
    }

    if ( captureSource && !captureSource->isTextureProvider() ){
        captureSource->property("layer").value<QObject*>()->setProperty("enabled", true);
    }

    m_captureSource = captureSource;

    if ( m_captureSource->window() ){
        __itemWindowAttached(m_captureSource->window());
    } else {
        connect(m_captureSource, &QQuickItem::windowChanged, this, &QItemCapture::__itemWindowAttached);
    }

    emit captureSourceChanged();
}

void QItemCapture::componentComplete(){
    emit ready();
}

void QItemCapture::capture(){
    if ( !m_captureSource ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "No capture source provided.", lv::Exception::toCode("~source"));
        lv::ViewEngine* ve = lv::ViewEngine::grab(this);
        lv::QmlError(ve, e, this).jsThrow();
        return;
    }
    if ( m_scheduleCapture )
        return;

    m_scheduleCapture = true;
}

void QItemCapture::__itemWindowAttached(QQuickWindow* window){
    if ( window )
        connect(window, &QQuickWindow::afterRendering, this, &QItemCapture::__windowRendered, Qt::DirectConnection);
}

void QItemCapture::__windowRendered(){
    if ( !m_scheduleCapture )
        return;

    m_scheduleCapture = false;

    QQuickItem* cs = m_captureSource; // save this since we're in a different thread
    if ( !cs )
        return;

    if ( !cs->textureProvider() )
        return;

    if ( !m_glFunctions ){
        m_glFunctions = new QOpenGLFunctions_3_3_Core;
        m_glFunctions->initializeOpenGLFunctions();
    }

    QSGTexture *texture = cs->textureProvider()->texture();
    texture->bind();

    GLint internalFormat;
    m_glFunctions->glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPONENTS, &internalFormat);

    QSize ts = texture->textureSize();
    if ( ts.isValid() ){
        QMat* down = new QMat(new cv::Mat(cv::Size(ts.width(), ts.height()), CV_8UC4));
        m_glFunctions->glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, down->internal().data);
        down->moveToThread(thread());

        lv::Shared::ref(down);
        m_result = down;
        emit resultChanged();
    }
}
