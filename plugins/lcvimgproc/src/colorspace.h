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

#ifndef COLORSPACE_H
#define COLORSPACE_H

#include <QObject>
#include "qmat.h"
#include "opencv2/imgproc.hpp"
#include "live/viewengine.h"

class ColorSpace: public QObject
{
    Q_OBJECT
    Q_ENUMS(CvtType)
public:
    enum CvtType{
        CV_BGR2BGRA    =0,
        CV_RGB2RGBA    =CV_BGR2BGRA,

        CV_BGRA2BGR    =1,
        CV_RGBA2RGB    =CV_BGRA2BGR,

        CV_BGR2RGBA    =2,
        CV_RGB2BGRA    =CV_BGR2RGBA,

        CV_RGBA2BGR    =3,
        CV_BGRA2RGB    =CV_RGBA2BGR,

        CV_BGR2RGB     =4,
        CV_RGB2BGR     =CV_BGR2RGB,

        CV_BGRA2RGBA   =5,
        CV_RGBA2BGRA   =CV_BGRA2RGBA,

        CV_BGR2GRAY    =6,
        CV_RGB2GRAY    =7,
        CV_GRAY2BGR    =8,
        CV_GRAY2RGB    =CV_GRAY2BGR,
        CV_GRAY2BGRA   =9,
        CV_GRAY2RGBA   =CV_GRAY2BGRA,
        CV_BGRA2GRAY   =10,
        CV_RGBA2GRAY   =11,

        CV_BGR2BGR565  =12,
        CV_RGB2BGR565  =13,
        CV_BGR5652BGR  =14,
        CV_BGR5652RGB  =15,
        CV_BGRA2BGR565 =16,
        CV_RGBA2BGR565 =17,
        CV_BGR5652BGRA =18,
        CV_BGR5652RGBA =19,

        CV_GRAY2BGR565 =20,
        CV_BGR5652GRAY =21,

        CV_BGR2BGR555  =22,
        CV_RGB2BGR555  =23,
        CV_BGR5552BGR  =24,
        CV_BGR5552RGB  =25,
        CV_BGRA2BGR555 =26,
        CV_RGBA2BGR555 =27,
        CV_BGR5552BGRA =28,
        CV_BGR5552RGBA =29,

        CV_GRAY2BGR555 =30,
        CV_BGR5552GRAY =31,

        CV_BGR2XYZ     =32,
        CV_RGB2XYZ     =33,
        CV_XYZ2BGR     =34,
        CV_XYZ2RGB     =35,

        CV_BGR2YCrCb   =36,
        CV_RGB2YCrCb   =37,
        CV_YCrCb2BGR   =38,
        CV_YCrCb2RGB   =39,

        CV_BGR2HSV     =40,
        CV_RGB2HSV     =41,

        CV_BGR2Lab     =44,
        CV_RGB2Lab     =45,

        CV_BayerBG2BGR =46,
        CV_BayerGB2BGR =47,
        CV_BayerRG2BGR =48,
        CV_BayerGR2BGR =49,

        CV_BayerBG2RGB =CV_BayerRG2BGR,
        CV_BayerGB2RGB =CV_BayerGR2BGR,
        CV_BayerRG2RGB =CV_BayerBG2BGR,
        CV_BayerGR2RGB =CV_BayerGB2BGR,

        CV_BGR2Luv     =50,
        CV_RGB2Luv     =51,
        CV_BGR2HLS     =52,
        CV_RGB2HLS     =53,

        CV_HSV2BGR     =54,
        CV_HSV2RGB     =55,

        CV_Lab2BGR     =56,
        CV_Lab2RGB     =57,
        CV_Luv2BGR     =58,
        CV_Luv2RGB     =59,
        CV_HLS2BGR     =60,
        CV_HLS2RGB     =61,

        CV_BayerBG2BGR_VNG =62,
        CV_BayerGB2BGR_VNG =63,
        CV_BayerRG2BGR_VNG =64,
        CV_BayerGR2BGR_VNG =65,

        CV_BayerBG2RGB_VNG =CV_BayerRG2BGR_VNG,
        CV_BayerGB2RGB_VNG =CV_BayerGR2BGR_VNG,
        CV_BayerRG2RGB_VNG =CV_BayerBG2BGR_VNG,
        CV_BayerGR2RGB_VNG =CV_BayerGB2BGR_VNG,

        CV_BGR2HSV_FULL = 66,
        CV_RGB2HSV_FULL = 67,
        CV_BGR2HLS_FULL = 68,
        CV_RGB2HLS_FULL = 69,

        CV_HSV2BGR_FULL = 70,
        CV_HSV2RGB_FULL = 71,
        CV_HLS2BGR_FULL = 72,
        CV_HLS2RGB_FULL = 73,

        CV_LBGR2Lab     = 74,
        CV_LRGB2Lab     = 75,
        CV_LBGR2Luv     = 76,
        CV_LRGB2Luv     = 77,

        CV_Lab2LBGR     = 78,
        CV_Lab2LRGB     = 79,
        CV_Luv2LBGR     = 80,
        CV_Luv2LRGB     = 81,

        CV_BGR2YUV      = 82,
        CV_RGB2YUV      = 83,
        CV_YUV2BGR      = 84,
        CV_YUV2RGB      = 85,

        CV_BayerBG2GRAY = 86,
        CV_BayerGB2GRAY = 87,
        CV_BayerRG2GRAY = 88,
        CV_BayerGR2GRAY = 89,

        //YUV 4:2:0 formats family
        CV_YUV2RGB_NV12 = 90,
        CV_YUV2BGR_NV12 = 91,
        CV_YUV2RGB_NV21 = 92,
        CV_YUV2BGR_NV21 = 93,
        CV_YUV420sp2RGB = CV_YUV2RGB_NV21,
        CV_YUV420sp2BGR = CV_YUV2BGR_NV21,

