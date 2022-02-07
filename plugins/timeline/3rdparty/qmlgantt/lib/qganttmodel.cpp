/****************************************************************************
**
** Copyright (C) 2015-2016 Dinu SV.
**
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

#include "qganttmodel.h"
#include "qganttmodelitem.h"
#include <QVariant>

// QGanttModelPrivate
// ----------------------------------------------------------------------------

class QGanttModelPrivate{
public:
    QGanttModelPrivate() : lastChange(0), itemDataFactory(0){}
    ~QGanttModelPrivate();

    class QGanttModelChange{
    public:
        qint64 startPosition;
        qint64 endPosition;
    };

    QList<QGanttModelItem*> items;
    QGanttModelChange* lastChange;
    QGanttModelChange* change(qint64 startPosition, qint64 endPosition);

    QGanttModel::ItemDataFactoryFunction itemDataFactory;

    int searchFirstIndex(qint64 position);
    int searchFirstIndex(qint64 position, qint64 length);
};

QGanttModelPrivate::~QGanttModelPrivate(){
    for (QList<QGanttModelItem*>::iterator it = items.begin(); it != items.end(); ++it ){
        delete *it;
    }
    items.clear();
}

QGanttModelPrivate::QGanttModelChange* QGanttModelPrivate::change(qint64 startPosition, qint64 endPosition){
    QGanttModelChange* change = new QGanttModelChange;
    change->startPosition = startPosition;
    change->endPosition   = endPosition;
    return change;
}

int QGanttModelPrivate::searchFirstIndex(qint64 position){
    if ( items.size() == 0 )
        return 0;
    if ( items.last()->position() < position )
        items.size();

    int first  = 0, last = items.size() - 1, middle = (first + last) / 2;

    while( first <= last ){
        if ( items[middle]->position() < position ){
            first = middle + 1;
        } else {
            last = middle - 1;
        }

        middle = (first + last) / 2;
    }

    if ( items[middle]->position() < position )
        return middle + 1;
    else
        return middle;
}

int QGanttModelPrivate::searchFirstIndex(qint64 position, qint64 length){
    int index = searchFirstIndex(position);
    while ( index < items.size() ){
        if ( items[index]->position() != position )
            return items.size();
        if ( items[index]->length() == length )
            return index;
        ++index;
    }
    return items.size();
}

// QGanttModel
// ----------------------------------------------------------------------------

QGanttModel::QGanttModel(QObject *parent)
    : QAbstractRangeModel(parent)
    , d_ptr(new QGanttModelPrivate)
{
}

QGanttModel::~QGanttModel(){
    delete d_ptr;
}

QAbstractRangeModelIterator* QGanttModel::dataBetween(qint64 startPosition, qint64 endPosition){
    Q_D(QGanttModel);

    int startIndex = d->searchFirstIndex(startPosition);
    while ( startIndex > 0 ){
        --startIndex;
        if ( d->items[startIndex]->position() + d->items[startIndex]->length() < startPosition ){
            ++startIndex;
            break;
        }
    }
    if ( startIndex < d->items.size() ){
        return new QGanttModelIterator(d->items.begin() + startIndex, d->items.end(), endPosition);
    }
    return new QGanttModelIterator(d->items.end(), d->items.end(), endPosition);
}

void QGanttModel::setItemPosition(qint64 itemPosition, qint64 itemLength, int indexOffset, qint64 itemNewPosition){
    Q_D(QGanttModel);

    int index = d->searchFirstIndex(itemPosition, itemLength) + indexOffset;
    if ( index >= d->items.size() )
        return;

    QGanttModelItem* item = d->items[index];
    if ( item->position() != itemPosition )
        return;

    d->items.takeAt(index);
    item->setPosition(itemNewPosition);
    int newIndex = addSegment(item);

    QList<QGanttModelItem*>::iterator itemIt = d->items.begin() + newIndex;
    QGanttModelIterator* modelIt = new QGanttModelIterator(itemIt, itemIt + 1, itemNewPosition + 1);
    emit itemCoordinatesChanged(itemPosition, itemLength, indexOffset, modelIt, 0);
}

void QGanttModel::setItemLength(qint64 itemPosition, qint64 itemLength, int indexOffset, qint64 newLength){
    Q_D(QGanttModel);
    if ( itemLength == newLength )
        return;

    int positionIndex = d->searchFirstIndex(itemPosition);
    int index = positionIndex;
    while ( index < d->items.size() ){
        if ( d->items[index]->length() == itemLength )
            break;
        ++index;
    }

    index += indexOffset;
    if ( index >= d->items.size() )
        return;

    QGanttModelItem* item = d->items[index];
    if ( item->position() != itemPosition )
        return;

    while ( positionIndex < d->items.size() ){
        if ( d->items[positionIndex]->position() == item->position() && d->items[positionIndex]->length() < newLength )
            ++positionIndex;
        else
            break;
    }

    if ( index == positionIndex || index == positionIndex + 1 ){
        item->setLength(newLength);

        QList<QGanttModelItem*>::iterator itemIt = d->items.begin() + index;
        QGanttModelIterator* modelIt = new QGanttModelIterator(itemIt, itemIt + 1, itemPosition + 1);
        emit itemCoordinatesChanged(itemPosition, itemLength, indexOffset, modelIt, indexOffset);
    } else {
        d->items.takeAt(index);
        item->setLength(newLength);
        int newIndex = addSegment(item);

        QList<QGanttModelItem*>::iterator itemIt = d->items.begin() + newIndex;
        QGanttModelIterator* modelIt = new QGanttModelIterator(itemIt, itemIt + 1, itemPosition + 1);
        emit itemCoordinatesChanged(itemPosition, itemLength, indexOffset, modelIt, 0);
    }

}

void QGanttModel::setItemData(
        qint64 itemPosition,
        qint64 itemLength,
        int positionIndex,
        int role,
        const QVariant& value)
{
    Q_D(QGanttModel);
    int index = d->searchFirstIndex(itemPosition, itemLength) + positionIndex;
    if ( index >= d->items.size() )
        return;

    QList<QGanttModelItem*>::iterator itemIt = d->items.begin() + index;
    QGanttModelItem* item = *itemIt;
    QGanttModelIterator* modelIt = new QGanttModelIterator(itemIt, itemIt + 1, itemPosition + 1);

    if ( role == QGanttModel::modelData ){
        item->setData(value.toString());
        emit itemsDataChanged(modelIt, positionIndex, QList<int>() << QGanttModel::modelData);
    }
}

QHash<int, QByteArray> QGanttModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[QGanttModel::modelData] = "modelData";
    return roles;
}

int QGanttModel::addSegment(QGanttModelItem* item){
    Q_D(QGanttModel);
    int index = d->searchFirstIndex(item->position());
    while ( index < d->items.size() ){
        if ( d->items[index]->position() == item->position() && d->items[index]->length() < item->length() )
            ++index;
        else {
            break;
        }
    }
    d->items.insert(index, item);
    return index;
}

void QGanttModel::setItemDataFactoryFunction(QGanttModel::ItemDataFactoryFunction fp){
    Q_D(QGanttModel);
    d->itemDataFactory = fp;
}

void QGanttModel::addSegment(qint64 position, qint64 length){
    Q_D(QGanttModel);
    QGanttModelItem* item = new QGanttModelItem(position, length);
    if ( d->itemDataFactory )
        item->setData(d->itemDataFactory());

    beginDataChange(position, position + 1);
    addSegment(item);
    endDataChange();
}

void QGanttModel::removeItem(qint64 position, qint64 length, qint64 relativeIndex){
    Q_D(QGanttModel);
    int index = d->searchFirstIndex(position);
    while ( index < d->items.size() ){
        if ( d->items[index]->position() != position )
            return;

        if ( d->items[index]->length() == length && index + relativeIndex < d->items.size() ){
            index += relativeIndex;
            if ( d->items[index]->position() == position && d->items[index]->length() == length ){
                beginDataChange(position, position + 1);
                delete d->items[index];
                d->items.removeAt(index);
                endDataChange();
                return;
            }
        }
        ++index;
    }
}

bool QGanttModelIterator::isEnd() const{
    if ( m_currentIt == m_endIt )
        return true;
    if ( (*m_currentIt)->position() >= m_endPosition )
        return true;
    return false;
}

void QGanttModelIterator::nextItem(){
    if ( isEnd() )
        return;
    ++m_currentIt;
}

QVariant QGanttModelIterator::data(int role){
    if ( role == QGanttModel::PositionRole )
        return (*m_currentIt)->position();
    else if ( role == QGanttModel::LengthRole )
        return (*m_currentIt)->length();
    else if ( role == QGanttModel::modelData )
        return (*m_currentIt)->data();
    return QVariant();
}
