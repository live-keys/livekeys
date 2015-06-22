#ifndef QMATCHFILTER_H
#define QMATCHFILTER_H

#include <QQuickItem>
#include "QDMatchVector.hpp"
#include "QLCVFeatures2DGlobal.hpp"

class Q_LCVFEATURES2D_EXPORT QDescriptorMatchFilter : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QDMatchVector* matches1to2    READ matches1to2    WRITE setMatches1to2 NOTIFY matches1to2Changed)
    Q_PROPERTY(QDMatchVector* matches1to2Out READ matches1to2Out NOTIFY matches1to2OutChanged)

public:
    QDescriptorMatchFilter(QQuickItem* parent = 0);
    ~QDescriptorMatchFilter();

    QDMatchVector* matches1to2() const;
    void setMatches1to2(QDMatchVector* arg);

    QDMatchVector* matches1to2Out() const;

signals:
    void matches1to2Changed();
    void matches1to2OutChanged();

protected:
    virtual void filter(
            const std::vector<std::vector<cv::DMatch> >& src,
            std::vector<std::vector<cv::DMatch> >& dst,
    );

private:
    QDMatchVector* m_matches1to2;
    QDMatchVector* m_matches1to2Out;
};

inline QDMatchVector *QDescriptorMatchFilter::matches1to2() const{
    return m_matches1to2;
}

inline void QDescriptorMatchFilter::setMatches1to2(QDMatchVector *arg){
    if (m_matches1to2 == arg)
        return;

    m_matches1to2 = arg;
    emit matches1to2Changed();
}

inline QDMatchVector *QDescriptorMatchFilter::matches1to2Out() const{
    return m_matches1to2Out;
}

#endif // QMATCHFILTER_H
