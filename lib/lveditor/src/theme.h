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

#ifndef LVTHEME_H
#define LVTHEME_H

#include <QObject>
#include "live/lveditorglobal.h"

namespace lv{

class LV_EDITOR_EXPORT Theme : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

public:
    explicit Theme(QObject *parent = 0);
    virtual ~Theme();

    const QString &name() const;
    void setName(const QString &name);

signals:
    void nameChanged();

private:
    QString m_name;
};

inline const QString& Theme::name() const{
    return m_name;
}

inline void Theme::setName(const QString& name){
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged();
}

}// namespace

#endif // LVTHEME_H
