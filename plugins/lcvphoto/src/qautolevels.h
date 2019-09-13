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

#ifndef QAUTOLEVELS_H
#define QAUTOLEVELS_H

#include <QObject>
#include "qmat.h"

class QAutoLevels : public QObject{

    Q_OBJECT
    Q_PROPERTY(QMat* histogram READ histogram WRITE setHistogram NOTIFY histogramChanged)
    Q_PROPERTY(QJSValue output READ output    NOTIFY outputChanged)

public:
    explicit QAutoLevels(QObject *parent = nullptr);

    QMat* histogram() const;
    QJSValue output() const;

signals:
    void histogramChanged();
    void outputChanged();

public slots:
    void setHistogram(QMat* histogram);

private:
    QMat*    m_histogram;
    QJSValue m_output;
};

inline QMat *QAutoLevels::histogram() const{
    return m_histogram;
}

inline QJSValue QAutoLevels::output() const{
    return m_output;
}

#endif // QAUTOLEVELS_H
