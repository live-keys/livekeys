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

#include "qrangeview.h"
#include "qabstractrangemodel.h"
#include <QQmlContext>

#include <QDebug>

// QRangeViewItem
// ----------------------------------------------------------------------------

class QRangeViewItem{
public:
    QQmlContext* context;
    QQuickItem*  delegate;
    qint64 position;
    qint64 length;

    ~QRangeViewItem();
};

QRangeViewItem::~QRangeViewItem(){
    delegate->setParentItem(0);
    delegate->deleteLater();
    context->deleteLater();
}

// QRangeViewPrivate
// ----------------------------------------------------------------------------

class QRangeViewPrivate{

public:
    QRangeViewPrivate();
    ~QRangeViewPrivate();

    // Fields
    // ------

    QList<QRangeViewItem*> items;

    int lastViewportWidth;
    int lastViewportX;

    QAbstractRangeModel* model;
    QQmlComponent* delegate;
    QHash<int, QByteArray> modelRoles;

    // Methods
    // -------

    int findItemFrom(qint64 position);
    int findItem(qint64 position, qint64 length);
    int findItemByDelegate(QQuickItem* delegate);

    int findRelativeIndex(int index);

    void clearItemsInInterval(qint64 positionBegin, qint64 positionEnd);
    void clearItems();

    QString listItems();
};

QRangeViewPrivate::QRangeViewPrivate()
    : lastViewportWidth(0)
    , lastViewportX(0)
    , model(0)
    , delegate(0)
{
}

QRangeViewPrivate::~QRangeViewPrivate(){
    clearItems();
}

int QRangeViewPrivate::findItemFrom(qint64 position){
    if ( items.size() == 0 )
        return 0;
    if ( items.last()->position < position )
        return items.size();

    int first  = 0, last = items.size() - 1, middle = (first + last) / 2;

    while( first <= last ){
        if ( items[middle]->position < position ){
            first = middle + 1;
        } else {
            last = middle - 1;
        }

        middle = (first + last) / 2;
    }

    if ( items[middle]->position < position )
        return middle + 1;
    else
        return middle;
}

int QRangeViewPrivate::findItem(qint64 position, qint64 length){
    int index = findItemFrom(position);
    while ( index < items.size() ){
        if ( items[index]->position != position )
            return items.size();
        if ( items[index]->length == length )
            return index;
        ++index;
    }
    return items.size();
}

int QRangeViewPrivate::findItemByDelegate(QQuickItem* delegate){
    int index = findItemFrom(qmlContext(delegate)->contextProperty("position").toLongLong());
    while( index != items.size() ){
        if ( items[index]->delegate == delegate )
            return index;
        ++index;
    }
    return index;
}

int QRangeViewPrivate::findRelativeIndex(int index){
    if ( index >= items.size() )
        return 0;

    QRangeViewItem* rangeItem = items[index];
    int itemIndex = 0;
    while ( index > 0 ){
        --index;
        if ( items[index]->position == rangeItem->position && items[index]->length == rangeItem->length)
            ++itemIndex;
        else
            break;
    }
    return itemIndex;
}

void QRangeViewPrivate::clearItemsInInterval(qint64 positionBegin, qint64 positionEnd){
    int endIndex   = findItemFrom(positionEnd);
    int index      = endIndex;
    while ( index > 0 ){
        --index;
        if ( items[index]->position + items[index]->length < positionBegin ){
            ++index;
            break;
        }
        delete items[index];
    }

    if (index != endIndex)
        items.erase(items.begin() + index, items.begin() + endIndex);
}

void QRangeViewPrivate::clearItems(){
    for ( QList<QRangeViewItem*>::iterator it = items.begin(); it != items.end(); ++it )
        delete *it;
    items.clear();
}

