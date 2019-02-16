#ifndef QITEMCAPTURE_H
#define QITEMCAPTURE_H

#include "qmatdisplay.h"
#include <QQuickItem>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLContext>

/// \private
class QItemCapture : public QMatDisplay{

    Q_OBJECT
    Q_PROPERTY(QQuickItem* captureSource READ captureSource WRITE setCaptureSource NOTIFY captureSourceChanged)

public:
    QItemCapture(QQuickItem* parent = 0);
    ~QItemCapture();

    void setCaptureSource(QQuickItem* captureSource);
    QQuickItem* captureSource();

public slots:
    void capture();
    void captureReady(QSize size);

    void windowAttached(QQuickWindow* window);
    void grabImage();

signals:
    void captureIsReady(QSize size);
    void captureSourceChanged();

private:
    QOpenGLFunctions_3_3_Core* m_glFunctions;

    bool        m_scheduledCapture;
    QQuickItem* m_captureSource;
};

inline QQuickItem *QItemCapture::captureSource(){
    return m_captureSource;
}

#endif // QITEMCAPTURE_H
