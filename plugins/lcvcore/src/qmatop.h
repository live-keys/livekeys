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

#ifndef QMATOP_H
#define QMATOP_H

#include <QObject>
#include <QMatrix4x4>
#include "qmat.h"
#include "qumat.h"
#include "qwritablemat.h"
#include "live/qmlobjectlist.h"
#include "live/viewengine.h"

/// \private
class QMatOp : public QObject{

    Q_OBJECT
    Q_PROPERTY(QMat* nullMat READ nullMat CONSTANT)
    Q_ENUMS(BorderType)
public:
    enum BorderType {
        BORDER_CONSTANT    = cv::BORDER_CONSTANT,
        BORDER_REPLICATE   = cv::BORDER_REPLICATE,
        BORDER_REFLECT     = cv::BORDER_REFLECT,
        BORDER_WRAP        = cv::BORDER_WRAP,
        BORDER_REFLECT_101 = cv::BORDER_REFLECT_101,
        BORDER_TRANSPARENT = cv::BORDER_TRANSPARENT,

        BORDER_REFLECT101  = BORDER_REFLECT_101,
        BORDER_DEFAULT     = BORDER_REFLECT_101,
        BORDER_ISOLATED    = cv::BORDER_ISOLATED
    };

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

    QUMat* toUMat(QMat* m);

    void fill(QMat* m, const QColor& color);
    void fillWithMask(QMat* m, const QColor& color, QMat* mask);

    QMat* roi(QMat* m, int x, int y, int w, int h);
    QMat* overlap(QMat* in, QMat* in2, QMat *mask);
    QMat* alphaMerge(QMat* in, QMat* mask);

    QMat* reloc(QMat* m);
    QMat* crop(QMat* m, const QRect& region);
    QMat* flip(QMat* m, int direction);

    QJSValue split(QMat* m);
    QMat* merge(const QJSValue& matArray);

    QMat* spreadByLinearInterpolation(QJSValue reference, QJSValue spread);
    QMat* lut(QMat* m, QMat* lut);

    QMatrix4x4 to4x4Matrix(QMat* m);
    QVariantList toArray(QMat* m);

    QMat* absdiff(QMat* in1, QMat *in2);

    QMat* bitwiseXor(QMat* arg1, QMat* arg2);
    QMat* bitwiseOr(QMat* arg1, QMat* arg2);
    QMat* bitwiseAnd(QMat* arg1, QMat* arg2);
    QMat* bitwiseNot(QMat* arg);

    QMat* selectChannel(QMat* input, int channel);
    QMat* copyMakeBorder(QMat* input,
                         int top, int bottom, int left, int right,
                         int borderType = BORDER_DEFAULT, const QColor& color = QColor());

    QMat* convertTo8U(QMat* input);
private:
    lv::ViewEngine* engine();
};

inline QMat *QMatOp::nullMat() const{
    return QMat::nullMat();
}

#endif // QMATOP_H
