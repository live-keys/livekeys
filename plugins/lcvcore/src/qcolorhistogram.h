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

#ifndef QCOLORHISTOGRAM_H
#define QCOLORHISTOGRAM_H

#include <QQuickItem>
#include <QSGSimpleTextureNode>
#include "qmat.h"

class QPainter;
class QOpenGLPaintDevice;
class QOpenGLFramebufferObject;
class QOpenGLFunctions;

/// \private
class QAbstractColorHistogramRenderer{
public:
    QAbstractColorHistogramRenderer(){}
    virtual ~QAbstractColorHistogramRenderer(){}

    virtual void renderSingleList(
        QPainter* painter,
        const QSize &size,
        const ushort* values,
        size_t valuesSize,
        qreal maxValue,
        const QColor& color
    ) = 0;
};

/// \private
class QColorHistogramNode : public QObject, public QSGSimpleTextureNode{

    Q_OBJECT

public:
    QColorHistogramNode(QQuickWindow* window);
    ~QColorHistogramNode();

    void render(
        int channel,
        const cv::Mat& values,
        qreal maxValue,
        QAbstractColorHistogramRenderer* renderer
    );

private:
    QOpenGLFramebufferObject *m_fbo;
    QSGTexture               *m_texture;
    QQuickWindow             *m_window;
    QPainter                 *m_painter;
    QOpenGLPaintDevice       *m_paintDevice;
    QOpenGLFunctions         *m_glFunctions;
};
/// \private
class QColorHistogram : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat* input  READ input   WRITE  setInput   NOTIFY inputChanged)
    Q_PROPERTY(QMat* output READ output  NOTIFY outputChanged)
    Q_PROPERTY(bool  fill   READ fill    WRITE  setFill    NOTIFY fillChanged)
    Q_PROPERTY(int channel  READ channel WRITE  setChannel NOTIFY channelChanged)
    Q_ENUMS(Selection)

public:
    enum Selection{
        Total = -2,
        AllChannels = -1,
        BlueChannel = 0,
        GreenChannel = 1,
        RedChannel = 2
    };

public:
    QColorHistogram(QQuickItem* parent = 0);
    ~QColorHistogram();

    QMat* input() const;
    QMat* output() const;
    bool fill() const;
    int channel() const;

    void setInput(QMat* input);
    void setFill(bool fill);
    void setChannel(int channel);

signals:
    void inputChanged();
    void fillChanged();
    void channelChanged();
    void outputChanged();

protected:
    void componentComplete();
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *nodeData);

private:
    QColorHistogram(const QColorHistogram&);
    QColorHistogram& operator = (const QColorHistogram& other);

    void createHistogram();

    QMat* m_input;
    QMat* m_output;
    bool  m_fill;
    int   m_channel;

    QAbstractColorHistogramRenderer* m_renderer;
};

inline QMat *QColorHistogram::input() const{
    return m_input;
}

inline bool QColorHistogram::fill() const{
    return m_fill;
}

inline int QColorHistogram::channel() const{
    return m_channel;
}

inline QMat *QColorHistogram::output() const{
    return m_output;
}

inline void QColorHistogram::setInput(QMat *input){
    if ( input != nullptr ){
        m_input = input;
        emit inputChanged();
        createHistogram();
    }
}

inline void QColorHistogram::setChannel(int channel){
    if (m_channel == channel)
        return;

    m_channel = channel;
    emit channelChanged();
    createHistogram();
}

#endif // QCOLORHISTOGRAM_H
