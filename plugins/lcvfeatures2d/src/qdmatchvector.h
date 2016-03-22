#ifndef QDMATCHVECTOR_HPP
#define QDMATCHVECTOR_HPP

#include <QQuickItem>
#include "qlcvfeatures2dglobal.h"
#include "opencv2/features2d/features2d.hpp"

class Q_LCVFEATURES2D_EXPORT QDMatchVector : public QQuickItem{

    Q_OBJECT

public:
    enum Type{
        BEST_MATCH = 0,
        KNN,
        RADIUS
    };

public:
    explicit QDMatchVector(QQuickItem *parent = 0);
    virtual ~QDMatchVector();

    std::vector<std::vector<cv::DMatch> >& matches();
    const std::vector<std::vector<cv::DMatch> >& matches() const;

    Type type() const;
    void setType(Type type);

private:
    std::vector<std::vector<cv::DMatch> > m_matches;
    Type m_type;
};

inline const std::vector< std::vector<cv::DMatch> >& QDMatchVector::matches() const{
    return m_matches;
}

inline QDMatchVector::Type QDMatchVector::type() const{
    return m_type;
}

inline void QDMatchVector::setType(QDMatchVector::Type type){
    m_type = type;
}

inline std::vector<std::vector<cv::DMatch> >& QDMatchVector::matches(){
    return m_matches;
}

#endif // QDMATCHVECTOR_HPP