QString QRangeViewPrivate::listItems(){
    QString base = "";
    for ( int i = 0; i < items.size(); ++i ){
        if ( i != 0 )
            base += " ";
        base += "[" + QString::number(items[i]->position) + "," +
                      QString::number(items[i]->position + items[i]->length) +  "]";
    }
    return base;
}

// QRangeView
// ----------------------------------------------------------------------------

QRangeView::QRangeView(QQuickItem *parent)
    : QQuickItem(parent)
    , d_ptr(new QRangeViewPrivate)
    , m_viewportWidth(0)
    , m_viewportX(0)
{
    setFlag(QQuickItem::ItemHasContents, true);
}

QRangeView::~QRangeView(){
    delete d_ptr;
}

void QRangeView::modelWidthChanged(){
    Q_D(QRangeView);
    setImplicitWidth(d->model->contentWidth());
}

void QRangeView::modelitemCoordinatesChanged(qint64 oldPosition, qint64 oldLength, int relativeIndex,
        QAbstractRangeModelIterator *modelIt, int newRelativeIndex)
{
    Q_D(QRangeView);

    QRangeViewItem* item = 0;

    if ( modelIt->isEnd() ){
        qWarning("Empty model iterator sent by model.");
        return;
    }

    qint64 newPosition = modelIt->data(QAbstractRangeModel::PositionRole).toLongLong();
    qint64 newLength   = modelIt->data(QAbstractRangeModel::LengthRole).toLongLong();

    if ( oldPosition < m_viewportX + m_viewportWidth && oldPosition + oldLength > m_viewportX ){
        int index = d->findItem(oldPosition, oldLength) + relativeIndex;
        if ( index >= d->items.size() ){
            qWarning("Failed to find item from given position: %lld, %lld", oldPosition, oldLength);
            return;
        }
        item = d->items.takeAt(index);
    }

    if ( newPosition >= m_viewportX + m_viewportWidth || newPosition + newLength <= m_viewportX ){
        if (item)
            delete item;
    } else {
        if (!item){
            item = new QRangeViewItem;
            item->context = new QQmlContext(d->delegate->creationContext(), this);
            item->context->setContextProperty("position", newPosition);
            item->context->setContextProperty("length",   newLength);
            for(QHash<int, QByteArray>::iterator rolesIt = d->modelRoles.begin(); rolesIt != d->modelRoles.end(); ++rolesIt){
                item->context->setContextProperty(rolesIt.value(), modelIt->data(rolesIt.key()));
            }

            item->delegate = qobject_cast<QQuickItem*>(d->delegate->create(item->context));
            item->delegate->setParentItem(parentItem());
        }

        item->position = newPosition;
        item->length   = newLength;
        item->context->setContextProperty("position", newPosition);
        item->context->setContextProperty("length",   newLength);

        int newIndex = d->findItemFrom(newPosition);
        while ( newIndex < d->items.size() ){
            if ( d->items[newIndex]->position == newPosition && d->items[newIndex]->length < newLength )
                ++newIndex;
            else {
                d->items.insert(newIndex + newRelativeIndex, item);
                return;
            }
        }
        d->items.insert(newIndex, item);
    }
}

void QRangeView::modelItemDataChanged(QAbstractRangeModelIterator* modelIt, int relativeIndex, QList<int> roles){
    Q_D(QRangeView);

    int itemAbsoluteIndex = d->items.size();
    if ( !modelIt->isEnd() ){
        qint64 position = modelIt->data(QAbstractRangeModel::PositionRole).toLongLong();
        qint64 length   = modelIt->data(QAbstractRangeModel::LengthRole).toLongLong();
        if ( position >= m_viewportX + m_viewportWidth || position + length <= m_viewportX )
            return;

        itemAbsoluteIndex = d->findItem(position, length) + relativeIndex;
        if ( itemAbsoluteIndex >= d->items.size() ){
            qWarning("Failed to find item from given model.");
            return;
        }

        for ( int i = 0; i < roles.size(); ++i ){
            d->items[itemAbsoluteIndex]->context->setContextProperty(
                d->modelRoles[roles[i]], modelIt->data(roles[i])
            );
        }
    }
}

