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

#ifndef QSTITCHER_H
#define QSTITCHER_H

#include <QQuickItem>
#include "opencv2/stitching.hpp"
#include "live/qmlobjectlist.h"
#include "qmat.h"
class QStitcher : public QObject{

    Q_OBJECT
    Q_PROPERTY(int                mode      READ mode      WRITE setMode      NOTIFY modeChanged)
    Q_PROPERTY(bool               tryUseGpu READ tryUseGpu WRITE setTryUseGpu NOTIFY tryUseGpuChanged)
public:
    enum Mode{
        Panorama = cv::Stitcher::PANORAMA,
        Scans = cv::Stitcher::SCANS
    };
    Q_ENUMS(Mode)

    enum Status{
        Ok = cv::Stitcher::OK,
        ErrNeedMoreFiles = cv::Stitcher::ERR_NEED_MORE_IMGS,
        ErrHomographyEstFail = cv::Stitcher::ERR_HOMOGRAPHY_EST_FAIL,
        ErrCameraParamsAdjustFail = cv::Stitcher::ERR_CAMERA_PARAMS_ADJUST_FAIL
    };
    Q_ENUMS(Status)

public:
    QStitcher(QObject* parent = nullptr);

    int mode() const;
    void setMode(int mode);

    bool tryUseGpu() const;
    void setTryUseGpu(bool tryUseGpu);

signals:
    void modeChanged();
    void tryUseGpuChanged();
    void error(int status);

public slots:
    QMat* stitch(lv::QmlObjectList* input);

private:
    void createSticher();
    lv::QmlObjectList* m_input;
#if (CV_VERSION_MAJOR >= 3 && CV_VERSION_MINOR > 2) || CV_VERSION_MAJOR >= 4
    cv::Ptr<cv::Stitcher> m_stitcher;
#else
    cv::Stitcher m_stitcher;
#endif
    int m_mode;
    bool m_tryUseGpu;
};

#endif // QSTITCHER_H
