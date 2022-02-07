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

#ifndef QADJUSTMENTS_H
#define QADJUSTMENTS_H

#include <QObject>
#include "qmat.h"

namespace lv{
class ViewEngine;
}

class QAdjustments : public QObject{

    Q_OBJECT

public:
    explicit QAdjustments(QObject *parent = nullptr);
    ~QAdjustments();

public slots:
    QMat* levels(QMat* source, const QJSValue& lightness, const QJSValue& channels);
    QJSValue autoLevels(QMat* histogram);
    QMat* brightnessAndContrast(QMat* source, double brightness, double contrast);
    QMat* hueSaturationLightness(QMat* source, int hue, int saturation, int lightness);
    
private:
    lv::ViewEngine* m_engine;

};

#endif // QADJUSTMENTS_H
