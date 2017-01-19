/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef QGLOBALITEMPROPERTY_H
#define QGLOBALITEMPROPERTY_H

#include <QQuickItem>

class QGlobalItemProperty : public QQuickItem{

    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit QGlobalItemProperty(QQuickItem *parent = 0);
    ~QGlobalItemProperty();

    const QVariant& value() const;
    void setValue(const QVariant& value);

signals:
    void valueChanged();

private:
    QVariant m_value;

};

inline const QVariant& QGlobalItemProperty::value() const{
    return m_value;
}

#endif // QGLOBALITEMPROPERTY_H
