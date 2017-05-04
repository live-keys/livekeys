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

#ifndef QDOCUMENTQMLOBJECT_H
#define QDOCUMENTQMLOBJECT_H

#include "qqmljsparserglobal.h"
#include <QString>
#include <QMap>

namespace lcv{

class Q_QMLJSPARSER_EXPORT QDocumentQmlObject{

public:
    class FunctionValue{
    public:
        QString name;
        QString returnType;
        QList<QPair<QString, QString> > arguments;
    };

    QDocumentQmlObject();
    ~QDocumentQmlObject();

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

inline void QDocumentQmlObject::setTypeName(const QString &name){
    m_className = name;
}

inline void QDocumentQmlObject::addProperty(const QString &name, const QString &type){
    m_memberProperties[name] = type;
}

inline void QDocumentQmlObject::addFunction(const QDocumentQmlObject::FunctionValue &value){
    m_memberFunctions[value.name] = value;
}

inline void QDocumentQmlObject::addSlot(const QString &name, const QString &member){
    m_memberSlots[name] = member;
}

inline void QDocumentQmlObject::addSignal(const QDocumentQmlObject::FunctionValue &value){
    m_memberSignals[value.name] = value;
}

inline const QString &QDocumentQmlObject::typeName() const{
    return m_className;
}

inline const QMap<QString, QString> &QDocumentQmlObject::memberProperties() const{
    return m_memberProperties;
}

inline const QMap<QString, QString> &QDocumentQmlObject::memberSlots() const{
    return m_memberSlots;
}

inline const QMap<QString, QDocumentQmlObject::FunctionValue> &QDocumentQmlObject::memberFunctions() const{
    return m_memberFunctions;
}

inline const QMap<QString, QDocumentQmlObject::FunctionValue> &QDocumentQmlObject::memberSignals() const{
    return m_memberSignals;
}

}// namespace

#endif // QDOCUMENTQMLOBJECT_H
