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

#ifndef QTONEMAPREINHARD_H
#define QTONEMAPREINHARD_H

#include <QQuickItem>
#include "qtonemap.h"

class QTonemapReinhard : public QTonemap{

    Q_OBJECT
    Q_PROPERTY(float intensity  READ intensity  WRITE setIntensity  NOTIFY intensityChanged)
    Q_PROPERTY(float lightAdapt READ lightAdapt WRITE setLightAdapt NOTIFY lightAdaptChanged)
    Q_PROPERTY(float colorAdapt READ colorAdapt WRITE setColorAdapt NOTIFY colorAdaptChanged)

public:
    QTonemapReinhard(QQuickItem* parent = nullptr);
    virtual ~QTonemapReinhard(){}

    void initialize(const QVariantMap &options);
    float intensity() const;
    void setIntensity(float intensity);

    float lightAdapt() const;
    void setLightAdapt(float lightAdapt);

    float colorAdapt() const;
    void setColorAdapt(float colorAdapt);

signals:
    void intensityChanged();
    void lightAdaptChanged();
    void colorAdaptChanged();
private:
    void createTonemapReinhard();

    float m_intensity;
    float m_lightAdapt;
    float m_colorAdapt;
};

#endif // QTONEMAPREINHARD_H
