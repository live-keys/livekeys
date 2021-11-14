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

#ifndef QUMAT_H
#define QUMAT_H

#include <QQuickItem>

#include "qlcvcoreglobal.h"
#include "live/shared.h"

#include "opencv2/core.hpp"

namespace lv{ class Memory; }

class QMat;

class Q_LCVCORE_EXPORT QUMat : public lv::Shared{

    Q_OBJECT
    Q_ENUMS(Type)

public:
    /**
    *\brief Type
    */
    typedef cv::UMat InternalType;
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
    explicit QUMat(QObject *parent = nullptr);
    QUMat(cv::UMat *mat, QObject *parent = nullptr);
    QUMat(int width, int height, QUMat::Type type, int channels, QObject* parent = nullptr);
    virtual ~QUMat();

    QUMat* clone() const;
    static QUMat* reloc(QUMat* m);

    cv::UMat* internalPtr();

    const cv::UMat& internal() const;
    cv::UMat& internal();

    virtual void recycleSize(int size);
    virtual Shared* transfer(){ return clone(); }

public slots:
    int    channels() const;
    int    depth() const;
    QSize  dimensions() const;
    QUMat* createOwnedObject();
    QUMat* cloneMat() const;
    QMat*  getMat() const;

private:
    static cv::UMat* memoryAlloc(int width, int height, int type, int channels);
    static size_t memorySize(int width, int height, int type, int channels);
    static size_t memorySize(const QUMat* m);
    static bool memoryValidate(cv::UMat* m, int width, int height, int type, int channels);
    static void memoryFree(cv::UMat* m);

    cv::UMat*  m_internal;
};

inline cv::UMat *QUMat::internalPtr(){
    return m_internal;
}

inline bool QUMat::memoryValidate(cv::UMat *, int, int, int, int){
    return true;
}

inline void QUMat::memoryFree(cv::UMat *m){
    delete m;
}


#endif // QUMAT_H
