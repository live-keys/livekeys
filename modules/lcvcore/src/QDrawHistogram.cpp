#include "QDrawHistogram.hpp"
#include <QSGTexture>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QQuickWindow>

#include <QPainter>

QDrawHistogramNode::QDrawHistogramNode(QQuickWindow *window)
    : m_fbo(0)
    , m_texture(0)
    , m_window(window)
    , m_painter(new QPainter)
    , m_paintDevice(0)
{
}

QDrawHistogramNode::~QDrawHistogramNode(){
    delete m_texture;
    delete m_fbo;
    delete m_painter;
    delete m_paintDevice;
}

void QDrawHistogramNode::render(const QVariantList &values, const QVariantList &colors, qreal maxValue){
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
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

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

            renderSingleList(
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
        renderSingleList(
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

void QDrawHistogramNode::renderSingleList(QPainter *painter,
        const QSize& size,
        const QVariantList &values,
        const QColor &color,
        qreal maxValue)
{
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

QDrawHistogram::QDrawHistogram(QQuickItem *parent)
    : QQuickItem(parent)
    , m_maxValue(100)
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
    node->render(m_values, m_colors, m_maxValue);

    return node;
}

