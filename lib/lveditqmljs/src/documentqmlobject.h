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

#ifndef LVDOCUMENTQMLOBJECT_H
#define LVDOCUMENTQMLOBJECT_H

#include "live/lveditqmljsglobal.h"
#include <QString>
#include <QMap>

namespace lv{

class LV_EDITQMLJS_EXPORT DocumentQmlObject{

public:
    class FunctionValue{
    public:
        QString name;
        QString returnType;
        QList<QPair<QString, QString> > arguments;
    };

    DocumentQmlObject();
    ~DocumentQmlObject();

    void setTypeName(const QString& name);
    void addProperty(const QString& name, const QString& type);
    void addFunction(const FunctionValue& value);
    void addSlot(const QString& name, const QString& member);
    void addSignal(const FunctionValue& value);

    const QString& typeName() const;
    const QMap<QString, QString>& memberProperties() const;
    const QMap<QString, QString>& memberSlots() const;
    const QMap<QString, FunctionValue>& memberFunctions() const;
    const QMap<QString, FunctionValue>& memberSignals() const;

private:
    QString m_className;
    QMap<QString, QString> m_memberProperties;
    QMap<QString, QString> m_memberSlots;
    QMap<QString, FunctionValue> m_memberFunctions;
    QMap<QString, FunctionValue> m_memberSignals;

};

inline void DocumentQmlObject::setTypeName(const QString &name){
    m_className = name;
}

inline void DocumentQmlObject::addProperty(const QString &name, const QString &type){
    m_memberProperties[name] = type;
}

inline void DocumentQmlObject::addFunction(const DocumentQmlObject::FunctionValue &value){
    m_memberFunctions[value.name] = value;
}

inline void DocumentQmlObject::addSlot(const QString &name, const QString &member){
    m_memberSlots[name] = member;
}

inline void DocumentQmlObject::addSignal(const DocumentQmlObject::FunctionValue &value){
    m_memberSignals[value.name] = value;
}

inline const QString &DocumentQmlObject::typeName() const{
    return m_className;
}

inline const QMap<QString, QString> &DocumentQmlObject::memberProperties() const{
    return m_memberProperties;
}

inline const QMap<QString, QString> &DocumentQmlObject::memberSlots() const{
    return m_memberSlots;
}

inline const QMap<QString, DocumentQmlObject::FunctionValue> &DocumentQmlObject::memberFunctions() const{
    return m_memberFunctions;
}

inline const QMap<QString, DocumentQmlObject::FunctionValue> &DocumentQmlObject::memberSignals() const{
    return m_memberSignals;
}

}// namespace

#endif // LVDOCUMENTQMLOBJECT_H
