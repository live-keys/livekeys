#ifndef QWRITABLEMAT_H
#define QWRITABLEMAT_H

#include "qlcvcoreglobal.h"
#include "opencv2/core.hpp"
#include <QObject>
#include <QSize>

class QMat;

/// \private
class Q_LCVCORE_EXPORT QWritableMat : public QObject{

    Q_OBJECT

public:
    explicit QWritableMat(QObject *parent = nullptr);
    QWritableMat(cv::Mat* mat, QObject* parent = nullptr);
    virtual ~QWritableMat();

    const cv::Mat& internal() const;
    cv::Mat& internal();

public slots:
    QByteArray buffer();
    int channels() const;
    int depth() const;
    QSize dimensions() const;
    QMat* toMat() const;

    void fill(const QColor& color, QMat* mask);

private:
    cv::Mat* m_internal;
};

#endif // QWRITABLEMAT_H
