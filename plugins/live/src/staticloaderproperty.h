/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef LVSTATICLOADERPROPERTY_H
#define LVSTATICLOADERPROPERTY_H

#include <QObject>
#include <QVariant>

namespace lv{

/// \private
class StaticLoaderProperty : public QObject{

    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit StaticLoaderProperty(QObject *parent = 0);
    ~StaticLoaderProperty();

    const QVariant& value() const;
    void setValue(const QVariant& value);

signals:
    void valueChanged();

private:
    QVariant m_value;

};

inline const QVariant &StaticLoaderProperty::value() const{
    return m_value;
}

}// namespace

#endif // LVSTATICLOADERPROPERTY_H
