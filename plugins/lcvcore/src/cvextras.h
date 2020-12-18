#ifndef LVCVEXTRAS_H
#define LVCVEXTRAS_H

#include "opencv2/core.hpp"

namespace lv{

class CvExtras{

public:
    CvExtras();

    static void copyTo4Channels(cv::Mat& src, cv::Mat& dst);
    static void blendTo3Channels(cv::Mat& src, cv::Mat& dst);
};

}// namespace

#endif // CVEXTRAS_H
