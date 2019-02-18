#ifndef QMATOP_H
#define QMATOP_H

#include <QObject>
#include <QMatrix4x4>
#include "qmat.h"
#include "qwritablemat.h"

/// \private
class QMatOp : public QObject{

    Q_OBJECT
    Q_PROPERTY(QMat* nullMat READ nullMat CONSTANT)

public:
    explicit QMatOp(QObject *parent = 0);

    static cv::Scalar toScalar(const QColor& color);
    static cv::Rect toRect(const QRect& r);

    QMat* nullMat() const;

public slots:
    QMat* create(const QSize& size, int type = QMat::CV8U, int channels = 1);
    QMat* createFill(const QSize& size, int type, int channels, const QColor& color);
    QMat* createFromArray(const QVariantList& a, int type = QMat::CV8U);

    QWritableMat* createWritable(const QSize& size, int type = QMat::CV8U, int channels = 1);
    QWritableMat* createWritableFill(const QSize& size, int type, int channels, const QColor& color);
    QWritableMat* createWritableFromMat(QMat* m);

    void fill(QMat* m, const QColor& color);
    void fillWithMask(QMat* m, const QColor& color, QMat* mask);

    QMat* reloc(QMat* m);

    QMat* crop(QMat* m, const QRect& region);

    QMatrix4x4 to4x4Matrix(QMat* m);
    QVariantList toArray(QMat* m);
};

inline QMat *QMatOp::nullMat() const{
    return QMat::nullMat();
}

#endif // QMATOP_H
