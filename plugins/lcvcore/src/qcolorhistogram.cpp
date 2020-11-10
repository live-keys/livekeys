/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "qcolorhistogram.h"

#include <QPainter>
#include <QSGTexture>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QOpenGLFunctions>
#include <QQuickWindow>

// QAbstractColorHistogramRenderer implementations
// -----------------------------------------------
/// \private
class QColorHistogramConnectedLinesRenderer : public QAbstractColorHistogramRenderer{

public:
    QColorHistogramConnectedLinesRenderer(){}

    virtual void renderSingleList(
        QPainter* painter,
        const QSize &size,
        const ushort* values,
        size_t valuesSize,
        qreal maxValue,
        const QColor& color
    ){
        painter->setPen(QPen(color, 1));

        qreal widthStep  = (qreal)size.width() / (valuesSize > 1 ? valuesSize - 1 : valuesSize);
        qreal heightStep = (qreal)size.height() / maxValue;

        for ( size_t i = 0; i < valuesSize; ++i ){
            ushort val = values[i];
            painter->drawLine(
                QPointF((i - 1) * widthStep, size.height() - values[i - 1] * heightStep),
                QPointF(i * widthStep, size.height() - val * heightStep)
            );
        }
    }
};
/// \private
class QColorHistogramRectanglesRenderer : public QAbstractColorHistogramRenderer{

public:
    QColorHistogramRectanglesRenderer(){}

    virtual void renderSingleList(
        QPainter* painter,
        const QSize &size,
        const ushort* values,
        size_t valuesSize,
        qreal maxValue,
        const QColor& color
    ){
        painter->setPen(QPen(color, 1));
        painter->setBrush(QBrush(color));

        qreal widthStep  = (qreal)size.width() / valuesSize;
        qreal heightStep = (qreal)size.height() / maxValue;

        for ( size_t i = 0; i < valuesSize; ++i ){
            ushort val = values[i];
            painter->drawRect(
                QRect(
                    QPoint(i * widthStep, size.height()),
                    QPoint((i + 1) * widthStep, size.height() - val * heightStep)
                )
            );
        }
    }
};

// QColorHistogramNode definitions
// -------------------------------

QColorHistogramNode::QColorHistogramNode(QQuickWindow *window)
    : m_fbo(0)
    , m_texture(0)
    , m_window(window)
    , m_painter(new QPainter)
    , m_paintDevice(0)
    , m_glFunctions(new QOpenGLFunctions)
{
    m_glFunctions->initializeOpenGLFunctions();
}

QColorHistogramNode::~QColorHistogramNode(){
    delete m_texture;
    delete m_fbo;
    delete m_painter;
    delete m_paintDevice;
    delete m_glFunctions;
}

void QColorHistogramNode::render(
        int channel,
        const cv::Mat& values,
        qreal maxValue,
        QAbstractColorHistogramRenderer* renderer
){
    QSize size = rect().size().toSize();

    if ( !m_fbo ){
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        m_fbo = new QOpenGLFramebufferObject(size, format);
        m_texture = m_window->createTextureFromId(m_fbo->texture(), size, QQuickWindow::TextureHasAlphaChannel);
        m_paintDevice = new QOpenGLPaintDevice;
        m_paintDevice->setPaintFlipped(true);
        setTexture(m_texture);
    }

    m_fbo->bind();
    m_glFunctions->glClearColor(0, 0, 0, 0);
    m_glFunctions->glClear(GL_COLOR_BUFFER_BIT);

    if ( size.isEmpty() ){
        m_fbo->release();
        return;
    }
    if ( values.empty() ){
        m_fbo->release();
        return;
    }

    m_paintDevice->setSize(size);
    m_painter->begin(m_paintDevice);
    m_painter->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

    static QColor colors[5] = {QColor("#05426e"), QColor("#00803c"), QColor("#6e0516"), QColor("#eee"), QColor("#9b0bbf")};

    if ( values.rows == 1 ){
        QColor color = channel < 0 ? QColor("#fff") : colors[(channel > 4 ? 4 : channel)];
        renderer->renderSingleList(m_painter, size, values.ptr<ushort>(0), values.cols, maxValue, color);
    } else {
        for ( int i = 0 ; i < values.rows; ++i ){
            QColor& color = i > 4 ? colors[4] : colors[i];

            const ushort* p = values.ptr<ushort>(i);
            renderer->renderSingleList(m_painter, size, p, values.cols, maxValue, color);
        }
    }

    m_painter->end();
    m_fbo->release();
}


