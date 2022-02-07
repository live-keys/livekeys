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

#ifndef QPROJECTIONSURFACE_H
#define QPROJECTIONSURFACE_H

#include <QObject>
#include "qvariant.h"
#include <QRectF>
#include "opencv2/imgproc.hpp"
#include <QJSValue>

class QMat;
class QProjectionMapper;

class QProjectionSurface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMat*                input       READ input          WRITE   setInput        NOTIFY  inputChanged)
    Q_PROPERTY(QProjectionMapper*   mapper      READ mapper         CONSTANT)
    Q_PROPERTY(QJSValue             destination READ destination    WRITE   setDestination  NOTIFY destinationChanged)

public:
    explicit QProjectionSurface(QObject *parent = nullptr);
    ~QProjectionSurface();

    QMat *input() const;
    void setInput(QMat *input);

    QProjectionMapper *mapper() const;

    QJSValue destination() const;
    void setDestination(const QJSValue &destination);

signals:
    void inputChanged();
    void sourceChanged();
    void destinationChanged();
private:

    QJSValue m_destination;
    QMat* m_input;
};

#endif // QPROJECTIONSURFACE_H
