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

    cvtColor(in, m_middle, CV_BGR2HSV);
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

    cvtColor(m_middle, out, CV_HSV2BGR);
}
