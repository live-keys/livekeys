#include "qmatloader.h"
#include "qmat.h"
#include "qstaticcontainer.h"

#include <QQmlContext>

namespace{

class QMatLoaderParams{
public:
    int width;
    int height;
    int channels;
    QMat::Type type;
    cv::Scalar color;

    static QMatLoaderParams parse(const QJSValue& value){
        QMatLoaderParams mp;
        if ( value.hasOwnProperty("w") )
            mp.width = value.property("w").toInt();
        if ( value.hasOwnProperty("h") )
            mp.height = value.property("h").toInt();
        if ( value.hasOwnProperty("ch") )
            mp.channels = value.property("ch").toInt();
        if  ( value.hasOwnProperty("type") )
            mp.type = static_cast<QMat::Type>(value.property("type").toInt());
        if ( value.hasOwnProperty("color" ) ){
            QColor cl(value.property("color").toString());
            if ( cl.isValid() ){
                mp.color = cv::Scalar(cl.blue(), cl.green(), cl.red(), cl.alpha());
            }
        }
        return mp;
    }

private:
    QMatLoaderParams()
        : width(0)
        , height(0)
        , channels(4)
        , type(QMat::CV8U)
        , color(cv::Scalar(-1))
    {
    }

};

}// namespace

QMatLoader::QMatLoader(QQuickItem *parent)
    : QQuickItem(parent)
{
}

QMatLoader::~QMatLoader(){

}

QMat *QMatLoader::staticLoad(const QString &id, const QJSValue &params){
    QStaticContainer* container = QStaticContainer::grabFromContext(this);
    QMat* m = container->get<QMat>(id);
    if ( !m ){
        QMatLoaderParams mparams = QMatLoaderParams::parse(params);
        cv::Mat* cvm = 0;
        if ( mparams.color == cv::Scalar(-1) ){
            cvm = new cv::Mat(mparams.width, mparams.height, CV_MAKETYPE(mparams.type, mparams.channels));
        } else {
            cvm = new cv::Mat(mparams.width, mparams.height, CV_MAKETYPE(mparams.type, mparams.channels), mparams.color);
        }
        m = new QMat(cvm);
        container->set<QMat>(id, m);
    }
    return m;
}
