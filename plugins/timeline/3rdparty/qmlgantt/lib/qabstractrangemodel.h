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

#ifndef QABSTRACTRANGEMODEL_H
#define QABSTRACTRANGEMODEL_H

#include "qganttglobal.h"
#include <QObject>

// QAbstractRangeModelIterator
// --------------------------------

class Q_GANTT_EXPORT QAbstractRangeModelIterator{

public:
    virtual ~QAbstractRangeModelIterator(){}

    virtual bool isEnd() const = 0;
    virtual void nextItem() = 0;
    virtual QVariant data(int role = 0) = 0;

};


// QAbstractRangeModel
// ------------------------

class QAbstractRangeModelPrivate;
class Q_GANTT_EXPORT QAbstractRangeModel : public QObject{

    Q_OBJECT
    Q_PROPERTY(qint64 contentWidth READ contentWidth NOTIFY contentWidthChanged)

public:
    enum Role{
        PositionRole = 0,
        LengthRole
    };

public:
    explicit QAbstractRangeModel(QObject* parent = nullptr);
    virtual ~QAbstractRangeModel();

    qint64 contentWidth() const;
    void setContentWidth(qint64 contentWidth);

    virtual QAbstractRangeModelIterator* dataBetween(qint64 startPosition, qint64 endPosition) = 0;

    virtual void setItemPosition(
        qint64 itemPosition,
        qint64 itemLength,
        int relativeIndex,
        qint64 itemNewPosition
    ) = 0;
    virtual void setItemLength(
        qint64 itemPosition,
        qint64 itemLength,
        int relativeIndex,
        qint64 newLength
    ) = 0;
    virtual void setItemData(
        qint64 itemPosition,
        qint64 itemLength,
        int relativeIndex,
        int role,
        const QVariant& value
    ) = 0;
    virtual QHash<int, QByteArray> roleNames() const = 0;

protected:
    void beginDataChange(qint64 startPosition, qint64 endPosition);
    void endDataChange();

signals:
    void contentWidthChanged();
    void itemsAboutToBeChanged(qint64 startPosition, qint64 endPosition);
    void itemsChanged(qint64 startPosition, qint64 endPosition);
    void itemCoordinatesChanged(
            qint64 oldPosition, qint64 oldLength, int relativeIndex,
            QAbstractRangeModelIterator* modelIt, int newRelativeIndex);
    void itemsDataChanged(QAbstractRangeModelIterator*, int relativeIndex, QList<int> roles = QList<int>() );

public slots:
    virtual void insertItem(qint64 position, qint64 length);
    virtual void removeItem(qint64 position, qint64 length, qint64 relativeIndex);

private:
    QAbstractRangeModel(const QAbstractRangeModel&);
    QAbstractRangeModel& operator = (const QAbstractRangeModel&);

    QAbstractRangeModelPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(QAbstractRangeModel)

    qint64 m_contentWidth;
};


inline qint64 QAbstractRangeModel::contentWidth() const{
    return m_contentWidth;
}

inline void QAbstractRangeModel::setContentWidth(qint64 contentWidth){
    if ( m_contentWidth != contentWidth ){
        m_contentWidth = contentWidth;
        emit contentWidthChanged();
    }
}

#endif // QABSTRACTRANGEMODEL_H
