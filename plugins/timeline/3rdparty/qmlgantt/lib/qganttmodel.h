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

#ifndef QGANTTMODEL_H
#define QGANTTMODEL_H

#include "qganttglobal.h"
#include "qabstractrangemodel.h"

// QGanttModelIterator
// -------------------

class QGanttModelItem;
class Q_GANTT_EXPORT QGanttModelIterator : public QAbstractRangeModelIterator{

public:
    QGanttModelIterator(
            QList<QGanttModelItem*>::iterator it,
            QList<QGanttModelItem*>::iterator endIt,
            qint64 endPosition)
        : m_endIt(endIt)
        , m_currentIt(it)
        , m_endPosition(endPosition)
    {}

    bool isEnd() const;
    void nextItem();

    QVariant data(int role = 0);

private:
    QList<QGanttModelItem*>::iterator m_endIt;
    QList<QGanttModelItem*>::iterator m_currentIt;
    qint64 m_endPosition;

};

// QGanttModel
// -----------

class QGanttModelPrivate;
class Q_GANTT_EXPORT QGanttModel : public QAbstractRangeModel{

    Q_OBJECT

public:
    typedef QVariant (*ItemDataFactoryFunction)();

    enum Role{
        modelData = QAbstractRangeModel::LengthRole + 1
    };

public:
    explicit QGanttModel(QObject *parent = 0);
    ~QGanttModel();

    QAbstractRangeModelIterator* dataBetween(qint64 startPosition, qint64 endPosition);

    void setItemPosition(qint64 itemPosition, qint64 itemLength, int positionIndex, qint64 itemNewPosition);
    void setItemLength(qint64 itemPosition, qint64 itemLength, int positionIndex, qint64 newLength);
    void setItemData(qint64 itemPosition, qint64 itemLength, int positionIndex, int role, const QVariant& value);

    QHash<int, QByteArray> roleNames() const;

    int addSegment(QGanttModelItem* item);

    void setItemDataFactoryFunction(ItemDataFactoryFunction fp);

public slots:
    void addSegment(qint64 position, qint64 length);
    void removeItem(qint64 position, qint64 length, qint64 relativeIndex);

private:
    QGanttModel(const QGanttModel&);
    QGanttModel& operator = (const QGanttModel&);

    QGanttModelPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(QGanttModel)

};

#endif // QGANTTMODEL_H
