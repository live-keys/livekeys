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

#include "qhuesaturationlightness.h"
#include "opencv2/imgproc.hpp"

using namespace cv;

QHueSaturationLightness::QHueSaturationLightness(QQuickItem *item)
    : QMatFilter(item)
    , m_hue(180)
    , m_saturation(100)
    , m_lightness(100)
{
}

QHueSaturationLightness::~QHueSaturationLightness(){
}

// From [answers.opencv.org/answers/178953/revisions] with some optimizations
void QHueSaturationLightness::transform(const cv::Mat &in, cv::Mat &out){
    if ( (in.channels() != 3 && in.channels() != 4) || in.depth() != CV_8U )
        return;

    cvtColor(in, m_middle, cv::COLOR_BGR2HSV);
    signed short hue_shift = (m_hue - 180) / 2;

    for (int y = 0; y < in.rows; y++){
        uchar* p = m_middle.ptr<uchar>(y);

        for (int x = 0; x < in.cols; x++){
            // hue
            signed short h = p[x * 3 + 0];
            signed short h_plus_shift = h;
            h_plus_shift += hue_shift;

            if (h_plus_shift < 0)
                h = 180 + h_plus_shift;
            else if (h_plus_shift > 180)
                h = h_plus_shift - 180;
            else
                h = h_plus_shift;

            p[x * 3 + 0] = static_cast<uchar>(h);

            // saturation
            double s = p[x * 3 + 1];
            double s_shift = (m_saturation - 100) / 100.0;
            double s_plus_shift = s + 255.0 * s_shift;

            if (s_plus_shift < 0)
                s_plus_shift = 0;
            else if (s_plus_shift > 255)
                s_plus_shift = 255;

            p[x * 3 + 1] = static_cast<unsigned char>(s_plus_shift);

            // lightness
            double lv = p[x * 3 + 2];
            double v_plus_shift = lv + 255.0 * ((m_lightness - 100) / 100.0);

            if (v_plus_shift < 0)
                v_plus_shift = 0;
            else if (v_plus_shift > 255)
                v_plus_shift = 255;

            p[x * 3 + 2] = static_cast<unsigned char>(v_plus_shift);
        }
    }

    cvtColor(m_middle, out, cv::COLOR_HSV2BGR);
}
