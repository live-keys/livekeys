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
