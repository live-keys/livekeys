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

#ifndef QMAT_H
#define QMAT_H

#include <QQuickItem>
#include "live/shared.h"
#include "opencv2/core.hpp"
#include "qlcvcoreglobal.h"

namespace lv{ class Memory; }

class Q_LCVCORE_EXPORT QMat : public lv::Shared{

    Q_OBJECT
    Q_ENUMS(Type)

public:
    /**
    *\brief Type
    */
    typedef cv::Mat InternalType;
    enum Type{
        CV8U  = CV_8U,
        CV8S  = CV_8S,
        CV16U = CV_16U,
        CV16S = CV_16S,
        CV32S = CV_32S,
        CV32F = CV_32F,
        CV64F = CV_64F
    };

    friend class lv::Memory;

public:
    explicit QMat(QObject *parent = nullptr);
    QMat(cv::Mat *mat, QObject *parent = nullptr);
    QMat(int width, int height, QMat::Type type, int channels, QObject* parent = nullptr);
    virtual ~QMat();

    static QMat* nullMat();

    QMat* clone() const;
    static QMat* reloc(QMat* m);

    cv::Mat* internalPtr();

    const cv::Mat& internal() const;
    cv::Mat& internal();

    virtual void recycleSize(int size);
    virtual Shared* transfer(){ return clone(); }

public slots:
    QByteArray  buffer() const;
    int         channels() const;
    int         depth() const;
    QSize       dimensions() const;
    QMat*       createOwnedObject();
    QMat*       cloneMat() const;

private:
    static cv::Mat* memoryAlloc(int width, int height, int type, int channels);
    static size_t memorySize(int width, int height, int type, int channels);
    static size_t memorySize(const QMat* m);
    static bool memoryValidate(cv::Mat* m, int width, int height, int type, int channels);
    static void memoryFree(cv::Mat* m);

    cv::Mat*  m_internal;

    static QMat* m_nullMat;
};

inline cv::Mat *QMat::internalPtr(){
    return m_internal;
}

inline bool QMat::memoryValidate(cv::Mat *, int, int, int, int){
    return true;
}

inline void QMat::memoryFree(cv::Mat *m){
    delete m;
}


#endif // QMAT_H
