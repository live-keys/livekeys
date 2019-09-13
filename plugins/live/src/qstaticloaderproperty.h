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

#ifndef QSTATICLOADERPROPERTY_H
#define QSTATICLOADERPROPERTY_H

#include <QObject>
#include <QVariant>

/// \private
class QStaticLoaderProperty : public QObject{

    Q_OBJECT
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit QStaticLoaderProperty(QObject *parent = 0);
    ~QStaticLoaderProperty();

    const QVariant& value() const;
    void setValue(const QVariant& value);

signals:
    void valueChanged();

private:
    QVariant m_value;

};

inline const QVariant &QStaticLoaderProperty::value() const{
    return m_value;
}

#endif // QSTATICLOADERPROPERTY_H
