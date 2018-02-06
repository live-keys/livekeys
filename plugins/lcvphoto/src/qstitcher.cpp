#include "qstitcher.h"
#include "live/engine.h"
#include "live/exception.h"
#include "live/plugincontext.h"

QStitcher::QStitcher(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_stitcher(cv::Stitcher::create())
{

}

void QStitcher::filter(){
    if ( m_input && m_input->size() > 1 ){
        try{
            cv::Stitcher::Status status = m_stitcher->stitch(m_input->asVector(), *output()->cvMat());

            if ( status == cv::Stitcher::OK ){
                setImplicitWidth(output()->data().cols);
                setImplicitHeight(output()->data().rows);
                emit outputChanged();
                update();
            } else {
                emit error(status);
            }
        } catch ( cv::Exception& e ){
            lv::Exception lve = CREATE_EXCEPTION(lv::Exception, e.what(), e.code);
            lv::PluginContext::engine()->throwError(&lve, this);
            return;
        }
    }
}

void QStitcher::setParams(const QVariantMap &params){
    if (m_params == params)
        return;

    m_params = params;
    emit paramsChanged(m_params);

    cv::Stitcher::Mode mode = cv::Stitcher::PANORAMA;
    bool tryUseGpu = false;

    if ( params.contains("mode") )
        mode = static_cast<cv::Stitcher::Mode>(params["mode"].toInt());
    if ( params.contains("tryUseGpu") )
        tryUseGpu = params["tryUseGpu"].toBool();

    m_stitcher = cv::Stitcher::create(mode, tryUseGpu);

    filter();
}
