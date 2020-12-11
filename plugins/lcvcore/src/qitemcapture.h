#ifndef QITEMCAPTURE_H
#define QITEMCAPTURE_H

#include <QObject>
#include <QQmlParserStatus>
#include <QQuickItem>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>

#include "qmat.h"

/// \private
class QItemCapture : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQuickItem* captureSource READ captureSource WRITE setCaptureSource NOTIFY captureSourceChanged)
    Q_PROPERTY(QMat* result              READ result        NOTIFY resultChanged)

public:
    QItemCapture(QObject* parent = nullptr);
    ~QItemCapture();

    void setCaptureSource(QQuickItem* captureSource);
    QQuickItem* captureSource();

    QMat* result() const;

    void classBegin(){}
    void componentComplete();

public slots:
    void capture();

    void __windowRendered();
    void __itemWindowAttached(QQuickWindow* window);

signals:
    void ready();
    void captureSourceChanged();
    void resultChanged();

private:
    QQuickItem* m_captureSource;
    bool        m_scheduleCapture;
    QMat*       m_result;
    QOpenGLFunctions_3_3_Core* m_glFunctions;
};

inline QQuickItem *QItemCapture::captureSource(){
    return m_captureSource;
}

inline QMat *QItemCapture::result() const{
    return m_result;
}

#endif // QITEMCAPTURE_H
