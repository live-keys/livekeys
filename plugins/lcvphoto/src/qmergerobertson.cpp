#include "qmergerobertson.h"

QMergeRobertson::QMergeRobertson(QObject *parent)
    : QObject(parent)
    , m_input(0)
    , m_response(0)
    , m_output(new QMat)
    , m_mergeRobertson(cv::createMergeRobertson())
    , m_componentComplete(false)
{
}

QMergeRobertson::~QMergeRobertson(){
    delete m_output;
}

void QMergeRobertson::filter(){
    if ( m_componentComplete &&
         m_response &&
         m_input &&
         m_input->size() == m_times.size() &&
         m_input->size() > 0)
    {
        std::vector<float> times;
        for ( int i = 0; i < m_times.size(); ++i )
            times.push_back(m_times[i]);

        m_mergeRobertson->process(m_input->asVector(), *m_output->cvMat(), times, *m_response->cvMat());

        emit outputChanged();
    }
}
