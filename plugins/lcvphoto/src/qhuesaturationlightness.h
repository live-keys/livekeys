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

#ifndef QHUESATURATIONLIGHTNESS_H
#define QHUESATURATIONLIGHTNESS_H

#include <QObject>
#include "qmatfilter.h"

class QHueSaturationLightness : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(int hue        READ hue        WRITE setHue        NOTIFY hueChanged)
    Q_PROPERTY(int saturation READ saturation WRITE setSaturation NOTIFY saturationChanged)
    Q_PROPERTY(int lightness  READ lightness  WRITE setLightness  NOTIFY lightnessChanged)

public:
    QHueSaturationLightness(QQuickItem* item = 0);
    ~QHueSaturationLightness();

    int hue() const;
    int saturation() const;
    int lightness() const;

    void setHue(int hue);
    void setSaturation(int saturation);
    void setLightness(int lightness);

    virtual void transform(const cv::Mat &in, cv::Mat &out);

signals:
    void hueChanged();
    void saturationChanged();
    void lightnessChanged();

private:
    cv::Mat m_middle;
    int m_hue;
    int m_saturation;
    int m_lightness;
};

inline int QHueSaturationLightness::hue() const{
    return m_hue;
}

inline int QHueSaturationLightness::saturation() const{
    return m_saturation;
}

inline int QHueSaturationLightness::lightness() const{
    return m_lightness;
}

inline void QHueSaturationLightness::setHue(int hue){
    if (m_hue == hue)
        return;

    m_hue = hue;
    emit hueChanged();
    QMatFilter::transform();
}

inline void QHueSaturationLightness::setSaturation(int saturation){
    if ( saturation < 0 )
        saturation = 0;
    if ( saturation > 200)
        saturation = 200;

    if (m_saturation == saturation)
        return;

    m_saturation = saturation;
    emit saturationChanged();
    QMatFilter::transform();
}

inline void QHueSaturationLightness::setLightness(int lightness){
    if ( lightness < 0 )
        lightness = 0;
    if ( lightness > 200)
        lightness = 200;

    if (m_lightness == lightness)
        return;

    m_lightness = lightness;
    emit lightnessChanged();
    QMatFilter::transform();
}

#endif // QHUESATURATIONLIGHTNESS_H
