/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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

#include "qmatloader.h"
#include "qmat.h"
#include "live/staticcontainer.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"

#include <QQmlContext>
#include <QQmlEngine>

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
    lv::StaticContainer* container = lv::StaticContainer::grabFromContext(this);
    QMat* m = container->get<QMat>(id);
    if ( !m ){
        QMatLoaderParams mparams = QMatLoaderParams::parse(params);
        cv::Mat* cvm = 0;
        if ( mparams.color == cv::Scalar(-1) ){
            cvm = new cv::Mat(mparams.height, mparams.width, CV_MAKETYPE(mparams.type, mparams.channels));
        } else {
            cvm = new cv::Mat(mparams.height, mparams.width, CV_MAKETYPE(mparams.type, mparams.channels), mparams.color);
        }
        m = new QMat(cvm);
        lv::ViewContext::instance().engine()->engine()->setObjectOwnership(m, QQmlEngine::CppOwnership);
        container->set<QMat>(id, m);
    }
    return m;
}
