/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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
#include "qcvtcolor.h"
#include "opencv2/imgproc.hpp"

using namespace cv;

/*!
  \qmltype CvtColor
  \instantiates QCvtColor
  \inqmlmodule lcvimgproc
  \inherits MatFilter
  \brief Converts an image from one color space to another.

  \quotefile imgproc/threshold.qml
*/

/*!
  \class QCvtColor
  \inmodule lcvimgproc_cpp
  \internal
  \brief Converts an image from one color space to another
 */

QCvtColor::QCvtColor(QQuickItem *parent) :
    QMatFilter(parent)
{
}

/*!
  \qmlproperty enumeration CvtColor::CvtType

  Conversion method:
  \list
  \li CV_BGR2BGRA
  \li CV_RGB2RGBA
  \li CV_BGRA2BGR
  \li CV_RGBA2RGB

  \li CV_BGR2RGBA
  \li CV_RGB2BGRA

  \li CV_RGBA2BGR
  \li CV_BGRA2RGB

  \li CV_BGR2RGB
  \li CV_RGB2BGR

  \li CV_BGRA2RGBA
  \li CV_RGBA2BGRA

  \li CV_BGR2GRAY
  \li CV_RGB2GRAY
  \li CV_GRAY2BGR
  \li CV_GRAY2RGB
  \li CV_GRAY2BGRA
  \li CV_GRAY2RGBA
  \li CV_BGRA2GRAY
  \li CV_RGBA2GRAY

  \li CV_BGR2BGR565
  \li CV_RGB2BGR565
  \li CV_BGR5652BGR
  \li CV_BGR5652RGB

  \li CV_BGRA2BGR565
  \li CV_RGBA2BGR565
  \li CV_BGR5652BGRA
  \li CV_BGR5652RGBA
  \li CV_GRAY2BGR565
  \li CV_BGR5652GRAY

  \li CV_BGR2BGR555
  \li CV_RGB2BGR555
  \li CV_BGR5552BGR
  \li CV_BGR5552RGB
  \li CV_BGRA2BGR555
  \li CV_RGBA2BGR555
  \li CV_BGR5552BGRA
  \li CV_BGR5552RGBA
  \li CV_GRAY2BGR555
  \li CV_BGR5552GRAY

  \li CV_BGR2XYZ
  \li CV_RGB2XYZ
  \li CV_XYZ2BGR
  \li CV_XYZ2RGB

  \li CV_BGR2YCrCb
  \li CV_RGB2YCrCb
  \li CV_YCrCb2BGR
  \li CV_YCrCb2RGB

  \li CV_BGR2HSV
  \li CV_RGB2HSV

  \li CV_BGR2Lab
  \li CV_RGB2Lab

  \li CV_BayerBG2BGR
  \li CV_BayerGB2BGR
  \li CV_BayerRG2BGR
  \li CV_BayerGR2BGR

  \li CV_BayerBG2RGB
  \li CV_BayerGB2RGB
  \li CV_BayerRG2RGB
  \li CV_BayerGR2RGB

  \li CV_BGR2Luv
  \li CV_RGB2Luv
  \li CV_BGR2HLS
  \li CV_RGB2HLS

  \li CV_HSV2BGR
  \li CV_HSV2RGB

  \li CV_Lab2BGR
  \li CV_Lab2RGB
  \li CV_Luv2BGR
  \li CV_Luv2RGB
  \li CV_HLS2BGR
  \li CV_HLS2RGB

  \li CV_BayerBG2BGR_VNG
  \li CV_BayerGB2BGR_VNG
  \li CV_BayerRG2BGR_VNG
  \li CV_BayerGR2BGR_VNG

  \li CV_BayerBG2RGB_VNG
  \li CV_BayerGB2RGB_VNG
  \li CV_BayerRG2RGB_VNG
  \li CV_BayerGR2RGB_VNG

  \li CV_BGR2HSV_FULL
  \li CV_RGB2HSV_FULL
  \li CV_BGR2HLS_FULL
  \li CV_RGB2HLS_FULL

  \li CV_HSV2BGR_FULL
  \li CV_HSV2RGB_FULL
  \li CV_HLS2BGR_FULL
  \li CV_HLS2RGB_FULL

  \li CV_LBGR2Lab
  \li CV_LRGB2Lab
  \li CV_LBGR2Luv
  \li CV_LRGB2Luv

  \li CV_Lab2LBGR
  \li CV_Lab2LRGB
  \li CV_Luv2LBGR
  \li CV_Luv2LRGB

  \li CV_BGR2YUV
  \li CV_RGB2YUV
  \li CV_YUV2BGR
  \li CV_YUV2RGB

  \li CV_BayerBG2GRAY
  \li CV_BayerGB2GRAY
  \li CV_BayerRG2GRAY
  \li CV_BayerGR2GRAY

  \li CV_YUV2RGB_NV12
  \li CV_YUV2BGR_NV12
  \li CV_YUV2RGB_NV21
  \li CV_YUV2BGR_NV21
  \li CV_YUV420sp2RGB
  \li CV_YUV420sp2BGR

  \li CV_YUV2RGBA_NV12
  \li CV_YUV2BGRA_NV12
  \li CV_YUV2RGBA_NV21
  \li CV_YUV2BGRA_NV21
  \li CV_YUV420sp2RGBA
  \li CV_YUV420sp2BGRA
  \li CV_YUV2RGB_YV12
  \li CV_YUV2BGR_YV12
  \li CV_YUV2RGB_IYUV
  \li CV_YUV2BGR_IYUV
  \li CV_YUV2RGB_I420
  \li CV_YUV2BGR_I420
  \li CV_YUV420p2RGB
  \li CV_YUV420p2BGR

  \li CV_YUV2RGBA_YV12
  \li CV_YUV2BGRA_YV12
  \li CV_YUV2RGBA_IYUV
  \li CV_YUV2BGRA_IYUV
  \li CV_YUV2RGBA_I420
  \li CV_YUV2BGRA_I420
  \li CV_YUV420p2RGBA
  \li CV_YUV420p2BGRA

  \li CV_YUV2GRAY_420
  \li CV_YUV2GRAY_NV21
  \li CV_YUV2GRAY_NV12
  \li CV_YUV2GRAY_YV12
  \li CV_YUV2GRAY_IYUV
  \li CV_YUV2GRAY_I420
  \li CV_YUV420sp2GRAY
  \li CV_YUV420p2GRAY

  \li CV_YUV2RGB_UYVY
  \li CV_YUV2BGR_UYVY
  \li CV_YUV2RGB_Y422
  \li CV_YUV2BGR_Y422
  \li CV_YUV2RGB_UYNV
  \li CV_YUV2BGR_UYNV

  \li CV_YUV2RGBA_UYVY
  \li CV_YUV2BGRA_UYVY
  \li CV_YUV2RGBA_Y422
  \li CV_YUV2BGRA_Y422
  \li CV_YUV2RGBA_UYNV
  \li CV_YUV2BGRA_UYNV

  \li CV_YUV2RGB_YUY2
  \li CV_YUV2BGR_YUY2
  \li CV_YUV2RGB_YVYU
  \li CV_YUV2BGR_YVYU
  \li CV_YUV2RGB_YUYV
  \li CV_YUV2BGR_YUYV
  \li CV_YUV2RGB_YUNV
  \li CV_YUV2BGR_YUNV

  \li CV_YUV2RGBA_YUY2
  \li CV_YUV2BGRA_YUY2
  \li CV_YUV2RGBA_YVYU
  \li CV_YUV2BGRA_YVYU
  \li CV_YUV2RGBA_YUYV
  \li CV_YUV2BGRA_YUYV
  \li CV_YUV2RGBA_YUNV
  \li CV_YUV2BGRA_YUNV

  \li CV_YUV2GRAY_UYVY
  \li CV_YUV2GRAY_YUY2
  \li CV_YUV2GRAY_Y422
  \li CV_YUV2GRAY_UYNV
  \li CV_YUV2GRAY_YVYU
  \li CV_YUV2GRAY_YUYV
  \li CV_YUV2GRAY_YUNV

  \li CV_RGBA2mRGBA
  \li CV_mRGBA2RGBA

  \li CV_COLORCVT_MAX
  \endlist
*/


