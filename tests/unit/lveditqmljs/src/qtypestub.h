/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#ifndef QTYPESTUB_HPP
#define QTYPESTUB_HPP

#include <QObject>

class QTypeStub : public QObject{

    Q_OBJECT
    Q_PROPERTY(int property1     READ property1 WRITE setProperty1 NOTIFY property1Changed)
    Q_PROPERTY(QString property2 READ property2 WRITE setProperty2 NOTIFY property2Changed)

public:
    explicit QTypeStub(QObject *parent = 0);

    int property1() const;
    QString property2() const;

signals:
    void property1Changed(int property1);
    void property2Changed(QString property2);

public slots:
    void setProperty1(int property1);
    void setProperty2(QString property2);

private:
    int m_property1;
    QString m_property2;
};

inline int QTypeStub::property1() const{
    return m_property1;
}

inline QString QTypeStub::property2() const{
    return m_property2;
}

inline void QTypeStub::setProperty1(int property1){
    if (m_property1 == property1)
        return;

    m_property1 = property1;
    emit property1Changed(property1);
}

inline void QTypeStub::setProperty2(QString property2){
    if (m_property2 == property2)
        return;

    m_property2 = property2;
    emit property2Changed(property2);
}

#endif // QTYPESTUB_HPP
