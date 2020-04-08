/****************************************************************************
**
** Copyright (C) 2015-2016 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of QML Gantt library.
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

#include "qabstractrangemodel.h"

// QAbstractRangeModelPrivate
// ----------------------------------------------------------------------------

class QAbstractRangeModelPrivate{
public:
    class QRangeModelChange{
    public:
        qint64 startPosition;
        qint64 endPosition;
    };

public:
    QAbstractRangeModelPrivate() : lastChange(nullptr){}

    QRangeModelChange* change(qint64 startPosition, qint64 endPosition);
    QRangeModelChange* lastChange;
};


QAbstractRangeModelPrivate::QRangeModelChange* QAbstractRangeModelPrivate::change(
        qint64 startPosition,
        qint64 endPosition)
{
    QRangeModelChange* change = new QRangeModelChange;
    change->startPosition = startPosition;
    change->endPosition   = endPosition;
    return change;
}


// QAbstractRangeModel
// ----------------------------------------------------------------------------

QAbstractRangeModel::QAbstractRangeModel(QObject* parent)
    : QObject(parent)
    , d_ptr(new QAbstractRangeModelPrivate)
{
}

QAbstractRangeModel::~QAbstractRangeModel(){
    delete d_ptr;
}

void QAbstractRangeModel::beginDataChange(qint64 startPosition, qint64 endPosition){
    Q_D(QAbstractRangeModel);
    if ( d->lastChange != nullptr )
        delete d->lastChange;
    d->lastChange = d->change(startPosition, endPosition);
    emit itemsAboutToBeChanged(startPosition, endPosition);
}

void QAbstractRangeModel::endDataChange(){
    Q_D(QAbstractRangeModel);
    if ( d->lastChange != nullptr ){
        emit itemsChanged(d->lastChange->startPosition, d->lastChange->endPosition);
        delete d->lastChange;
        d->lastChange = nullptr;
    }
}

void QAbstractRangeModel::insertItem(qint64, qint64){}

void QAbstractRangeModel::removeItem(qint64, qint64, qint64){}
