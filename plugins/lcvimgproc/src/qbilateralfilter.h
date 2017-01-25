/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

/****************************************************************************
**
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

#ifndef QBILATERALFILTER_HPP
#define QBILATERALFILTER_HPP

#include <QQuickItem>
#include "qmatfilter.h"

class QBilateralFilterPrivate;

class QBilateralFilter : public QMatFilter{

    Q_OBJECT
    Q_PROPERTY(int    d          READ d          WRITE setD          NOTIFY dChanged)
    Q_PROPERTY(double sigmaColor READ sigmaColor WRITE setSigmaColor NOTIFY sigmaColorChanged)
    Q_PROPERTY(double sigmaSpace READ sigmaSpace WRITE setSigmaSpace NOTIFY sigmaSpaceChanged)
    Q_PROPERTY(int    borderType READ borderType WRITE setBorderType NOTIFY borderTypeChanged)

public:
    explicit QBilateralFilter(QQuickItem *parent = 0);
    ~QBilateralFilter();

    int d() const;
    void setD(int d);

    double sigmaColor() const;
    void setSigmaColor(double sigmaColor);

    double sigmaSpace() const;
    void setSigmaSpace(double sigmaSpace);

    int borderType() const;
    void setBorderType(int borderType);

    virtual void transform(cv::Mat& in, cv::Mat& out);

signals:
    void dChanged();
    void sigmaColorChanged();
    void sigmaSpaceChanged();
    void borderTypeChanged();

private:
    QBilateralFilter(const QBilateralFilter& other);
    QBilateralFilter& operator= (const QBilateralFilter& other);

    QBilateralFilterPrivate* const d_ptr;

    Q_DECLARE_PRIVATE(QBilateralFilter)

};

#endif // QBILATERALFILTER_HPP
