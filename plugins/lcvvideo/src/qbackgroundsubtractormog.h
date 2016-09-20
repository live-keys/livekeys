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

#ifndef QBACKGROUNDSUBTRACTORMOG_HPP
#define QBACKGROUNDSUBTRACTORMOG_HPP

#include <QQuickItem>
#include "qbackgroundsubtractor.h"

class QBackgroundSubtractorMogPrivate;

class QBackgroundSubtractorMog : public QBackgroundSubtractor{

    Q_OBJECT
    Q_PROPERTY(int    history         READ history         WRITE setHistory         NOTIFY historyChanged)
    Q_PROPERTY(int    nmixtures       READ nmixtures       WRITE setNmixtures       NOTIFY nmixturesChanged)
    Q_PROPERTY(double backgroundRatio READ backgroundRatio WRITE setBackgroundRatio NOTIFY backgroundRatioChanged)
    Q_PROPERTY(double noiseSigma      READ noiseSigma      WRITE setNoiseSigma      NOTIFY noiseSigmaChanged)

public:
    explicit QBackgroundSubtractorMog(QQuickItem *parent = 0);
    virtual ~QBackgroundSubtractorMog();

    int history() const;
    void setHistory(int history);

    int nmixtures() const;
    void setNmixtures(int nmixtures);

    double backgroundRatio() const;
    void setBackgroundRatio(double backgroundRatio);

    double noiseSigma() const;
    void setNoiseSigma(double noiseSigma);

signals:
    void historyChanged();
    void nmixturesChanged();
    void backgroundRatioChanged();
    void noiseSigmaChanged();

private:
    QBackgroundSubtractorMog(const QBackgroundSubtractorMog& other);
    QBackgroundSubtractorMog& operator= (const QBackgroundSubtractorMog& other);

    QBackgroundSubtractorMogPrivate* const d_ptr;

    Q_DECLARE_PRIVATE(QBackgroundSubtractorMog)

};

#endif // QBACKGROUNDSUBTRACTORMOG_HPP
