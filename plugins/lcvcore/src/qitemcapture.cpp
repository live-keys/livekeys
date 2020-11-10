#include "qitemcapture.h"
#include <QSGTextureProvider>
#include <QQuickWindow>

QItemCapture::QItemCapture(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_glFunctions(0)
    , m_scheduledCapture(false)
    , m_captureSource(0)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(windowAttached(QQuickWindow*)));
    connect(this, SIGNAL(captureIsReady(QSize)), this, SLOT(captureReady(QSize)));
}

QItemCapture::~QItemCapture(){
    delete m_glFunctions;
}

void QItemCapture::windowAttached(QQuickWindow *window){
    disconnect(this, SLOT(grabImage()));
    if ( m_scheduledCapture && m_captureSource ){
        connect(window, SIGNAL(afterRendering()), this, SLOT(grabImage()), Qt::DirectConnection);
        m_scheduledCapture = false;
    }
}

void QItemCapture::grabImage(){
    QQuickItem* cs = m_captureSource; // save this since we're in a different thread
    if ( !cs )
        return;
    if ( !cs->textureProvider() )
        return;

    if ( !m_glFunctions ){
        m_glFunctions = new QOpenGLFunctions_3_3_Core;
        m_glFunctions->initializeOpenGLFunctions();
    }

    disconnect(window(), SIGNAL(afterRendering()), this, SLOT(grabImage()));

    QSGTexture *texture = cs->textureProvider()->texture();
    texture->bind();

    GLint internalFormat;
    m_glFunctions->glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPONENTS, &internalFormat);

    QSize ts = texture->textureSize();
    if ( ts.isValid() ){
        output()->internalPtr()->create(cv::Size(ts.width(), ts.height()), CV_8UC4);
        m_glFunctions->glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, output()->internalPtr()->data);
        emit captureIsReady(ts);
    }
}

void QItemCapture::setCaptureSource(QQuickItem *captureSource){
    if (m_captureSource == captureSource)
        return;

    m_captureSource = captureSource;
    emit captureSourceChanged();

    if ( m_scheduledCapture && m_captureSource ){
        connect(window(), SIGNAL(afterRendering()), this, SLOT(grabImage()), Qt::DirectConnection);
        m_scheduledCapture = false;
    }
}

void QItemCapture::capture(){
    if ( window() && m_captureSource ){
        connect(window(), SIGNAL(afterRendering()), this, SLOT(grabImage()), Qt::DirectConnection);
    } else {
        m_scheduledCapture = true;
    }
}

void QItemCapture::captureReady(QSize size){
    if ( size.width() != implicitWidth() )
        setImplicitWidth(size.width());
    if ( size.height() != implicitHeight() )
        setImplicitHeight(size.height());
    emit outputChanged();
    update();
}
