/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Livekeys Application.
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

#ifndef QDMATCHVECTOR_H
#define QDMATCHVECTOR_H

#include <QQuickItem>
#include "qlcvfeatures2dglobal.h"
#include "opencv2/features2d.hpp"

class Q_LCVFEATURES2D_EXPORT QDMatchVector : public QQuickItem{

    Q_OBJECT

public:
    /**
    \brief Type enumeration
    */
    enum Type{
        BEST_MATCH = 0,
        KNN,
        RADIUS
    };

public:

    explicit QDMatchVector(QQuickItem *parent = 0);

    virtual ~QDMatchVector();

    /// \private
    std::vector<std::vector<cv::DMatch> >& matches();
    
    /// \private
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

#endif // QDMATCHVECTOR_H
