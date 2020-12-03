#include "svgview.h"
#include <QSvgRenderer>
#include <QImage>
#include <QPainter>

namespace lv{

SvgView::SvgView(){
}

SvgView::~SvgView(){
}

void SvgView::paint(QPainter *painter){
    m_renderer.render(painter);
}

void SvgView::loadInRenderer(){
    if ( !m_content.isEmpty() ){
        m_renderer.load(m_content.toUtf8());
        QSize rendererSize = m_renderer.defaultSize();
        setImplicitSize(rendererSize.width(), rendererSize.height());
    } else if ( !m_path.isEmpty() ){
        m_renderer.load(m_path);
        QSize rendererSize = m_renderer.defaultSize();
        setImplicitSize(rendererSize.width(), rendererSize.height());
    }
}

void SvgView::componentComplete(){
    QQuickPaintedItem::componentComplete();
    loadInRenderer();
    update();
}

}// namespace
