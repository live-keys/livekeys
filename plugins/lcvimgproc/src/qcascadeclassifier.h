/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef QCASCADECLASSIFIER_H
#define QCASCADECLASSIFIER_H

#include <QObject>
#include "opencv2/objdetect.hpp"
#include "qmat.h"
#include <QVariantList>

class QCascadeClassifier: public QObject
{
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_OBJECT
public:
    QCascadeClassifier();

    void setFile(QString& fileString);
    QString file();
public slots:
    QVariantList detectFaces(QMat* input, double scaleFactor = 1.1, int minNeighbors = 3, int flags = 0);
signals:
    void fileChanged();
private:
    cv::CascadeClassifier* m_cc;
    QString m_file;
};

inline QString QCascadeClassifier::file()
{
    return m_file;
}

#endif // QCASCADECLASSIFIER_H