        CV_YUV2RGBA_NV12 = 94,
        CV_YUV2BGRA_NV12 = 95,
        CV_YUV2RGBA_NV21 = 96,
        CV_YUV2BGRA_NV21 = 97,
        CV_YUV420sp2RGBA = CV_YUV2RGBA_NV21,
        CV_YUV420sp2BGRA = CV_YUV2BGRA_NV21,

        CV_YUV2RGB_YV12 = 98,
        CV_YUV2BGR_YV12 = 99,
        CV_YUV2RGB_IYUV = 100,
        CV_YUV2BGR_IYUV = 101,
        CV_YUV2RGB_I420 = CV_YUV2RGB_IYUV,
        CV_YUV2BGR_I420 = CV_YUV2BGR_IYUV,
        CV_YUV420p2RGB = CV_YUV2RGB_YV12,
        CV_YUV420p2BGR = CV_YUV2BGR_YV12,

        CV_YUV2RGBA_YV12 = 102,
        CV_YUV2BGRA_YV12 = 103,
        CV_YUV2RGBA_IYUV = 104,
        CV_YUV2BGRA_IYUV = 105,
        CV_YUV2RGBA_I420 = CV_YUV2RGBA_IYUV,
        CV_YUV2BGRA_I420 = CV_YUV2BGRA_IYUV,
        CV_YUV420p2RGBA = CV_YUV2RGBA_YV12,
        CV_YUV420p2BGRA = CV_YUV2BGRA_YV12,

        CV_YUV2GRAY_420 = 106,
        CV_YUV2GRAY_NV21 = CV_YUV2GRAY_420,
        CV_YUV2GRAY_NV12 = CV_YUV2GRAY_420,
        CV_YUV2GRAY_YV12 = CV_YUV2GRAY_420,
        CV_YUV2GRAY_IYUV = CV_YUV2GRAY_420,
        CV_YUV2GRAY_I420 = CV_YUV2GRAY_420,
        CV_YUV420sp2GRAY = CV_YUV2GRAY_420,
        CV_YUV420p2GRAY = CV_YUV2GRAY_420,

        //YUV 4:2:2 formats family
        CV_YUV2RGB_UYVY = 107,
        CV_YUV2BGR_UYVY = 108,
        //CV_YUV2RGB_VYUY = 109,
        //CV_YUV2BGR_VYUY = 110,
        CV_YUV2RGB_Y422 = CV_YUV2RGB_UYVY,
        CV_YUV2BGR_Y422 = CV_YUV2BGR_UYVY,
        CV_YUV2RGB_UYNV = CV_YUV2RGB_UYVY,
        CV_YUV2BGR_UYNV = CV_YUV2BGR_UYVY,

        CV_YUV2RGBA_UYVY = 111,
        CV_YUV2BGRA_UYVY = 112,
        //CV_YUV2RGBA_VYUY = 113,
        //CV_YUV2BGRA_VYUY = 114,
        CV_YUV2RGBA_Y422 = CV_YUV2RGBA_UYVY,
        CV_YUV2BGRA_Y422 = CV_YUV2BGRA_UYVY,
        CV_YUV2RGBA_UYNV = CV_YUV2RGBA_UYVY,
        CV_YUV2BGRA_UYNV = CV_YUV2BGRA_UYVY,

        CV_YUV2RGB_YUY2 = 115,
        CV_YUV2BGR_YUY2 = 116,
        CV_YUV2RGB_YVYU = 117,
        CV_YUV2BGR_YVYU = 118,
        CV_YUV2RGB_YUYV = CV_YUV2RGB_YUY2,
        CV_YUV2BGR_YUYV = CV_YUV2BGR_YUY2,
        CV_YUV2RGB_YUNV = CV_YUV2RGB_YUY2,
        CV_YUV2BGR_YUNV = CV_YUV2BGR_YUY2,

        CV_YUV2RGBA_YUY2 = 119,
        CV_YUV2BGRA_YUY2 = 120,
        CV_YUV2RGBA_YVYU = 121,
        CV_YUV2BGRA_YVYU = 122,
        CV_YUV2RGBA_YUYV = CV_YUV2RGBA_YUY2,
        CV_YUV2BGRA_YUYV = CV_YUV2BGRA_YUY2,
        CV_YUV2RGBA_YUNV = CV_YUV2RGBA_YUY2,
        CV_YUV2BGRA_YUNV = CV_YUV2BGRA_YUY2,

        CV_YUV2GRAY_UYVY = 123,
        CV_YUV2GRAY_YUY2 = 124,
        //CV_YUV2GRAY_VYUY = CV_YUV2GRAY_UYVY,
        CV_YUV2GRAY_Y422 = CV_YUV2GRAY_UYVY,
        CV_YUV2GRAY_UYNV = CV_YUV2GRAY_UYVY,
        CV_YUV2GRAY_YVYU = CV_YUV2GRAY_YUY2,
        CV_YUV2GRAY_YUYV = CV_YUV2GRAY_YUY2,
        CV_YUV2GRAY_YUNV = CV_YUV2GRAY_YUY2,

        // alpha premultiplication
        CV_RGBA2mRGBA = 125,
        CV_mRGBA2RGBA = 126,

        CV_COLORCVT_MAX  = 127
    };
    ColorSpace(QObject* parent = nullptr);

public slots:
    QMat* cvtColor(QMat* input, int code, int dstCn);

private:
    lv::ViewEngine* engine();
};

#endif // COLORSPACE_H
