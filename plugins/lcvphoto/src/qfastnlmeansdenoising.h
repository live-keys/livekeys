/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef QFASTNLMEANSDENOISING_H
#define QFASTNLMEANSDENOISING_H

#include "qmatfilter.h"
#include "live/filter.h"

class QFastNlMeansDenoising : public QMatFilter, public lv::Filter{

    Q_OBJECT
    Q_PROPERTY(bool  colorAlgorithm     READ colorAlgorithm     WRITE setColorAlgorithm     NOTIFY colorAlgorithmChanged)
    Q_PROPERTY(float h                  READ h                  WRITE setH                  NOTIFY hChanged)
    Q_PROPERTY(float hColor             READ hColor             WRITE setHColor             NOTIFY hColorChanged)
    Q_PROPERTY(int   templateWindowSize READ templateWindowSize WRITE setTemplateWindowSize NOTIFY templateWindowSizeChanged)
    Q_PROPERTY(int   searchWindowSize   READ searchWindowSize   WRITE setSearchWindowSize   NOTIFY searchWindowSizeChanged)

public:
    explicit QFastNlMeansDenoising(QQuickItem *parent = 0);
    ~QFastNlMeansDenoising();

    virtual void process();
    virtual void transform(const cv::Mat &in, cv::Mat &out);

    bool colorAlgorithm() const;
    float h() const;
    float hColor() const;
    int templateWindowSize() const;
    int searchWindowSize() const;

    void setColorAlgorithm(bool useColorAlgorithm);
    void setH(float h);
    void setHColor(float hColor);
    void setTemplateWindowSize(int templateWindowSize);
    void setSearchWindowSize(int searchWindowSize);

signals:
    void colorAlgorithmChanged();
    void hChanged();
    void hColorChanged();
    void templateWindowSizeChanged();
    void searchWindowSizeChanged();

protected:
    bool autoDetectColor() const;

private:
    bool m_useColorAlgorithm;
    bool m_autoDetectColor;
    float m_h;
    float m_hColor;
    int m_templateWindowSize;
    int m_searchWindowSize;
};

inline bool QFastNlMeansDenoising::colorAlgorithm() const{
    return m_useColorAlgorithm;
}

inline bool QFastNlMeansDenoising::autoDetectColor() const{
    return m_autoDetectColor;
}

inline float QFastNlMeansDenoising::h() const{
    return m_h;
}

inline float QFastNlMeansDenoising::hColor() const{
    return m_hColor;
}

inline int QFastNlMeansDenoising::templateWindowSize() const{
    return m_templateWindowSize;
}

inline int QFastNlMeansDenoising::searchWindowSize() const{
    return m_searchWindowSize;
}

inline void QFastNlMeansDenoising::setColorAlgorithm(bool useColorAlgorithm){
    m_autoDetectColor = false;
    if ( m_useColorAlgorithm != useColorAlgorithm ){
        m_useColorAlgorithm = useColorAlgorithm;
        emit colorAlgorithmChanged();
    }
}

inline void QFastNlMeansDenoising::setH(float h){
    if ( m_h != h ){
        m_h = h;
        emit hChanged();
    }
}

inline void QFastNlMeansDenoising::setHColor(float hColor){
    if ( m_hColor != hColor ){
        m_hColor = hColor;
        emit hColorChanged();
    }
}

inline void QFastNlMeansDenoising::setTemplateWindowSize(int templateWindowSize){
    if ( m_templateWindowSize != templateWindowSize ){
        m_templateWindowSize = templateWindowSize;
        emit templateWindowSizeChanged();
        QMatFilter::transform();
    }
}

inline void QFastNlMeansDenoising::setSearchWindowSize(int searchWindowSize){
    if ( m_searchWindowSize != searchWindowSize ){
        m_searchWindowSize = searchWindowSize;
        emit searchWindowSizeChanged();
        QMatFilter::transform();
    }
}

#endif // QFASTNLMEANSDENOISING_H
