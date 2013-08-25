#include "QMatSource.hpp"
#include "QMatState.hpp"
#include "QMatNode.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <QSGSimpleMaterial>

QMatSource::QMatSource(QQuickItem *parent) :
    QMatDisplay(parent)
{
}

QMatSource::~QMatSource(){
}

void QMatSource::setFile(const QString &file){
    if ( file != m_file ){
        m_file = file;
        emit fileChanged();
        cv::Mat temp = cv::imread(file.toStdString());

        if ( !temp.empty() ){
            temp.copyTo(*out()->data());
            emit outChanged();
            update();
        }
    }
}
