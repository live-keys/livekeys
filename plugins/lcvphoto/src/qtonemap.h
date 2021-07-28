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

#ifndef QTONEMAP_H
#define QTONEMAP_H

#include <QQuickItem>
#include "opencv2/photo.hpp"

#include "qmat.h"
#include "qmatdisplay.h"

class QTonemap : public QObject{

    Q_OBJECT
    Q_PROPERTY(float gamma READ gamma WRITE setGamma NOTIFY gammaChanged)
public:
    QTonemap(QObject* parent = nullptr);
    QTonemap(cv::Ptr<cv::Tonemap> tonemapper, QObject* parent = nullptr);
    virtual ~QTonemap();


    float gamma() const;
    void setGamma(float gamma);
protected:
    cv::Ptr<cv::Tonemap> tonemapper();
    void initializeTonemapper(cv::Ptr<cv::Tonemap> tonemapper);
signals:
    void gammaChanged();
public slots:
    QMat* process(QMat* input);
private:
    float                m_gamma;
    cv::Ptr<cv::Tonemap> m_tonemapper;
};

inline cv::Ptr<cv::Tonemap> QTonemap::tonemapper(){
    return m_tonemapper;
}
#endif // QTONEMAP_H
