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

#ifndef QCVGLOBALOBJECT_H
#define QCVGLOBALOBJECT_H

#include <QObject>
#include <QMatrix4x4>
#include "qmat.h"

class QCvGlobalObject : public QObject{

    Q_OBJECT
    Q_PROPERTY(QMat* nullMat READ nullMat CONSTANT)

public:
    explicit QCvGlobalObject(QObject *parent = nullptr);

    QMat* nullMat() const;

public slots:
    QVariantList matToArray(QMat* m);
    void assignArrayToMat(const QVariantList &a, QMat *m);
    QMatrix4x4 matrix4x4(QMat* m);
};

inline QMat *QCvGlobalObject::nullMat() const{
    return QMat::nullMat();
}

#endif // QCVGLOBALOBJECT_H
