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

#ifndef QIMWRITE_H
#define QIMWRITE_H

#include <QObject>
#include <QJSValue>
#include <vector>
#include "qmat.h"
/// \private
class QImWrite : public QObject{

    Q_OBJECT
    Q_PROPERTY(QJSValue params READ params WRITE setParams NOTIFY paramsChanged)

public:
    QImWrite(QObject* parent = 0);
    ~QImWrite();

    const QJSValue& params() const;
    void setParams(const QJSValue& params);

public slots:
    bool saveImage(const QString& file, QMat* image);

signals:
    void paramsChanged();

private:
    QJSValue m_params;
    std::vector<int> m_convertedParams;
};

inline const QJSValue &QImWrite::params() const{
    return m_params;
}



#endif // QIMWRITE_H
