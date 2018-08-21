/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtQuick module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "textutil_p.h"

#include <QtQml/qqmlinfo.h>

//#include "qqmlglobal_p.h"

namespace lv {

QQuickItem *TextUtil::createCursor(
        QQmlComponent *component, QQuickItem *parent, const QRectF &rectangle, const char *className)
{
    Q_UNUSED(rectangle)
    QQuickItem *item = 0;
    if (component->isReady()) {
        QQmlContext *creationContext = component->creationContext();

        if (QObject *object = component->beginCreate(creationContext
                ? creationContext
                : qmlContext(parent))) {
            Q_UNUSED(object)
//            if ((item = qobject_cast<QQuickItem *>(object))) {
//                QQml_setParent_noEvent(item, parent);
//                item->setParentItem(parent);
//                item->setPosition(rectangle.topLeft());
//                item->setHeight(rectangle.height());
//            } else {
                qmlInfo(parent) << tr("%1 does not support loading non-visual cursor delegates.")
                        .arg(QString::fromUtf8(className));
//            }
            component->completeCreate();
            return item;
        }
    } else if (component->isLoading()) {
        QObject::connect(component, SIGNAL(statusChanged(QQmlComponent::Status)),
                parent, SLOT(createCursor()), Qt::UniqueConnection);
        return item;
    }
    qmlInfo(parent, component->errors()) << tr("Could not load cursor delegate");
    return item;
}

qreal TextUtil::alignedX(const qreal textWidth, const qreal itemWidth, int alignment)
{
    qreal x = 0;
    switch (alignment) {
    case Qt::AlignLeft:
    case Qt::AlignJustify:
        break;
    case Qt::AlignRight:
        x = itemWidth - textWidth;
        break;
    case Qt::AlignHCenter:
        x = (itemWidth - textWidth) / 2;
        break;
    }
    return x;
}

qreal TextUtil::alignedY(const qreal textHeight, const qreal itemHeight, int alignment)
{
    qreal y = 0;
    switch (alignment) {
    case Qt::AlignTop:
        break;
    case Qt::AlignBottom:
        y = itemHeight - textHeight;
        break;
    case Qt::AlignVCenter:
        y = (itemHeight - textHeight) / 2;
        break;
    }
    return y;
}

}
