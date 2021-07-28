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

#ifndef QMERGEDEBEVEC_H
#define QMERGEDEBEVEC_H

#include <QObject>
#include <QQmlParserStatus>
#include "opencv2/photo.hpp"
#include "live/qmlobjectlist.h"
#include "qmat.h"

class QMergeDebevec : public QObject{

    Q_OBJECT
public:
    explicit QMergeDebevec(QObject *parent = nullptr);
    ~QMergeDebevec();
public slots:
    QMat* process(lv::QmlObjectList* input, QList<qreal> times, QMat* response);
private:
    cv::Ptr<cv::MergeDebevec> m_mergeDebevec;
};


#endif // QMERGEDEBEVEC_H
