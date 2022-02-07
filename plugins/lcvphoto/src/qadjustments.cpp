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

#include "qadjustments.h"

#include "live/visuallogqt.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "live/exception.h"

#include <QJSValue>
#include <QJSValueIterator>

#include "opencv2/imgproc.hpp"

namespace{

// Levels class
// ------------------------------------------------------------------------

class Levels{

public:
    class Configuration{
    public:
        Configuration() : lowRange(0), highRange(255), midtonePoint(1.0){}

        int    lowRange;
        int    highRange;
        double midtonePoint;
    };

    static void createBlackAndWhiteLut(int histogramSize, int lowCut, int highCut, cv::Mat& lut){
        int totalSize = histogramSize;
        int totalItems = highCut - lowCut;

        lut.create(1, histogramSize, CV_8U);
        lut.setTo(0);
        uchar* pl = lut.ptr();

        if ( highCut > lowCut + 1 ){
            double ratio = (double)(totalSize - 1) / (double)(totalItems - 1);

            pl[lowCut] = 0;
            for ( int i = lowCut + 1; i < highCut; ++i ){
                pl[i] = std::round(ratio * (double)(i - lowCut));
            }
            for ( int i = highCut; i < lut.cols; ++i ){
                pl[i] = pl[highCut - 1];
            }
        }
    }

    static void createGammaLut(int lutSize, float fGamma, cv::Mat& lut){
        lut.create(1, lutSize, CV_8U);
        lut.setTo(0);
        uchar* pl = lut.ptr();

        for (int i = 0; i < 256; i++){
            pl[i] = cv::saturate_cast<uchar>(pow((float)(i / 255.0), 1/ fGamma) * 255.0f);
        }
    }

    static void applyLut(const cv::Mat& lut, int channel, cv::Mat& dst){
        const uchar* pl = lut.ptr();
        for ( int y = 0; y < dst.rows; ++y ){
            uchar* p = dst.ptr<uchar>(y);
            for ( int x = 0; x < dst.cols; ++x ){
                p[x * dst.channels() + channel] = pl[p[x * dst.channels() + channel]];
            }
        }
    }

};

}

// QAdjustments
// ---------------------------------------------------------------------

QAdjustments::QAdjustments(QObject *parent)
    : QObject(parent)
{

}

QAdjustments::~QAdjustments(){

}

QMat *QAdjustments::levels(QMat *source, const QJSValue &lightness, const QJSValue &channels){
    if (!source){
        return nullptr;
    }

    Levels::Configuration lightnessConfiguration;

    // Assign lightness
    if ( lightness.isArray() ){
        if ( lightness.property("length").toInt() == 3 ){
            Levels::Configuration cfg;
            cfg.lowRange = lightness.property(0).toInt();
            cfg.midtonePoint = lightness.property(1).toNumber();
            cfg.highRange = lightness.property(2).toInt();
            lightnessConfiguration = cfg;
        } else {
            lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Global channel must be array of size 3.", 0);
            lv::ViewContext::instance().engine()->throwError(&e);
            return nullptr;
        }
    } else if (!lightness.isUndefined() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Global channel must be array of size 3.", 0);
        lv::ViewContext::instance().engine()->throwError(&e, this);
        return nullptr;
    } else {
        lightnessConfiguration = Levels::Configuration();
    }

    QMap<int, Levels::Configuration> channelConfiguration;
    if ( channels.isObject() ){
        channelConfiguration.clear();
        QJSValueIterator it(channels);
        while ( it.hasNext() ){
            it.next();
            bool isKeyInt;
            int channelNo = it.name().toInt(&isKeyInt);
            if ( !isKeyInt ){
                lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Channels must be of object of <int, Array> type.", 0);
                lv::ViewContext::instance().engine()->throwError(&e);
                return nullptr;
            }
            QJSValue channel = it.value();
            if ( !channel.isArray() || channel.property("length").toInt() != 3 ){
                lv::Exception e = CREATE_EXCEPTION(
                    lv::Exception, "Channels must be of object of <int, Array> type, where Array is of size[3]", 0
                );
                lv::ViewContext::instance().engine()->throwError(&e);
                return nullptr;
            }

            Levels::Configuration cfg;
            cfg.lowRange = channel.property(0).toInt();
            cfg.midtonePoint = channel.property(1).toNumber();
            cfg.highRange = channel.property(2).toInt();
            channelConfiguration[channelNo] = cfg;
        }
    } else if (!channels.isUndefined() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Channels must be of object of <int, Array> type.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return nullptr;
    } else {
        channelConfiguration.clear();
    }

    QMat* m = new QMat(source->internalPtr()->cols, source->internalPtr()->rows, static_cast<QMat::Type>(source->depth()), source->channels(), nullptr);
    source->internalPtr()->copyTo(*m->internalPtr());

    // iterate channels first
    for ( auto it = channelConfiguration.begin(); it != channelConfiguration.end(); ++it ){
        const Levels::Configuration& cfg = it.value();
        if ( it.key() < source->internalPtr()->channels() ){
            if ( cfg.lowRange > 0 || cfg.highRange < 255 ){

                if ( cfg.lowRange < 253 && cfg.highRange > 0 ){
                    cv::Mat lut;
                    Levels::createBlackAndWhiteLut(256, cfg.lowRange, cfg.highRange, lut);
                    Levels::applyLut(lut, it.key(), *m->internalPtr());
                }
            }
            if ( cfg.midtonePoint != 1.0 ){
                cv::Mat lut;
                Levels::createGammaLut(256, cfg.midtonePoint, lut);
                Levels::applyLut(lut, it.key(), *m->internalPtr());
            }
        }
    }

    // adjust luminance channel
    const Levels::Configuration& cfg = lightnessConfiguration;
    if ( cfg.lowRange > 0 || cfg.highRange < 255 || cfg.midtonePoint != 1.0 ){
        int channel = 1;

        if ( m->internalPtr()->channels() == 3 )
            cv::cvtColor(*m->internalPtr(), *m->internalPtr(), cv::COLOR_BGR2HLS);

        if ( cfg.lowRange > 0 || cfg.highRange < 255 ){
            if ( cfg.lowRange < 253 && cfg.highRange > 0 ){
                cv::Mat lut;
                Levels::createBlackAndWhiteLut(256, cfg.lowRange, cfg.highRange, lut);
                Levels::applyLut(lut, channel, *m->internalPtr());
            }
        }
        if ( cfg.midtonePoint != 1.0 ){
            cv::Mat lut;
            Levels::createGammaLut(256, cfg.midtonePoint, lut);
            Levels::applyLut(lut, channel, *m->internalPtr());
        }

        if ( m->internalPtr()->channels() == 3 )
            cv::cvtColor(*m->internalPtr(), *m->internalPtr(), cv::COLOR_HLS2BGR);
    }

    return m;
}

