/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "qdrawhistogram.h"
#include "math.h"
#include <QSGTexture>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QOpenGLFunctions>
#include <QQuickWindow>

#include <QPainter>

//TODO: Document

// QAbstractHistogramRenderer definitions
// --------------------------------------

/// \private
QAbstractHistogramRenderer::QAbstractHistogramRenderer(){
}

QAbstractHistogramRenderer::~QAbstractHistogramRenderer(){
}

// QAbstractHistogramRenderer implementations
// ------------------------------------------

class QHistogramConnectedLinesRenderer : public QAbstractHistogramRenderer{

public:
    QHistogramConnectedLinesRenderer(){}

    virtual void renderSingleList(
        QPainter* painter,
        const QSize &size,
        const QVariantList& values,
        const QColor &color,
        qreal maxValue
    ){
        painter->setPen(QPen(color, 1));

        int totalItems = values.size();
        qreal widthStep  = (qreal)size.width() / (totalItems > 1 ? totalItems - 1 : totalItems);
        qreal heightStep = (qreal)size.height() / maxValue;

        for ( int i = 1; i < values.size(); ++i ){
            double val = values[i].toDouble();
            painter->drawLine(
                QPointF((i - 1) * widthStep, size.height() - values[i - 1].toDouble() * heightStep),
                QPointF(i * widthStep, size.height() - val * heightStep)
            );
        }
    }
};

/// \private
class QHistogramRectanglesRenderer : public QAbstractHistogramRenderer{

public:
    QHistogramRectanglesRenderer(){}

    virtual void renderSingleList(
        QPainter* painter,
        const QSize &size,
        const QVariantList& values,
        const QColor &color,
        qreal maxValue
    ){
        painter->setPen(QPen(color, 1));
        painter->setBrush(QBrush(color));

        int totalItems = values.size();
        qreal widthStep  = (qreal)size.width() / totalItems;
        qreal heightStep = (qreal)size.height() / maxValue;

        for ( int i = 0; i < values.size(); ++i ){
            double val = values[i].toDouble();
            painter->drawRect(
                QRectF(
                    QPointF((double)i * widthStep, size.height()),
                    QPointF((double)(i + 1) * widthStep, size.height() - val * heightStep)
                )
            );
        }
    }
};

/// \private
class QHistogramBinaryRenderer : public QAbstractHistogramRenderer{

public:
    QHistogramBinaryRenderer(bool convert = false) : m_convert(convert){}

    virtual void renderSingleList(
        QPainter* painter,
        const QSize &size,
        const QVariantList& values,
        const QColor &color,
        qreal maxValue
    ){
        painter->setPen(QPen(color, 1));
        painter->setBrush(QBrush(color));

        if ( m_convert ){

            int totalItems   = values.size() > (int)maxValue / 32 ? (int)maxValue / 32 : values.size();
            int cellsPerLine = (int)ceil(sqrt(((float)totalItems * 32 * size.width()) / size.height()));
            int cellSize     = (int)floor(size.width() / cellsPerLine);
            int currentItem = 0;


            for ( int i = 0; i < totalItems; ++i ){
                int val = values[i].toInt();
                for ( int t = 31; t >= 0; --t ){
                    int row  = (int)(currentItem / cellsPerLine);
                    if ( val & (1 << t) ){
                        painter->drawRect(
                            QRectF(
                                QPointF((double)(currentItem % cellsPerLine * cellSize), (double)(row * cellSize)),
                                QSizeF(cellSize, cellSize)
                            )
                        );
                    }
                    ++currentItem;
                }
            }

        } else {

            int totalItems   = values.size() > (int)maxValue ? (int)maxValue : values.size();
            int cellsPerLine = (int)ceil(sqrt(((float)totalItems * size.width()) / size.height()));
            int cellSize     = (int)floor(size.width() / cellsPerLine);
            int currentItem = 0;

            for ( int i = 0; i < totalItems; ++i ){
                int row  = (int)(currentItem / cellsPerLine);
                if ( values[i].toBool() ){
                    painter->drawRect(
                        QRectF(
                            QPointF((double)(i % cellsPerLine * cellSize), (double)(row * cellSize)),
                            QSizeF(cellSize, cellSize)
                        )
                    );
                }
                ++currentItem;
            }
        }
    }

private:
    bool m_convert;
};


