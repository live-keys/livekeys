#include "cvextras.h"
#include "opencv2/imgproc.hpp"

#include <limits>

namespace lv{

namespace{

template<typename T>
void copyTo4ChannelsImpl(cv::Mat &src, cv::Mat &dst){
    T* srcData = reinterpret_cast<T*>(src.data);
    int srcStep = (int)(src.step / sizeof(T));

    T* dstData  = reinterpret_cast<T*>(dst.data);
    int dstStep = (int)(dst.step / sizeof(T));

    if ( src.channels() == 1 ){

        for ( int i = 0; i < dst.rows; ++i ){
            int dstRowStep = i * dstStep;
            int srcRowStep = i * srcStep;
            for ( int j = 0; j < dst.cols; ++j ){
                dstData[dstRowStep + j * dst.channels() + 0] = srcData[srcRowStep + j];
                dstData[dstRowStep + j * dst.channels() + 1] = srcData[srcRowStep + j];
                dstData[dstRowStep + j * dst.channels() + 2] = srcData[srcRowStep + j];
                dstData[dstRowStep + j * dst.channels() + 3] = std::numeric_limits<T>::max();
            }
        }


    } else if ( src.channels() == 3 ){

        for ( int i = 0; i < dst.rows; ++i ){
            int dstRowStep = i * dstStep;
            int srcRowStep = i * srcStep;
            for ( int j = 0; j < dst.cols; ++j ){
                dstData[dstRowStep + j * dst.channels() + 0] = srcData[srcRowStep + j * 3 + 0];
                dstData[dstRowStep + j * dst.channels() + 1] = srcData[srcRowStep + j * 3 + 1];
                dstData[dstRowStep + j * dst.channels() + 2] = srcData[srcRowStep + j * 3 + 2];
                dstData[dstRowStep + j * dst.channels() + 3] = std::numeric_limits<T>::max();
            }
        }

    } else if ( src.channels() == 4 ){
        src.copyTo(dst);
    }
}

template<typename T>
void blendTo3ChannelsImpl(cv::Mat &src, cv::Mat &dst){
    T* srcData = reinterpret_cast<T*>(src.data);
    int srcStep = (int)(src.step / sizeof(T));

    T* dstData  = reinterpret_cast<T*>(dst.data);
    int dstStep = (int)(dst.step / sizeof(T));

    if ( src.channels() == 1 ){

        for ( int i = 0; i < dst.rows; ++i ){
            int dstRowStep = i * dstStep;
            int srcRowStep = i * srcStep;
            for ( int j = 0; j < dst.cols; ++j ){
                dstData[dstRowStep + j * dst.channels() + 0] = srcData[srcRowStep + j];
                dstData[dstRowStep + j * dst.channels() + 1] = srcData[srcRowStep + j];
                dstData[dstRowStep + j * dst.channels() + 2] = srcData[srcRowStep + j];
            }
        }


    } else if ( src.channels() == 3 ){
        src.copyTo(dst);


    } else if ( src.channels() == 4 ){

        //TODO: Capture opacity

        for ( int i = 0; i < dst.rows; ++i ){
            int dstRowStep = i * dstStep;
            int srcRowStep = i * srcStep;
            for ( int j = 0; j < dst.cols; ++j ){
                dstData[dstRowStep + j * dst.channels() + 0] = srcData[srcRowStep + j * 4 + 0];
                dstData[dstRowStep + j * dst.channels() + 1] = srcData[srcRowStep + j * 4 + 1];
                dstData[dstRowStep + j * dst.channels() + 2] = srcData[srcRowStep + j * 4 + 2];
            }
        }
    }
}

} // namespace




CvExtras::CvExtras()
{

}

void CvExtras::copyTo4Channels(cv::Mat &src, cv::Mat &dst){
    switch(src.depth()){
    case CV_8U :  copyTo4ChannelsImpl<unsigned char>(src, dst); break;
    case CV_8S :  copyTo4ChannelsImpl<char>(src, dst); break;
    case CV_16U : copyTo4ChannelsImpl<unsigned short>(src, dst); break;
    case CV_16S : copyTo4ChannelsImpl<short>(src, dst); break;
    case CV_32S : copyTo4ChannelsImpl<long>(src, dst); break;
    case CV_32F : copyTo4ChannelsImpl<float>(src, dst); break;
    case CV_64F : copyTo4ChannelsImpl<double>(src, dst); break;
    }

}

void CvExtras::blendTo3Channels(cv::Mat &src, cv::Mat &dst){
    switch(src.depth()){
    case CV_8U :  blendTo3ChannelsImpl<unsigned char>(src, dst); break;
    case CV_8S :  blendTo3ChannelsImpl<char>(src, dst); break;
    case CV_16U : blendTo3ChannelsImpl<unsigned short>(src, dst); break;
    case CV_16S : blendTo3ChannelsImpl<short>(src, dst); break;
    case CV_32S : blendTo3ChannelsImpl<long>(src, dst); break;
    case CV_32F : blendTo3ChannelsImpl<float>(src, dst); break;
    case CV_64F : blendTo3ChannelsImpl<double>(src, dst); break;
    }
}

}// namepsace