void QRangeView::modelItemsAboutToBeChanged(qint64, qint64){}

void QRangeView::modelItemsChanged(qint64 beginPosition, qint64 endPosition){
    Q_D(QRangeView);

    if ( beginPosition >= m_viewportX + m_viewportWidth || endPosition <= m_viewportX )
        return;

    d->clearItemsInInterval(beginPosition, endPosition);

    QAbstractRangeModelIterator* modelIt = d->model->dataBetween(beginPosition, endPosition);

    QList<QRangeViewItem*>::iterator itemsIt = d->items.end();
    if ( !modelIt->isEnd() ){
        itemsIt = d->items.begin() + d->findItemFrom(modelIt->data(QAbstractRangeModel::PositionRole).toLongLong());
    }

    while ( !modelIt->isEnd() ){
        QRangeViewItem* viewItem = new QRangeViewItem;
        viewItem->position = modelIt->data(QAbstractRangeModel::PositionRole).toLongLong();
        viewItem->length   = modelIt->data(QAbstractRangeModel::LengthRole).toLongLong();

        viewItem->context = new QQmlContext(d->delegate->creationContext(), this);
        viewItem->context->setContextProperty("position", viewItem->position);
        viewItem->context->setContextProperty("length",   viewItem->length);

        for(QHash<int, QByteArray>::iterator rolesIt = d->modelRoles.begin(); rolesIt != d->modelRoles.end(); ++rolesIt){
            viewItem->context->setContextProperty(rolesIt.value(), modelIt->data(rolesIt.key()));
        }

        viewItem->delegate = qobject_cast<QQuickItem*>(d->delegate->create(viewItem->context));
        viewItem->delegate->setParentItem(parentItem());

        itemsIt = d->items.insert(itemsIt, viewItem);
        ++itemsIt;

        modelIt->nextItem();
    }

    delete modelIt;
}

void QRangeView::setPositionViaDelegate(QQuickItem* item, qint64 position){
    Q_D(QRangeView);
    int index = d->findItemByDelegate(item);
    if ( index == d->items.size() ){
        qWarning("Cannot find item by its position: %lld", static_cast<qint64>(item->x()));
        return;
    }

    QRangeViewItem* rangeItem = d->items[index];
    if ( rangeItem->position == position )
        return;

    int relativeIndex = d->findRelativeIndex(index);

    d->model->setItemPosition(rangeItem->position, rangeItem->length, relativeIndex, position);
}

void QRangeView::setLengthViaDelegate(QQuickItem* item, qint64 newLength){
    Q_D(QRangeView);
    int index = d->findItemByDelegate(item);
    if ( index == d->items.size() ){
        qWarning("Cannot find item by its position: %lld", static_cast<qint64>(item->x()));
        return;
    }

    QRangeViewItem* rangeItem = d->items[index];
    if ( rangeItem->length == newLength )
        return;

    int relativeIndex = d->findRelativeIndex(index);

    d->model->setItemLength(rangeItem->position, rangeItem->length, relativeIndex, newLength);

}

void QRangeView::setDataViaDelegate(QQuickItem* item, const QString& role, const QVariant& value){
    Q_D(QRangeView);
    int index = d->findItemByDelegate(item);
    if ( index == d->items.size() ){
        qWarning("Cannot find item by its position: %lld.", static_cast<qint64>(item->x()));
        return;
    }

    QRangeViewItem* rangeItem = d->items[index];
    int relativeIndex = d->findRelativeIndex(index);

    d->model->setItemData(
        rangeItem->position,
        rangeItem->length,
        relativeIndex,
        d->modelRoles.key(role.toLatin1()),
        value
                );
}

