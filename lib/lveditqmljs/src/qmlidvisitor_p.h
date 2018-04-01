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

#ifndef LVQMLIDVISITOR_H
#define LVQMLIDVISITOR_H

#include "qmljs/qmljsinterpreter.h"
#include "qmljs/parser/qmljsast_p.h"
#include "qmljs/qmljsdescribevalue.h"
#include "live/documentqmlobject.h"
#include <QMap>

namespace lv{

class IdExtractor : public QmlJS::MemberProcessor{

public:
    bool processProperty(const QString &name, const QmlJS::Value *, const QmlJS::PropertyInfo &) override{
        m_ids << name;
        return true;
    }

    QStringList ids() const{ return m_ids; }

private:
    QStringList m_ids;
};

class IdValueExtractor : public QmlJS::MemberProcessor{

public:
    IdValueExtractor(const QString& name)
        : m_name(name)
        , m_value(0)
    {
    }

    bool processProperty(const QString &name, const QmlJS::Value *value, const QmlJS::PropertyInfo &) override{
        if ( name == m_name ){
            m_value = value;
            return false;
        }
        return true;
    }

    const QmlJS::Value* value(){ return m_value; }

private:
    QString m_name;
    const QmlJS::Value* m_value;
};

class ValueMemberExtractor : public QmlJS::MemberProcessor{
public:
    ValueMemberExtractor(DocumentQmlObject* object) : m_parent(0), m_object(object)
    {}

    bool processProperty(const QString &name, const QmlJS::Value *value, const QmlJS::PropertyInfo&) override{
        QString type = "object";
        if ( name == "parent" ){
            m_parent = value;
            return true;
        } else if ( const QmlJS::ASTPropertyReference* vr = value->asAstPropertyReference() ){
            type = (vr->ast() != 0) ? vr->ast()->memberType.toString() : QString("object");
        } else if ( const QmlJS::ASTFunctionValue* fv = value->asAstFunctionValue() ){
            type = "function";
            DocumentQmlObject::FunctionValue mf;
            mf.name = name;
            for( int i = 0; i < fv->namedArgumentCount(); ++i ){
                mf.arguments.append(QPair<QString, QString>(fv->argumentName(i), ""));
            }
            m_object->addFunction(mf);
            return true;
        }
        m_object->addProperty(name, type);
        return true;
    }
    bool processEnumerator(const QString &, const QmlJS::Value *) override
    {
        return false;
    }
    bool processSignal(const QString &name, const QmlJS::Value *value) override
    {
        if ( const QmlJS::ASTSignal* vs = value->asAstSignal() ){
            DocumentQmlObject::FunctionValue mf;
            mf.name = name;
            for( int i = 0; i < vs->namedArgumentCount(); ++i ){
                mf.arguments.append(QPair<QString, QString>(vs->argumentName(i), ""));
            }
            m_object->addSignal(mf);
        }
        return true;
    }
    bool processSlot(const QString &name, const QmlJS::Value *value) override
    {
        if ( const QmlJS::ASTPropertyReference* vr = value->asAstPropertyReference() ){
            m_object->addSlot(name, vr->ast()->name.toString());
        } else {
            m_object->addSlot(name, "");
        }
        return true;
    }
    bool processGeneratedSlot(const QString &name, const QmlJS::Value *value) override
    {
        if ( const QmlJS::ASTPropertyReference* vr = value->asAstPropertyReference() ){
            m_object->addSlot(name, vr->ast()->name.toString());
        } else if ( const QmlJS::ASTSignal* vs = value->asAstSignal() ){
            m_object->addSlot(name, vs->ast()->name.toString());
        } else {
            m_object->addSlot(name, "");
        }
        return true;
    }

    const QmlJS::Value* parent(){ return m_parent; }

private:
    const QmlJS::Value* m_parent;
    DocumentQmlObject* m_object;
};

}// namespace

#endif // LVQMLIDVISITOR_H
