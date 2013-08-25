#include "QMat.hpp"

QMat::QMat(QQuickItem *parent):
    QQuickItem(parent),
    m_data(new cv::Mat){
}

QMat::QMat(cv::Mat *mat, QQuickItem *parent):
    QQuickItem(parent),
    m_data(mat){
}

QMat::~QMat(){
    delete m_data;
}