void QRangeView::removeItemViaDelegate(QQuickItem *item){
    Q_D(QRangeView);
    int index = d->findItemByDelegate(item);
    if ( index == d->items.size() ){
        qWarning("Cannot find item by its position: %lld.", static_cast<qint64>(item->x()));
        return;
    }

    QRangeViewItem* rangeItem = d->items[index];
    int relativeIndex = d->findRelativeIndex(index);

    d->model->removeItem(rangeItem->position, rangeItem->length, relativeIndex);
}

void QRangeView::componentComplete(){
    regenerateNewContent();
}

void QRangeView::regenerateNewContent(){
    Q_D(QRangeView);
    if ( !d->delegate || !d->model )
        return;

    if ( m_viewportX > d->lastViewportX ){ // delete left
        QList<QRangeViewItem*>::iterator it = d->items.begin();
        while ( it != d->items.end() ){
            if ( (*it)->position + (*it)->length > m_viewportX ){
                if ( it != d->items.begin() )
                    d->items.erase(d->items.begin(), it);
                break;
            }
            delete *it;
            ++it;
        }
        if ( it == d->items.end() )
            d->items.clear();
    } else if ( m_viewportX < d->lastViewportX ){ // append left
        QAbstractRangeModelIterator *modelIt = d->model->dataBetween(
            m_viewportX,
            d->lastViewportX < m_viewportX + m_viewportWidth ? d->lastViewportX : m_viewportX + m_viewportWidth
        );

        QList<QRangeViewItem*>::iterator itemsIt = d->items.end();
        if ( !modelIt->isEnd() ){
            itemsIt = d->items.begin() + d->findItemFrom(modelIt->data(QAbstractRangeModel::PositionRole).toLongLong());
            if ( itemsIt != d->items.begin() && itemsIt != d->items.end() )
                --itemsIt;
        }

        while ( !modelIt->isEnd() ){

            qint64 position = modelIt->data(QAbstractRangeModel::PositionRole).toLongLong();
            qint64 length   = modelIt->data(QAbstractRangeModel::LengthRole).toLongLong();
            if ( position + length > d->lastViewportX && d->items.size() > 0 ){
                if ( d->items.first()->position < position ||
                     ( d->items.first()->position == position && d->items.first()->length < length ) )
                        break;
            }

            QRangeViewItem* viewItem = new QRangeViewItem;
            viewItem->position = position;
            viewItem->length   = length;

            viewItem->context = new QQmlContext(d->delegate->creationContext(), this);
            viewItem->context->setContextProperty("position", position);
            viewItem->context->setContextProperty("length",   length);

            for(QHash<int, QByteArray>::iterator rolesIt = d->modelRoles.begin(); rolesIt != d->modelRoles.end(); ++rolesIt){
                viewItem->context->setContextProperty(rolesIt.value(), modelIt->data(rolesIt.key()));
            }

            viewItem->delegate = qobject_cast<QQuickItem*>(d->delegate->create(viewItem->context));
            viewItem->delegate->setParentItem(parentItem());

            modelIt->nextItem();

            itemsIt = d->items.insert(itemsIt, viewItem);
            ++itemsIt;
        }

        delete modelIt;
    }

    if ( m_viewportX + m_viewportWidth < d->lastViewportX + d->lastViewportWidth ){ // delete right
        QList<QRangeViewItem*>::iterator it = d->items.end();
        while ( it != d->items.begin() ){
            --it;
            if ( (*it)->position < m_viewportX + m_viewportWidth ){
                ++it;
                if ( it != d->items.end() )
                    d->items.erase(it, d->items.end());
                break;
            }
            delete *it;
        }
        if ( it == d->items.begin() )
            d->items.clear();

    } else if ( m_viewportX + m_viewportWidth > d->lastViewportX + d->lastViewportWidth ){ // append right
        QAbstractRangeModelIterator* modelIt = d->model->dataBetween(
            m_viewportX > d->lastViewportX + d->lastViewportWidth ? m_viewportX : d->lastViewportX + d->lastViewportWidth,
            m_viewportX + m_viewportWidth
        );

        while ( !modelIt->isEnd() ){
            if ( modelIt->data(QAbstractRangeModel::PositionRole).toLongLong() >= d->lastViewportX + d->lastViewportWidth )
                break;
            modelIt->nextItem();
        }

        QList<QRangeViewItem*>::iterator itemsIt = d->items.end();
        if ( !modelIt->isEnd() ){
            itemsIt = d->items.begin() + d->findItemFrom(modelIt->data(QAbstractRangeModel::PositionRole).toLongLong());
            if ( itemsIt != d->items.begin() && itemsIt != d->items.end() )
                --itemsIt;
        }

        while ( !modelIt->isEnd() ){

            QRangeViewItem* viewItem = new QRangeViewItem;
            viewItem->position = modelIt->data(QAbstractRangeModel::PositionRole).toLongLong();
            viewItem->length   = modelIt->data(QAbstractRangeModel::LengthRole).toLongLong();

            viewItem->context = new QQmlContext(d->delegate->creationContext(), this);
            viewItem->context->setContextProperty("position", viewItem->position);
            viewItem->context->setContextProperty("length",   viewItem->length);

            for(QHash<int, QByteArray>::iterator rolesIt = d->modelRoles.begin(); rolesIt != d->modelRoles.end(); ++rolesIt){
                viewItem->context->setContextProperty(rolesIt.value(), modelIt->data(rolesIt.key()));
            }

            viewItem->delegate = qobject_cast<QQuickItem*>(d->delegate->create(viewItem->context));
            viewItem->delegate->setParentItem(parentItem());

            modelIt->nextItem();

            itemsIt = d->items.insert(itemsIt, viewItem);
            ++itemsIt;
        }

        delete modelIt;
    }

    d->lastViewportX     = m_viewportX;
    d->lastViewportWidth = m_viewportWidth;
}

