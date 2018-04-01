/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "qlevels.h"
#include "live/plugincontext.h"
#include "live/engine.h"

#include "opencv2/imgproc.hpp"

#include <QJSValueIterator>

namespace{

void createBlackAndWhiteLut(int histogramSize, int lowCut, int highCut, cv::Mat& lut){
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

void createGammaLut(int lutSize, float fGamma, cv::Mat& lut){
    lut.create(1, lutSize, CV_8U);
    lut.setTo(0);
    uchar* pl = lut.ptr();

    for (int i = 0; i < 256; i++){
        pl[i] = cv::saturate_cast<uchar>(pow((float)(i / 255.0), 1/ fGamma) * 255.0f);
    }
}

void applyLut(const cv::Mat& lut, int channel, cv::Mat& dst){
    const uchar* pl = lut.ptr();
    for ( int y = 0; y < dst.rows; ++y ){
        uchar* p = dst.ptr<uchar>(y);
        for ( int x = 0; x < dst.cols; ++x ){
            p[x * dst.channels() + channel] = pl[p[x * dst.channels() + channel]];
        }
    }
}

}

QLevels::QLevels(QQuickItem *parent)
    : QMatFilter(parent)
{
}

QLevels::~QLevels(){
}

void QLevels::transform(const cv::Mat &in, cv::Mat &out){
    in.copyTo(m_temp);

    // iterate channels first
    for ( auto it = m_channelConfiguration.begin(); it != m_channelConfiguration.end(); ++it ){
        const Configuration& cfg = it.value();
        if ( it.key() < in.channels() ){
            if ( cfg.lowRange > 0 || cfg.highRange < 255 ){

                if ( cfg.lowRange < 253 && cfg.highRange > 0 ){
                    cv::Mat lut;
                    createBlackAndWhiteLut(256, cfg.lowRange, cfg.highRange, lut);
                    applyLut(lut, it.key(), m_temp);
                }
            }
            if ( cfg.midtonePoint != 1.0 ){
                cv::Mat lut;
                createGammaLut(256, cfg.midtonePoint, lut);
                applyLut(lut, it.key(), m_temp);
            }
        }
    }

    // adjust luminance channel
    const Configuration& cfg = m_lightnessConfiguration;
    if ( cfg.lowRange > 0 || cfg.highRange < 255 || cfg.midtonePoint != 1.0 ){
        int channel = 1;

        if ( m_temp.channels() == 3 )
            cv::cvtColor(m_temp, m_temp, CV_BGR2HLS);

        if ( cfg.lowRange > 0 || cfg.highRange < 255 ){
            if ( cfg.lowRange < 253 && cfg.highRange > 0 ){
                cv::Mat lut;
                createBlackAndWhiteLut(256, cfg.lowRange, cfg.highRange, lut);
                applyLut(lut, channel, m_temp);
            }
        }
        if ( cfg.midtonePoint != 1.0 ){
            cv::Mat lut;
            createGammaLut(256, cfg.midtonePoint, lut);
            applyLut(lut, channel, m_temp);
        }

        if ( m_temp.channels() == 3 )
            cv::cvtColor(m_temp, m_temp, CV_HLS2BGR);
    }

    cv::swap(m_temp, out);
}

void QLevels::setLightness(const QJSValue &value){
    if ( value.isArray() ){
        if ( value.property("length").toInt() == 3 ){
            Configuration cfg;
            cfg.lowRange = value.property(0).toInt();
            cfg.midtonePoint = value.property(1).toNumber();
            cfg.highRange = value.property(2).toInt();
            m_lightnessConfiguration = cfg;
            m_lightness = value;
        } else {
            lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Global channel must be array of size 3.", 0);
            lv::PluginContext::engine()->throwError(&e);
            return;
        }
    } else if (!value.isUndefined() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Global channel must be array of size 3.", 0);
        lv::PluginContext::engine()->throwError(&e, this);
        return;
    } else {
        m_lightnessConfiguration = Configuration();
    }

    emit lightnessChanged();
    QMatFilter::transform();
}

void QLevels::setChannels(const QJSValue &channels){
    if ( channels.isObject() ){
        m_channelConfiguration.clear();
        QJSValueIterator it(channels);
        while ( it.hasNext() ){
            it.next();
            bool isKeyInt;
            int channelNo = it.name().toInt(&isKeyInt);
            if ( !isKeyInt ){
                lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Channels must be of object of <int, Array> type.", 0);
                lv::PluginContext::engine()->throwError(&e);
                return;
            }
            QJSValue channel = it.value();
            if ( !channel.isArray() || channel.property("length").toInt() != 3 ){
                lv::Exception e = CREATE_EXCEPTION(
                    lv::Exception, "Channels must be of object of <int, Array> type, where Array is of size[3]", 0
                );
                lv::PluginContext::engine()->throwError(&e);
                return;
            }

            Configuration cfg;
            cfg.lowRange = channel.property(0).toInt();
            cfg.midtonePoint = channel.property(1).toNumber();
            cfg.highRange = channel.property(2).toInt();
            m_channelConfiguration[channelNo] = cfg;
        }
    } else if (!channels.isUndefined() ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Channels must be of object of <int, Array> type.", 0);
        lv::PluginContext::engine()->throwError(&e);
        return;
    } else {
        m_channelConfiguration.clear();
    }

    m_channel = channels;
    emit channelsChanged();
    QMatFilter::transform();
}
