#include "qcalibraterobertson.h"
#include "live/exception.h"
#include "live/engine.h"
#include "live/plugincontext.h"

QCalibrateRobertson::QCalibrateRobertson(QObject *parent)
    : QObject(parent)
    , m_input(0)
    , m_output(new QMat)
    , m_calibrateRobertson(cv::createCalibrateRobertson())
    , m_componentComplete(false)
{
}

QCalibrateRobertson::~QCalibrateRobertson(){
    delete m_output;
}

void QCalibrateRobertson::setParams(const QVariantMap &params){
    if (m_params == params)
        return;

    m_params = params;
    emit paramsChanged();

    int maxIter     = 30;
    float threshold = 0.01f;

    if ( params.contains("maxIter") )
        maxIter = params["maxIter"].toInt();
    if ( params.contains("threshold") )
        threshold = params["threshold"].toFloat();

    m_calibrateRobertson = cv::createCalibrateRobertson(maxIter, threshold);

    filter();
}

void QCalibrateRobertson::filter(){
    if ( m_componentComplete &&
         m_input &&
         m_input->size() == m_times.size() &&
         m_input->size() > 0 &&
         m_calibrateRobertson)
    {
        try{
            std::vector<float> times;
            for ( int i = 0; i < m_times.size(); ++i )
                times.push_back(m_times[i]);

            m_calibrateRobertson->process(m_input->asVector(), *m_output->cvMat(), times);
        } catch ( cv::Exception& e ){
            lv::Exception lve = CREATE_EXCEPTION(lv::Exception, e.what(), e.code);
            lv::PluginContext::engine()->throwError(&lve, this);
            return;
        }

    }
}