// QDrawHistogramNode definitions
// ------------------------------

QDrawHistogramNode::QDrawHistogramNode(QQuickWindow *window)
    : m_fbo(0)
    , m_texture(0)
    , m_window(window)
    , m_painter(new QPainter)
    , m_paintDevice(0)
    , m_glFunctions(new QOpenGLFunctions)
{
    m_glFunctions->initializeOpenGLFunctions();
}

QDrawHistogramNode::~QDrawHistogramNode(){
    delete m_texture;
    delete m_fbo;
    delete m_painter;
    delete m_paintDevice;
    delete m_glFunctions;
}

void QDrawHistogramNode::render(
        const QVariantList &values,
        const QVariantList &colors,
        qreal maxValue,
        QAbstractHistogramRenderer* renderer
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
    if ( values.isEmpty() ){
        m_fbo->release();
        return;
    }

    m_paintDevice->setSize(size);
    m_painter->begin(m_paintDevice);
    m_painter->setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing);

    QVariant value = values.first();
    if ( value.type() == QVariant::List ){
        QVariantList::const_iterator colorIt = colors.begin();

        for( QVariantList::const_iterator it = values.begin(); it != values.end(); ++it ){
            const QVariant& v = *it;
            if ( v.type() != QVariant::List ){
                qCritical("Error: Incosistent value. Not of list type. [Type: %s]", v.typeName());
                return;
            }

            QColor color = colorIt == colors.end() ? QColor(255, 255, 255, 255) : QColor((*colorIt).toString());

            renderer->renderSingleList(
                m_painter,
                size,
                v.toList(),
                color,
                maxValue
            );

            ++colorIt;
            if ( colorIt == colors.end() )
                colorIt = colors.begin();
        }
    } else {
        renderer->renderSingleList(
            m_painter,
            size,
            values,
            colors.isEmpty() ? QColor(255, 255, 255, 255) : QColor(colors.first().toString()),
            maxValue
        );
    }

    m_painter->end();
    m_fbo->release();
}

// QDrawHistogram definitions
// --------------------------

QDrawHistogram::QDrawHistogram(QQuickItem *parent)
    : QQuickItem(parent)
    , m_maxValue(100)
    , m_renderType(QDrawHistogram::ConnectedLines)
    , m_renderer(new QHistogramConnectedLinesRenderer)
{
    setFlag(ItemHasContents, true);
}

QDrawHistogram::~QDrawHistogram(){
}

QSGNode *QDrawHistogram::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *){
    QDrawHistogramNode* node = static_cast<QDrawHistogramNode*>(oldNode);
    if (!node)
        node = new QDrawHistogramNode(window());

    node->setRect(boundingRect());
    node->render(m_values, m_colors, m_maxValue, m_renderer);

    return node;
}

void QDrawHistogram::setRender(QDrawHistogram::RenderType arg){
    if (m_renderType == arg)
        return;

    m_renderType = arg;
    delete m_renderer;

    switch (m_renderType) {
    case QDrawHistogram::ConnectedLines:
        m_renderer = new QHistogramConnectedLinesRenderer;
        break;
    case QDrawHistogram::Rectangles:
        m_renderer = new QHistogramRectanglesRenderer;
        break;
    case QDrawHistogram::Binary:
        m_renderer = new QHistogramBinaryRenderer;
        break;
    case QDrawHistogram::BinaryConverted:
        m_renderer = new QHistogramBinaryRenderer(true);
    }

    emit renderChanged();
}

void QDrawHistogram::setValuesFromIntListAt(const QList<int> &values, int index){
    QVariantList vals;
    for ( QList<int>::const_iterator it = values.begin(); it != values.end(); ++it )
        vals.append(*it);

    if ( m_values.size() == 0 ){
        m_values.reserve(index + 1);
        for ( int i = 0; i < index + 1; ++i )
            m_values.append(QVariant::fromValue(QVariantList()));
        m_values[index] = vals;
    } else {
        if ( m_values.at(0).type() != QVariant::List ){
            m_values.clear();
        }
        if ( index >= m_values.size() ){
            m_values.reserve(index + 1);
            for ( int i = m_values.size(); i < index + 1; ++i )
                m_values.append(QVariant::fromValue(QVariantList()));
        }
        m_values[index] = vals;
    }

    emit valuesChanged();
    update();
}