/*!
  \enum QCvtColor::CvtType

  \value CV_BGR2BGRA
  \value CV_RGB2RGBA
  \value CV_BGRA2BGR
  \value CV_RGBA2RGB

  \value CV_BGR2RGBA
  \value CV_RGB2BGRA

  \value CV_RGBA2BGR
  \value CV_BGRA2RGB

  \value CV_BGR2RGB
  \value CV_RGB2BGR

  \value CV_BGRA2RGBA
  \value CV_RGBA2BGRA

  \value CV_BGR2GRAY
  \value CV_RGB2GRAY
  \value CV_GRAY2BGR
  \value CV_GRAY2RGB
  \value CV_GRAY2BGRA
  \value CV_GRAY2RGBA
  \value CV_BGRA2GRAY
  \value CV_RGBA2GRAY

  \value CV_BGR2BGR565
  \value CV_RGB2BGR565
  \value CV_BGR5652BGR
  \value CV_BGR5652RGB

  \value CV_BGRA2BGR565
  \value CV_RGBA2BGR565
  \value CV_BGR5652BGRA
  \value CV_BGR5652RGBA
  \value CV_GRAY2BGR565
  \value CV_BGR5652GRAY

  \value CV_BGR2BGR555
  \value CV_RGB2BGR555
  \value CV_BGR5552BGR
  \value CV_BGR5552RGB
  \value CV_BGRA2BGR555
  \value CV_RGBA2BGR555
  \value CV_BGR5552BGRA
  \value CV_BGR5552RGBA
  \value CV_GRAY2BGR555
  \value CV_BGR5552GRAY

  \value CV_BGR2XYZ
  \value CV_RGB2XYZ
  \value CV_XYZ2BGR
  \value CV_XYZ2RGB

  \value CV_BGR2YCrCb
  \value CV_RGB2YCrCb
  \value CV_YCrCb2BGR
  \value CV_YCrCb2RGB

  \value CV_BGR2HSV
  \value CV_RGB2HSV

  \value CV_BGR2Lab
  \value CV_RGB2Lab

  \value CV_BayerBG2BGR
  \value CV_BayerGB2BGR
  \value CV_BayerRG2BGR
  \value CV_BayerGR2BGR

  \value CV_BayerBG2RGB
  \value CV_BayerGB2RGB
  \value CV_BayerRG2RGB
  \value CV_BayerGR2RGB

  \value CV_BGR2Luv
  \value CV_RGB2Luv
  \value CV_BGR2HLS
  \value CV_RGB2HLS

  \value CV_HSV2BGR
  \value CV_HSV2RGB

  \value CV_Lab2BGR
  \value CV_Lab2RGB
  \value CV_Luv2BGR
  \value CV_Luv2RGB
  \value CV_HLS2BGR
  \value CV_HLS2RGB

  \value CV_BayerBG2BGR_VNG
  \value CV_BayerGB2BGR_VNG
  \value CV_BayerRG2BGR_VNG
  \value CV_BayerGR2BGR_VNG

  \value CV_BayerBG2RGB_VNG
  \value CV_BayerGB2RGB_VNG
  \value CV_BayerRG2RGB_VNG
  \value CV_BayerGR2RGB_VNG

  \value CV_BGR2HSV_FULL
  \value CV_RGB2HSV_FULL
  \value CV_BGR2HLS_FULL
  \value CV_RGB2HLS_FULL

  \value CV_HSV2BGR_FULL
  \value CV_HSV2RGB_FULL
  \value CV_HLS2BGR_FULL
  \value CV_HLS2RGB_FULL

  \value CV_LBGR2Lab
  \value CV_LRGB2Lab
  \value CV_LBGR2Luv
  \value CV_LRGB2Luv

  \value CV_Lab2LBGR
  \value CV_Lab2LRGB
  \value CV_Luv2LBGR
  \value CV_Luv2LRGB

  \value CV_BGR2YUV
  \value CV_RGB2YUV
  \value CV_YUV2BGR
  \value CV_YUV2RGB

  \value CV_BayerBG2GRAY
  \value CV_BayerGB2GRAY
  \value CV_BayerRG2GRAY
  \value CV_BayerGR2GRAY

  \value CV_YUV2RGB_NV12
  \value CV_YUV2BGR_NV12
  \value CV_YUV2RGB_NV21
  \value CV_YUV2BGR_NV21
  \value CV_YUV420sp2RGB
  \value CV_YUV420sp2BGR

  \value CV_YUV2RGBA_NV12
  \value CV_YUV2BGRA_NV12
  \value CV_YUV2RGBA_NV21
  \value CV_YUV2BGRA_NV21
  \value CV_YUV420sp2RGBA
  \value CV_YUV420sp2BGRA
  \value CV_YUV2RGB_YV12
  \value CV_YUV2BGR_YV12
  \value CV_YUV2RGB_IYUV
  \value CV_YUV2BGR_IYUV
  \value CV_YUV2RGB_I420
  \value CV_YUV2BGR_I420
  \value CV_YUV420p2RGB
  \value CV_YUV420p2BGR

  \value CV_YUV2RGBA_YV12
  \value CV_YUV2BGRA_YV12
  \value CV_YUV2RGBA_IYUV
  \value CV_YUV2BGRA_IYUV
  \value CV_YUV2RGBA_I420
  \value CV_YUV2BGRA_I420
  \value CV_YUV420p2RGBA
  \value CV_YUV420p2BGRA

  \value CV_YUV2GRAY_420
  \value CV_YUV2GRAY_NV21
  \value CV_YUV2GRAY_NV12
  \value CV_YUV2GRAY_YV12
  \value CV_YUV2GRAY_IYUV
  \value CV_YUV2GRAY_I420
  \value CV_YUV420sp2GRAY
  \value CV_YUV420p2GRAY

  \value CV_YUV2RGB_UYVY
  \value CV_YUV2BGR_UYVY
  \value CV_YUV2RGB_Y422
  \value CV_YUV2BGR_Y422
  \value CV_YUV2RGB_UYNV
  \value CV_YUV2BGR_UYNV

  \value CV_YUV2RGBA_UYVY
  \value CV_YUV2BGRA_UYVY
  \value CV_YUV2RGBA_Y422
  \value CV_YUV2BGRA_Y422
  \value CV_YUV2RGBA_UYNV
  \value CV_YUV2BGRA_UYNV

  \value CV_YUV2RGB_YUY2
  \value CV_YUV2BGR_YUY2
  \value CV_YUV2RGB_YVYU
  \value CV_YUV2BGR_YVYU
  \value CV_YUV2RGB_YUYV
  \value CV_YUV2BGR_YUYV
  \value CV_YUV2RGB_YUNV
  \value CV_YUV2BGR_YUNV

  \value CV_YUV2RGBA_YUY2
  \value CV_YUV2BGRA_YUY2
  \value CV_YUV2RGBA_YVYU
  \value CV_YUV2BGRA_YVYU
  \value CV_YUV2RGBA_YUYV
  \value CV_YUV2BGRA_YUYV
  \value CV_YUV2RGBA_YUNV
  \value CV_YUV2BGRA_YUNV

  \value CV_YUV2GRAY_UYVY
  \value CV_YUV2GRAY_YUY2
  \value CV_YUV2GRAY_Y422
  \value CV_YUV2GRAY_UYNV
  \value CV_YUV2GRAY_YVYU
  \value CV_YUV2GRAY_YUYV
  \value CV_YUV2GRAY_YUNV

  \value CV_RGBA2mRGBA
  \value CV_mRGBA2RGBA

  \value CV_COLORCVT_MAX
 */
enum CvtType{



};


/*!
  \qmlproperty CvtColor::CvtType CvtColor::code

  Color space conversion code.
 */


/*!
  \qmlproperty int CvtColor::dstCn

  Number of channels in the destination image; if the parameter is 0, the number of the channels is derived
  automatically from input and code.
 */


/*!
  \brief Filtering function.

  Parameters:
  \a in
  \a out
 */
void QCvtColor::transform(cv::Mat &in, cv::Mat &out){
    cvtColor(in, out, m_code, m_dstCn);
}
