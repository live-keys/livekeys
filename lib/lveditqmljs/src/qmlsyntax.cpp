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

#include "qmlsyntax_p.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include "live/visuallogqt.h"

#include "qmljs/qmljsscanner.h"

namespace lv{

QmlSyntax::QmlSyntax(QObject *parent)
    : QObject(parent)
    , m_engine(nullptr)
{
    QQmlEngine* qmlengine = qobject_cast<QQmlEngine*>(parent);
    if ( qmlengine ){
        m_engine = qobject_cast<ViewEngine*>(qmlengine->property("viewEngine").value<QObject*>());
    }
}

QmlSyntax::~QmlSyntax(){
}

QJSValue QmlSyntax::parseObjectDeclarations(const QString &content){
    if ( !m_engine )
        return QJSValue();

    DocumentQmlInfo::Ptr docinfo = lv::DocumentQmlInfo::create("Syntax.qml");
    if ( !docinfo->parse(content) )
        return QJSValue();

    DocumentQmlValueObjects::Ptr objects = docinfo->createObjects();
    DocumentQmlValueObjects::RangeObject* root = objects->root();

    return recurseObjectDeclarations(root, content);
}

QJSValue QmlSyntax::parsePathDeclaration(const QString &content){
    if ( !m_engine )
        return QJSValue();

    QQmlEngine* e = m_engine->engine();
    QJSValue result = e->newObject();

    QQmlJS::Scanner scanner;
    QList<QQmlJS::Token> tokens = scanner(content);
    if ( tokens.size() == 0 ){
        result.setProperty("path", "");
        return result;
    }

    if ( tokens[0].kind == QQmlJS::Token::String ){
        result.setProperty("path", content.mid(tokens[0].begin() + 1, tokens[0].length - 2));
        result.setProperty("from", "");
        return result;
    }

    if ( tokens.size() > 5 ){
        if ( tokens[0].kind == QQmlJS::Token::Identifier &&
             tokens[1].kind == QQmlJS::Token::Dot &&
             tokens[2].kind == QQmlJS::Token::Identifier &&
             tokens[3].kind == QQmlJS::Token::LeftParenthesis &&
             tokens[4].kind == QQmlJS::Token::String &&
             tokens[5].kind == QQmlJS::Token::RightParenthesis )
        {
            if ( content.mid(tokens[0].begin(), tokens[0].length ) == "project" &&
                 content.mid(tokens[2].begin(), tokens[2].length ) == "path" )
            {
                result.setProperty("path", content.mid(tokens[4].begin() + 1, tokens[4].length - 2));
                result.setProperty("from", "project");
                return result;
            }
        }
    }

    return result;
}

QJSValue QmlSyntax::recurseObjectDeclarations(DocumentQmlValueObjects::RangeObject *object, const QString &content){
    if ( !object )
        return QJSValue();

    QQmlEngine* e = m_engine->engine();

    QJSValue base = e->newObject();
    base.setProperty("begin", object->begin);
    base.setProperty("end", object->end);
    base.setProperty("identifierEnd", object->identifierEnd);

    QJSValue baseProperties = e->newObject();
    for ( auto i = 0; i < object->properties.size(); ++i ){
        auto rp = object->properties[i];
        QJSValue baseProperty = e->newObject();
        QString basePropertyName = content.mid(rp->begin, rp->propertyEnd - rp->begin);
        baseProperty.setProperty("begin", rp->begin);
        baseProperty.setProperty("propertyEnd", rp->propertyEnd);
        baseProperty.setProperty("valueBegin", rp->valueBegin);
        baseProperty.setProperty("end", rp->end);
        baseProperty.setProperty("child", recurseObjectDeclarations(rp->child, content));
        baseProperties.setProperty(basePropertyName, baseProperty);
    }
    base.setProperty("properties", baseProperties);

    QJSValue baseChildren = e->newArray(object->children.size());
    for ( auto i = 0; i < object->children.size(); ++i ){
        baseChildren.setProperty(static_cast<quint32>(i), recurseObjectDeclarations(object->children[i], content));
    }
    base.setProperty("children", baseChildren);
    return base;

}

}// namespace
