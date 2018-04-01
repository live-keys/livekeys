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

#ifndef LVCODECONVERTER_H
#define LVCODECONVERTER_H

#include <QObject>

#include "live/lveditorglobal.h"
#include "live/abstractcodeserializer.h"

namespace lv{

class LV_EDITOR_EXPORT CodeConverter : public QObject{

    Q_OBJECT
    Q_PROPERTY(AbstractCodeSerializer* serialize READ serialize  WRITE setSerialize  NOTIFY serializeChanged)
    Q_PROPERTY(QString type                       READ type       WRITE setType       NOTIFY typeChanged)
    Q_PROPERTY(QString typeObject                 READ typeObject WRITE setTypeObject NOTIFY typeObjectChanged)

public:
    explicit CodeConverter(QObject *parent = 0);
    virtual ~CodeConverter();

    AbstractCodeSerializer* serialize();
    void setSerialize(AbstractCodeSerializer* serialize);

    void setType(const QString& type);
    const QString& type() const;

    void setTypeObject(const QString& typeObject);
    const QString& typeObject() const;

signals:
    void serializeChanged();
    void typeChanged();
    void typeObjectChanged();

private:
    Q_DISABLE_COPY(CodeConverter)

    AbstractCodeSerializer* m_serialize;
    QString m_type;
    QString m_typeObject;
};

inline AbstractCodeSerializer *CodeConverter::serialize(){
    return m_serialize;
}

inline const QString &CodeConverter::type() const{
    return m_type;
}

inline const QString &CodeConverter::typeObject() const{
    return m_typeObject;
}

inline void CodeConverter::setSerialize(AbstractCodeSerializer *serialize){
    if (m_serialize == serialize)
        return;

    m_serialize = serialize;
    emit serializeChanged();
}

inline void CodeConverter::setType(const QString &type){
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

inline void CodeConverter::setTypeObject(const QString &typeObject){
    if (m_typeObject == typeObject)
        return;

    m_typeObject = typeObject;
    emit typeObjectChanged();
}

}// namespace

#endif // LVCODECONVERTER_H