QQmlComponent*QRangeView::delegate() const{
    const Q_D(QRangeView);
    return d->delegate;
}

void QRangeView::setDelegate(QQmlComponent* delegate){
    Q_D(QRangeView);
    if ( d->delegate != delegate ){
        d->delegate = delegate;
        emit delegateChanged();
        regenerateNewContent();
        update();
    }
}

QAbstractRangeModel*QRangeView::model() const{
    const Q_D(QRangeView);
    return d->model;
}

void QRangeView::setModel(QAbstractRangeModel* arg){
    Q_D(QRangeView);
    if (d->model == arg)
        return;

    if (d->model)
        disconnect(d->model, 0, this, 0);

    d->model = arg;
    if ( d->model == 0 )
        return;

    d->modelRoles = d->model->roleNames();
    setImplicitWidth(d->model->contentWidth());

    connect(d->model, SIGNAL(contentWidthChanged()), this, SLOT(modelWidthChanged()));
    connect(d->model, SIGNAL(itemCoordinatesChanged(qint64,qint64,int,QAbstractRangeModelIterator*,int)),
            this, SLOT(modelitemCoordinatesChanged(qint64,qint64,int,QAbstractRangeModelIterator*,int)));
    connect(d->model, SIGNAL(itemsDataChanged(QAbstractRangeModelIterator*,int, QList<int>)),
            this, SLOT(modelItemDataChanged(QAbstractRangeModelIterator*,int, QList<int>)));
    connect(d->model, SIGNAL(itemsAboutToBeChanged(qint64,qint64)),
            this, SLOT(modelItemsAboutToBeChanged(qint64,qint64)));
    connect(d->model, SIGNAL(itemsChanged(qint64,qint64)),
            this, SLOT(modelItemsChanged(qint64,qint64)));

    d->lastViewportX     = 0;
    d->lastViewportWidth = 0;
    regenerateNewContent();

    emit modelChanged(arg);
}

