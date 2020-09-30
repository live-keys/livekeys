#ifndef QMATOP_H
#define QMATOP_H

#include <QObject>
#include <QMatrix4x4>
#include "qmat.h"
#include "qwritablemat.h"
#include "live/qmlobjectlist.h"

/// \private
class QMatOp : public QObject{

    Q_OBJECT
    Q_PROPERTY(QMat* nullMat READ nullMat CONSTANT)

public:
    explicit QMatOp(QObject *parent = nullptr);

    static cv::Scalar toScalar(const QColor& color);
    static cv::Rect toRect(const QRect& r);

    QMat* nullMat() const;

    /// \private
    class ListOperations{
    public:
        static void appendItem(lv::QmlObjectList* list, QObject* item);
        static int itemCount(lv::QmlObjectList* list);
        static QObject* itemAt(lv::QmlObjectList* list, int idx);
        static void removeItemAt(lv::QmlObjectList* list, int idx);
        static void clearItems(lv::QmlObjectList* list);
   };
public slots:
    lv::QmlObjectList* createMatList(const QJSValue &matArray = QJSValue());

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
    QMat* flip(QMat* m, int direction);
    QMat* perspective(QMat* input, QJSValue points);

    QJSValue split(QMat* m);
    QMat* merge(const QJSValue& matArray);

    QMat* spreadByLinearInterpolation(QJSValue reference, QJSValue spread);
    QMat* lut(QMat* m, QMat* lut);

    QMatrix4x4 to4x4Matrix(QMat* m);
    QVariantList toArray(QMat* m);

    QMat* bitwiseXor(QMat* arg1, QMat* arg2);
    QMat* bitwiseOr(QMat* arg1, QMat* arg2);
    QMat* bitwiseAnd(QMat* arg1, QMat* arg2);
    QMat* bitwiseNot(QMat* arg);
};

inline QMat *QMatOp::nullMat() const{
    return QMat::nullMat();
}

#endif // QMATOP_H
