#include "valuehistory.h"
#include "live/applicationcontext.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include <QtMath>
#include <QSGTexture>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QOpenGLFunctions>
#include <QQuickWindow>

#include <QPainter>

namespace lv{

// QDrawValueHistoryNode definition
// --------------------------------

DrawValueHistoryNode::DrawValueHistoryNode(QQuickWindow* window)
    : m_fbo(0)
    , m_texture(0)
    , m_window(window)
    , m_painter(new QPainter)
    , m_paintDevice(0)
    , m_glFunctions(new QOpenGLFunctions)
{
    m_glFunctions->initializeOpenGLFunctions();
}

DrawValueHistoryNode::~DrawValueHistoryNode(){
    delete m_texture;
    delete m_fbo;
    delete m_painter;
    delete m_paintDevice;
    delete m_glFunctions;
}

void DrawValueHistoryNode::render(const QLinkedList<double> &values, double minVal, double maxVal, const QColor& color){
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

    m_painter->setPen(QPen(color, 1));
    int totalItems = values.size();
    qreal widthStep  = (qreal)size.width() / (totalItems > 1 ? totalItems - 1 : totalItems);
    qreal heightStep = (qreal)size.height() / qFabs(maxVal - minVal);

    QLinkedList<double>::const_iterator prevIt = values.begin();
    QLinkedList<double>::const_iterator it = prevIt + 1;
    int itIndex = 1;

    while ( it != values.end() ){
        double itY     = size.height() - (*prevIt - minVal) * heightStep;
        double prevItY = size.height() - (*it - minVal) * heightStep;
        m_painter->drawLine(QPointF((itIndex - 1) * widthStep, itY), QPointF(itIndex * widthStep, prevItY));
        prevIt = it;
        ++it;
        ++itIndex;
    }

    m_painter->end();
    m_fbo->release();
}


ValueHistory::ValueHistory(QQuickItem *parent)
    : QQuickItem(parent)
    , m_maxDisplayValue(ValueHistory::Auto)
    , m_minDisplayValue(ValueHistory::Auto)
    , m_maxHistory(400)
    , m_displayColor(QColor(255, 255, 255))
{
    setFlag(ItemHasContents, true);
}

ValueHistory::~ValueHistory(){
}

QSGNode *ValueHistory::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *){
    if ( m_values.size() < 2 )
        return oldNode;
    if ( boundingRect().isEmpty() )
        return oldNode;

    DrawValueHistoryNode* node = static_cast<DrawValueHistoryNode*>(oldNode);
    if ( !node )
        node = new DrawValueHistoryNode(window());

    node->setRect(boundingRect());

    double maxValue = m_maxDisplayValue;
    double minValue = m_minDisplayValue;

    if ( maxValue == ValueHistory::Auto ){
        maxValue = INT_MIN;
        for ( auto it = m_values.begin(); it != m_values.end(); ++it ){
            if ( *it > maxValue ){
                maxValue = *it;
            }
        }
    }
    if ( minValue == ValueHistory::Auto ){
        minValue = INT_MAX;
        for ( auto it = m_values.begin(); it != m_values.end(); ++it ){
            if ( *it < minValue ){
                minValue = *it;
            }
        }
    }

    if ( minValue == maxValue && m_minDisplayValue != ValueHistory::Auto && m_maxDisplayValue != ValueHistory::Auto){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "ValueHistory error: minDisplayValue == maxDisplayValue.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return node;
    }

    node->render(m_values, minValue, maxValue, m_displayColor);

    return node;

}

}// namespace