// QColorHistogram definitions
// ---------------------------

QColorHistogram::QColorHistogram(QQuickItem *parent)
    : QQuickItem(parent)
    , m_input(QMat::nullMat())
    , m_output(new QMat)
    , m_fill(false)
    , m_channel(QColorHistogram::AllChannels)
    , m_renderer(new QColorHistogramConnectedLinesRenderer)
{
    setFlag(ItemHasContents, true);
    setImplicitSize(300, 256);
}

QColorHistogram::~QColorHistogram(){
    delete m_output;
    delete m_renderer;
}

void QColorHistogram::componentComplete(){
    QQuickItem::componentComplete();
    createHistogram();
}

QSGNode *QColorHistogram::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *){
    QColorHistogramNode* node = static_cast<QColorHistogramNode*>(oldNode);
    if (!node)
        node = new QColorHistogramNode(window());

    ushort maxValue = 0;
    for ( int i = 0; i < m_output->internalPtr()->rows; ++i ){
        const ushort* p = m_output->internalPtr()->ptr<ushort>(i);
        for ( int j = 0; j < m_output->internalPtr()->cols; ++j ){
            if ( p[j] > maxValue ){
                maxValue = p[j];
            }
        }
    }

    node->setRect(boundingRect());

    int sendChannel = (m_channel == 0 && m_input->internalPtr()->channels() == 1) ? QColorHistogram::Total : m_channel;
    node->render(sendChannel, *m_output->internalPtr(), maxValue, m_renderer);

    return node;
}


void QColorHistogram::createHistogram(){
    if ( !isComponentComplete() )
        return;

    if ( m_channel == QColorHistogram::AllChannels ){
        m_output->internalPtr()->create(m_input->internalPtr()->channels(), 256, CV_16UC1);
        m_output->internalPtr()->setTo(0);
        ushort* outdata = reinterpret_cast<ushort*>(m_output->internalPtr()->data);
        for ( int y = 0; y < m_input->internalPtr()->rows; ++y ){
            uchar* p  = m_input->internalPtr()->ptr<uchar>(y);
            for ( int x = 0; x < m_input->internalPtr()->cols; ++x ){
                for ( int c = 0; c < m_input->internalPtr()->channels(); ++c ){
                    uchar v = p[x * m_input->internalPtr()->channels() + c];
                    outdata[c * (m_output->internalPtr()->step / sizeof(ushort)) + v]++;
                }
            }
        }
    } else if ( m_channel == QColorHistogram::Total ){
        m_output->internalPtr()->create(1, 256, CV_16UC1);
        m_output->internalPtr()->setTo(0);
        ushort* outdata = reinterpret_cast<ushort*>(m_output->internalPtr()->data);
        for ( int y = 0; y < m_input->internalPtr()->rows; ++y ){
            uchar* p  = m_input->internalPtr()->ptr<uchar>(y);
            for ( int x = 0; x < m_input->internalPtr()->cols; ++x ){
                for ( int c = 0; c < m_input->internalPtr()->channels(); ++c ){
                    uchar v = p[x * m_input->internalPtr()->channels() + c];
                    outdata[v]++;
                }
            }
        }
    } else {
        m_output->internalPtr()->create(1, 256, CV_16UC1);
        m_output->internalPtr()->setTo(0);
        ushort* outdata = reinterpret_cast<ushort*>(m_output->internalPtr()->data);
        int c = m_channel;
        for ( int y = 0; y < m_input->internalPtr()->rows; ++y ){
            uchar* p  = m_input->internalPtr()->ptr<uchar>(y);
            for ( int x = 0; x < m_input->internalPtr()->cols; ++x ){
                uchar v = p[x * m_input->internalPtr()->channels() + c];
                outdata[v]++;
            }
        }
    }

    emit outputChanged();
    update();
}


void QColorHistogram::setFill(bool fill){
    if (m_fill == fill)
        return;

    delete m_renderer;
    m_renderer = fill ? new QColorHistogramRectanglesRenderer : new QColorHistogramRectanglesRenderer;

    m_fill = fill;
    emit fillChanged();
    createHistogram();
}
