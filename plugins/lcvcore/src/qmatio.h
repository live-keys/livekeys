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

#ifndef QMATIO_H
#define QMATIO_H

#include <QObject>
#include "qmat.h"

/// \private
class QMatIO : public QObject{

    Q_OBJECT

    Q_ENUMS(Load)

public:
    enum Load{
        CV_LOAD_IMAGE_UNCHANGED  = -1,
        CV_LOAD_IMAGE_GRAYSCALE  =  0,
        CV_LOAD_IMAGE_COLOR      =  1,
        CV_LOAD_IMAGE_ANYDEPTH   =  2,
        CV_LOAD_IMAGE_ANYCOLOR   =  4
    };

public:
    explicit QMatIO(QObject *parent = nullptr);

    static cv::Mat fromQImage(const QImage& inImage, bool cloneImageData = true);

public slots:
    QMat* read(const QString& path, int isColor = CV_LOAD_IMAGE_COLOR);
    QMat* decode(const QByteArray& bytes, int isColor = CV_LOAD_IMAGE_COLOR);
    bool write(const QString& file, QMat* image, QJSValue options = QJSValue());

};

#endif // QMATIO_H