QJSValue QAdjustments::autoLevels(QMat *histogram){
    QJSValue result = lv::ViewContext::instance().engine()->engine()->newObject();
    const cv::Mat& hist = *histogram->internalPtr();

    if ( hist.cols < 3 )
        return QJSValue();

    for ( int y = 0; y < hist.rows; ++y ){
        const ushort* p = hist.ptr<ushort>(y);

        int x = 0;
        while ( x < hist.cols - 2 ){
            if ( p[x] == 0 )
                ++x;
            else
                break;
        }
        int black = x;

        x = hist.cols - 1;
        while ( x > 0 ){
            if ( p[x] == 0 )
                --x;
            else
                break;
        }
        int white = x;

        if ( white < black + 2 )
            white = black + 2;

        QJSValue channelAutoLevels = lv::ViewContext::instance().engine()->engine()->newArray(3);
        channelAutoLevels.setProperty(0, black);
        channelAutoLevels.setProperty(1, 1.0);
        channelAutoLevels.setProperty(2, white);

        result.setProperty(y, channelAutoLevels);
    }

    return result;
}

QMat *QAdjustments::brightnessAndContrast(QMat *source, double brightness, double contrast){
    if (!source){
        return nullptr;
    }

    const cv::Mat& in = source->internal();

    QMat* m = new QMat(source->internalPtr()->cols, source->internalPtr()->rows, static_cast<QMat::Type>(source->depth()), source->channels(), nullptr);

    for( int y = 0; y < in.rows; y++ ) {
        const uchar* pi = in.ptr<uchar>(y);
        uchar* po       = m->internalPtr()->ptr<uchar>(y);

        for( int x = 0; x < in.cols; x++ ) {
            for( int c = 0; c < in.channels(); c++ ){
                int pos = x * in.channels() + c;
                po[pos] = cv::saturate_cast<uchar>(contrast * pi[pos] + brightness);
            }
        }
    }

    return m;
}

QMat *QAdjustments::hueSaturationLightness(QMat *source, int hue, int saturation, int lightness){
    if (!source){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Adjustments: Null source provided.", 0);
        lv::ViewContext::instance().engine()->throwError(&e);
        return nullptr;
    }

    cv::Mat& in = *source->internalPtr();

    if ( (in.channels() != 3 && in.channels() != 4) || in.depth() != CV_8U )
        return nullptr;

    QMat* m = new QMat(in.cols, in.rows, static_cast<QMat::Type>(source->depth()), source->channels(), nullptr);

    cv::Mat temp;

    cvtColor(in, temp, cv::COLOR_BGR2HSV);
    signed short hue_shift = (hue - 180) / 2;

    for (int y = 0; y < in.rows; y++){
        uchar* p = temp.ptr<uchar>(y);

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
            double s_shift = (saturation - 100) / 100.0;
            double s_plus_shift = s + 255.0 * s_shift;

            if (s_plus_shift < 0)
                s_plus_shift = 0;
            else if (s_plus_shift > 255)
                s_plus_shift = 255;

            p[x * 3 + 1] = static_cast<unsigned char>(s_plus_shift);

            // lightness
            double lv = p[x * 3 + 2];
            double v_plus_shift = lv + 255.0 * ((lightness - 100) / 100.0);

            if (v_plus_shift < 0)
                v_plus_shift = 0;
            else if (v_plus_shift > 255)
                v_plus_shift = 255;

            p[x * 3 + 2] = static_cast<unsigned char>(v_plus_shift);
        }
    }

    cvtColor(temp, *m->internalPtr(), cv::COLOR_HSV2BGR);

    return m;
}
