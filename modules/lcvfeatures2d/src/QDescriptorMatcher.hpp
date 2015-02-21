#ifndef QDESCRIPTORMATCHER_HPP
#define QDESCRIPTORMATCHER_HPP

#include <QQuickItem>
#include "QMat.hpp"
#include "QDMatchVector.hpp"

namespace cv{
    class DescriptorMatcher;
}

class QDescriptorMatcher : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QMat*          queryDescriptors READ queryDescriptors WRITE  setQueryDescriptors NOTIFY queryDescriptorsChanged)
    Q_PROPERTY(QDMatchVector* matches          READ matches          NOTIFY matchesChanged)

public:
    explicit QDescriptorMatcher(QQuickItem *parent = 0);
    QDescriptorMatcher(cv::DescriptorMatcher* matcher, QQuickItem *parent = 0);
    virtual ~QDescriptorMatcher();

    void setQueryDescriptors(QMat* descriptors);
    QMat* queryDescriptors();

    QDMatchVector* matches();

signals:
    void queryDescriptorsChanged();
    void matchesChanged();

public slots:
    void add(QMat* descriptors);
    void train();

    void match(QMat* queryDescriptors, QDMatchVector* matches);

private:
    cv::DescriptorMatcher* m_matcher;

    QDMatchVector*         m_matches;
    QMat*                  m_queryDescriptors;
};

inline void QDescriptorMatcher::setQueryDescriptors(QMat* descriptors){
    m_queryDescriptors = descriptors;
    emit queryDescriptorsChanged();
    if ( isComponentComplete() )
        match(m_queryDescriptors, m_matches);
}

inline QMat* QDescriptorMatcher::queryDescriptors(){
    return m_queryDescriptors;
}

#endif // QDESCRIPTORMATCHER_HPP
