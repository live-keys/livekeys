#include "qdrawhistogram.h"
#include "math.h"
#include <QSGTexture>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QOpenGLFunctions>
#include <QQuickWindow>

#include <QPainter>

// QAbstractHistogramRenderer definitions
// --------------------------------------

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

class QHistogramBinaryRenderer : public QAbstractHistogramRenderer{

public:
    QHistogramBinaryRenderer(){}

    virtual void renderSingleList(
        QPainter* painter,
        const QSize &size,
        const QVariantList& values,
        const QColor &color,
        qreal maxValue
    ){
        painter->setPen(QPen(color, 1));
        painter->setBrush(QBrush(color));

        int totalItems   = values.size() > (int)maxValue ? (int)maxValue : values.size();
        int cellsPerLine = (int)ceil(sqrt(((float)maxValue * size.width()) / size.height()));
        int cellSize     = (int)floor(size.width() / cellsPerLine);

        for ( int i = 0; i < totalItems; ++i ){
            int row  = (int)(i / cellsPerLine);
            if ( values[i].toBool() ){
                painter->drawRect(
                    QRectF(
                        QPointF((double)(i % cellsPerLine * cellSize), (double)(row * cellSize)),
                        QSizeF(cellSize, cellSize)
                    )
                );
            }
        }
    }
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
    }

    emit renderChanged();
}
