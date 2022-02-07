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

#ifndef LVQMLIDVISITOR_H
#define LVQMLIDVISITOR_H

#include "qmljs/qmljsinterpreter.h"
#include "qmljs/parser/qqmljsast_p.h"
#include "qmljs/qmljsdescribevalue.h"
#include "live/documentqmlobject.h"
#include "qmllanguageinfo_p.h"
#include <QMap>

namespace lv{

/// \private
class IdExtractor : public QQmlJS::MemberProcessor{

public:
    bool processProperty(const QString &name, const QQmlJS::Value *, const QQmlJS::PropertyInfo &) override{
        m_ids << name;
        return true;
    }

    QStringList ids() const{ return m_ids; }

private:
    QStringList m_ids;
};

/// \private
class IdValueExtractor : public QQmlJS::MemberProcessor{

public:
    IdValueExtractor(const QString& name)
        : m_name(name)
        , m_value(nullptr)
    {
    }

    bool processProperty(const QString &name, const QQmlJS::Value *value, const QQmlJS::PropertyInfo &) override{
        if ( name == m_name ){
            m_value = value;
            return false;
        }
        return true;
    }

    const QQmlJS::Value* value(){ return m_value; }

private:
    QString m_name;
    const QQmlJS::Value* m_value;
};

/// \private
class ValueMemberExtractor : public QQmlJS::MemberProcessor{
public:
    ValueMemberExtractor(QmlTypeInfo::Ptr object) : m_parent(nullptr), m_object(object)
    {}

    bool processProperty(const QString &name, const QQmlJS::Value *value, const QQmlJS::PropertyInfo&) override{
        QString type = "object";
        if ( name == "parent" ){
            m_parent = value;
            return true;
        } else if ( const QQmlJS::ASTPropertyReference* vr = value->asAstPropertyReference() ){
            if ( vr->ast()->memberType->next ){
                type = (vr->ast() != nullptr) ? (vr->ast()->memberType->name.toString() + "." + vr->ast()->memberType->next->name.toString()) : QString("object");
            } else {
                type = (vr->ast() != nullptr) ? vr->ast()->memberType->name.toString() : QString("object");
            }

        } else if ( const QQmlJS::ASTFunctionValue* fv = value->asAstFunctionValue() ){
            type = "function";
            QmlFunctionInfo mf;
            mf.name = name;
            for( int i = 0; i < fv->namedArgumentCount(); ++i ){
                mf.addParameter(fv->argumentName(i), QmlTypeReference(QmlTypeReference::Qml, "var"));
            }
            m_object->appendFunction(mf);
            return true;
        }
        QmlTypeReference typeReference = (QmlTypeInfo::isQmlBasicType(type))
                ? QmlTypeReference(QmlTypeReference::Qml, type)
                : QmlTypeReference(QmlTypeReference::Unknown, type);
        m_object->appendProperty(QmlPropertyInfo(name, typeReference));
        return true;
    }
    bool processEnumerator(const QString &, const QQmlJS::Value *) override
    {
        return false;
    }
    bool processSignal(const QString &name, const QQmlJS::Value *value) override
    {
        if ( const QQmlJS::ASTSignal* vs = value->asAstSignal() ){
            QmlFunctionInfo mf;
            mf.name = name;
            mf.functionType = QmlFunctionInfo::Signal;
            for( int i = 0; i < vs->namedArgumentCount(); ++i ){
                mf.addParameter(vs->argumentName(i), QmlTypeReference(QmlTypeReference::Qml, "var"));
            }
            m_object->appendFunction(mf);
        }
        return true;
    }
    bool processSlot(const QString &name, const QQmlJS::Value *) override
    {
        QmlFunctionInfo mf;
        mf.name = name;
        mf.functionType = QmlFunctionInfo::SlotGenerated;
        m_object->appendFunction(mf);
        return true;
    }
    bool processGeneratedSlot(const QString &name, const QQmlJS::Value *) override
    {
        QmlFunctionInfo mf;
        mf.name = name;
        mf.functionType = QmlFunctionInfo::SlotGenerated;
        m_object->appendFunction(mf);
        return true;
    }

    const QQmlJS::Value* parent(){ return m_parent; }

private:
    const QQmlJS::Value*  m_parent;
    QmlTypeInfo::Ptr     m_object;
};

}// namespace

#endif // LVQMLIDVISITOR_H
