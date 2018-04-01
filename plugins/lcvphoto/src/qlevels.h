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

#ifndef QLEVELS_H
#define QLEVELS_H

#include <QQuickItem>
#include <QJSValue>
#include "qmatfilter.h"

class QLevels : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(QJSValue lightness READ lightness WRITE setLightness NOTIFY lightnessChanged)
    Q_PROPERTY(QJSValue channels  READ channels  WRITE setChannels  NOTIFY channelsChanged)

    class Configuration{
    public:
        Configuration() : lowRange(0), highRange(255), midtonePoint(1.0){}

        int    lowRange;
        int    highRange;
        double midtonePoint;
    };

public:
    QLevels(QQuickItem* parent = 0);
    ~QLevels();

    virtual void transform(const cv::Mat &in, cv::Mat &out);

    const QJSValue& lightness() const;
    const QJSValue& channels() const;

    void setLightness(const QJSValue& value);
    void setChannels(const QJSValue& channels);

signals:
    void lightnessChanged();
    void channelsChanged();

private:
    QJSValue m_lightness;
    QJSValue m_channel;
    cv::Mat  m_temp;

    Configuration            m_lightnessConfiguration;
    QMap<int, Configuration> m_channelConfiguration;
};

inline const QJSValue &QLevels::lightness() const{
    return m_lightness;
}

inline const QJSValue &QLevels::channels() const{
    return m_channel;
}


#endif // QLEVELS_H
