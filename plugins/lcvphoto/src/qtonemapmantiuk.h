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

#ifndef QTONEMAPMANTIUK_H
#define QTONEMAPMANTIUK_H

#include <QQuickItem>
#include "qtonemap.h"

class QTonemapMantiuk : public QTonemap{

    Q_OBJECT
    Q_PROPERTY(float scale      READ scale      WRITE setScale      NOTIFY scaleChanged)
    Q_PROPERTY(float saturation READ saturation WRITE setSaturation NOTIFY saturationChanged)
public:
    QTonemapMantiuk(QQuickItem* parent = nullptr);
    virtual ~QTonemapMantiuk(){}

    float scale() const;
    void setScale(float scale);

    float saturation() const;
    void setSaturation(float saturation);
signals:
    void scaleChanged();
    void saturationChanged();
private:
    void createTonemapMantiuk();

    float m_scale;
    float m_saturation;
};

#endif // QTONEMAPMANTIUK_H
