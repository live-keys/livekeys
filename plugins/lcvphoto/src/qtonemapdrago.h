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

#ifndef QTONEMAPDRAGO_H
#define QTONEMAPDRAGO_H

#include <QQuickItem>
#include "qtonemap.h"

class QTonemapDrago : public QTonemap{

    Q_OBJECT
    Q_PROPERTY(float saturation READ saturation WRITE setSaturation NOTIFY saturationChanged)
    Q_PROPERTY(float bias       READ bias       WRITE setBias       NOTIFY biasChanged)
public:
    QTonemapDrago(QQuickItem* parent = nullptr);
    virtual ~QTonemapDrago(){}

    float saturation() const;
    void setSaturation(float saturation);

    float bias() const;
    void setBias(float bias);
signals:
    void biasChanged();
    void saturationChanged();
private:
    void createTonemapDrago();

    float m_saturation;
    float m_bias;

};

#endif // QTONEMAPDRAGO_H
