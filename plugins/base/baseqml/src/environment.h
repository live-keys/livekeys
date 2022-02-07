/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QObject>
#include <QVariantMap>

namespace lv{

class Environment : public QObject{

    Q_OBJECT
    Q_PROPERTY(QVariantMap os READ os CONSTANT)

public:
    explicit Environment(QObject *parent = 0);
    ~Environment();

    QVariantMap os() const;

signals:

public slots:

private:
    QVariantMap m_os;
};


inline QVariantMap Environment::os() const{
    return m_os;
}

}// namespace

#endif // ENVIRONMENT_H
