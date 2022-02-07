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

#ifndef QRANGEVIEW_H
#define QRANGEVIEW_H

#include "qganttglobal.h"
#include "qabstractrangemodel.h"

#include <QQuickItem>
#include <QQmlComponent>

class QRangeViewPrivate;
class Q_GANTT_EXPORT QRangeView : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QQmlComponent* delegate           READ delegate      WRITE setDelegate      NOTIFY delegateChanged)
    Q_PROPERTY(QAbstractRangeModel* model   READ model         WRITE setModel         NOTIFY modelChanged)
    Q_PROPERTY(qint64 viewportX                  READ viewportX     WRITE setViewportX     NOTIFY viewportXChanged)
    Q_PROPERTY(qint64 viewportWidth              READ viewportWidth WRITE setViewportWidth NOTIFY viewportWidthChanged)

public:
    explicit QRangeView(QQuickItem *parent = 0);
    ~QRangeView();

    QQmlComponent* delegate() const;
    void setDelegate(QQmlComponent* delegate);

    qint64 viewportWidth() const;
    qint64 viewportX() const;

    QAbstractRangeModel* model() const;

signals:
    void delegateChanged();
    void viewportWidthChanged(qint64 arg);
    void viewportXChanged(qint64 arg);
    void modelChanged(QAbstractRangeModel* arg);

public slots:
    void setViewportWidth(qint64 arg);
    void setViewportX(qint64 arg);
    void setModel(QAbstractRangeModel* arg);

    void modelWidthChanged();
    void modelitemCoordinatesChanged(
            qint64 oldPosition, qint64 oldLength, int relativeIndex,
            QAbstractRangeModelIterator* modelIt, int newRelativeIndex);
    void modelItemDataChanged(QAbstractRangeModelIterator* modelIt, int relativeIndex, QList<int> roles);
    void modelItemsAboutToBeChanged(qint64 beginPosition, qint64 endPosition);
    void modelItemsChanged(qint64 beginPosition, qint64 endPosition);

    void setPositionViaDelegate(QQuickItem* delegate, qint64 position);
    void setLengthViaDelegate(QQuickItem* delegate, qint64 length);
    void setDataViaDelegate(QQuickItem* delegate, const QString& role, const QVariant& value);
    void removeItemViaDelegate(QQuickItem* delegate);

protected:
    void componentComplete();

private:
    QRangeView(const QRangeView&);
    QRangeView& operator = (const QRangeView&);

    QRangeViewPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(QRangeView)

    void regenerateNewContent();

    int m_viewportWidth;
    int m_viewportX;
};

inline qint64 QRangeView::viewportWidth() const{
    return m_viewportWidth;
}

inline qint64 QRangeView::viewportX() const{
    return m_viewportX;
}

inline void QRangeView::setViewportWidth(qint64 arg){
    if (m_viewportWidth == arg)
        return;

    m_viewportWidth = arg;
    regenerateNewContent();
    emit viewportWidthChanged(arg);
}

inline void QRangeView::setViewportX(qint64 arg){
    if (m_viewportX == arg)
        return;

    m_viewportX = arg;
    regenerateNewContent();
    emit viewportXChanged(arg);
}


#endif // QRANGEVIEW_H
