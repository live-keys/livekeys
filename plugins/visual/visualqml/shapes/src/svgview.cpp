#include "svgview.h"
#include <QSvgRenderer>
#include <QImage>
#include <QPainter>
#include <QQuickWindow>

namespace lv{

SvgView::SvgView(){
}

SvgView::~SvgView(){
}

void SvgView::paint(QPainter *painter){

    if ( window() && !qFuzzyCompare(window()->devicePixelRatio(), 1) ){
        qreal dpr = window()->devicePixelRatio();
        m_renderer.setViewBox(QRectF(
            m_rendererViewPort.x(), m_rendererViewPort.y(), m_rendererViewPort.width() * dpr, m_rendererViewPort.height() * dpr
        ));
    }
    m_renderer.render(painter);
}

void SvgView::loadInRenderer(){
    if ( !m_content.isEmpty() ){
        m_renderer.load(m_content.toUtf8());
        QSize rendererSize = m_renderer.defaultSize();
        setImplicitSize(rendererSize.width(), rendererSize.height());
        m_rendererViewPort = m_renderer.viewBox();
    } else if ( !m_path.isEmpty() ){
        m_renderer.load(m_path);
        QSize rendererSize = m_renderer.defaultSize();
        setImplicitSize(rendererSize.width(), rendererSize.height());
        m_rendererViewPort = m_renderer.viewBox();
    }
}

void SvgView::componentComplete(){
    QQuickPaintedItem::componentComplete();
    loadInRenderer();
    update();
}

}// namespace
