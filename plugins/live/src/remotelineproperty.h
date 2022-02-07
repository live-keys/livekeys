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

#ifndef TCPLINEPROPERTY_H
#define TCPLINEPROPERTY_H

#include <QObject>

namespace lv{

class RemoteLine;
class RemoteLineProperty : public QObject{

    Q_OBJECT

public:
    explicit RemoteLineProperty(const QString& name, RemoteLine *parent = nullptr);
    ~RemoteLineProperty(){}

    const QString& name();

public slots:
    void changed();

private:
    RemoteLine* m_line;
    QString  m_name;

};

inline const QString &RemoteLineProperty::name(){
    return m_name;
}

}// namespace

#endif // TCPLINEPROPERTY_H
