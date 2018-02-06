#include "qmergedebevec.h"
#include "live/exception.h"
#include "live/plugincontext.h"
#include "live/engine.h"

QMergeDebevec::QMergeDebevec(QObject *parent)
    : QObject(parent)
    , m_input(0)
    , m_response(0)
    , m_output(new QMat)
    , m_mergeDebevec(cv::createMergeDebevec())
    , m_componentComplete(false)
{
}

QMergeDebevec::~QMergeDebevec(){
    delete m_output;
}

void QMergeDebevec::filter(){
    if ( m_componentComplete &&
         m_response &&
         !m_response->data().empty() &&
         m_input &&
         m_input->size() == m_times.size() &&
         m_input->size() > 0)
    {
        try{
            std::vector<float> times;
            for ( int i = 0; i < m_times.size(); ++i )
                times.push_back(m_times[i]);

            m_mergeDebevec->process(m_input->asVector(), *m_output->cvMat(), times, *m_response->cvMat());

            emit outputChanged();

        } catch ( cv::Exception& e ){
            lv::Exception lve = CREATE_EXCEPTION(lv::Exception, e.what(), e.code);
            lv::PluginContext::engine()->throwError(&lve, this);
            return;
        }
    }
}
