#include "qcalibratedebevec.h"
#include "live/plugincontext.h"
#include "live/engine.h"
#include "live/exception.h"

QCalibrateDebevec::QCalibrateDebevec(QObject *parent)
    : QObject(parent)
    , m_input(0)
    , m_output(new QMat)
    , m_calibrateDebevec(cv::createCalibrateDebevec())
    , m_componentComplete(false)
{
}

QCalibrateDebevec::~QCalibrateDebevec(){
    delete m_output;
}

void QCalibrateDebevec::setParams(const QVariantMap &params){
    if (m_params == params)
        return;

    m_params = params;
    emit paramsChanged();

    int samples  = 70;
    float lambda = 10.0f;
    bool random  = false;

    if ( params.contains("samples") )
        samples = params["samples"].toInt();
    if ( params.contains("lambda") )
        lambda = params["lambda"].toFloat();
    if ( params.contains("random") )
        random = params["random"].toBool();

    m_calibrateDebevec = cv::createCalibrateDebevec(samples, lambda, random);

    filter();
}

void QCalibrateDebevec::filter(){
    if ( m_componentComplete &&
         m_input &&
         m_input->size() == m_times.size() &&
         m_input->size() > 0 &&
         m_calibrateDebevec)
    {
        try{
            std::vector<float> times;
            for ( int i = 0; i < m_times.size(); ++i )
                times.push_back(m_times[i]);

            m_calibrateDebevec->process(m_input->asVector(), *m_output->cvMat(), times);

            emit outputChanged();

        } catch ( cv::Exception& e ){
            lv::Exception lve = CREATE_EXCEPTION(lv::Exception, e.what(), e.code);
            lv::PluginContext::engine()->throwError(&lve, this);
            return;
        }

    }
}
