#include "qtonemap.h"

QTonemap::QTonemap(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_input(QMat::nullMat())
    , m_tonemapper()
{
}

QTonemap::QTonemap(cv::Ptr<cv::Tonemap> tonemapper, QQuickItem *parent)
    : QMatDisplay(parent)
    , m_input(QMat::nullMat())
    , m_tonemapper(tonemapper){
}

QTonemap::~QTonemap(){
}

void QTonemap::initialize(const QVariantMap &){
}

void QTonemap::initializeTonemapper(cv::Ptr<cv::Tonemap> tonemapper){
    m_tonemapper = tonemapper;
    process();
}

void QTonemap::process(){
    if ( m_tonemapper && !m_input->data().empty() && isComponentComplete() ){
        m_tonemapper->process(m_input->data(), m_store);
//        m_store = m_store * 3;

        m_store.convertTo(*output()->cvMat(), CV_8UC3, 255);

        setImplicitWidth(output()->data().cols);
        setImplicitHeight(output()->data().rows);

        emit outputChanged();
        update();
    }
}

void QTonemap::componentComplete(){
    QQuickItem::componentComplete();
    process();
}
