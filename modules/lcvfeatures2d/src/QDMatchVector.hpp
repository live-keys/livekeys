#ifndef QDMATCHVECTOR_HPP
#define QDMATCHVECTOR_HPP

#include <QQuickItem>
#include "opencv2/features2d/features2d.hpp"

class QDMatchVector : public QQuickItem{

    Q_OBJECT

public:
    explicit QDMatchVector(QQuickItem *parent = 0);
    virtual ~QDMatchVector();

    std::vector<std::vector<cv::DMatch> >& matches();
    const std::vector<std::vector<cv::DMatch> >& matches() const;

private:
    std::vector<std::vector<cv::DMatch> > m_matches;
};

inline const std::vector< std::vector<cv::DMatch> >& QDMatchVector::matches() const{
    return m_matches;
}

inline std::vector<std::vector<cv::DMatch> >& QDMatchVector::matches(){
    return m_matches;
}

#endif // QDMATCHVECTOR_HPP
