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

#include "qdocumentqmlvalueobjects.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/parser/qmljsastvisitor_p.h"
#include "qmljs/parser/qmljsast_p.h"

namespace lcv{

class QDocumentQmlValueObjectsVisitor : protected QmlJS::AST::Visitor{

public:
    QDocumentQmlValueObjectsVisitor(QDocumentQmlValueObjects *parent);
    ~QDocumentQmlValueObjectsVisitor();

    void operator()(QmlJS::AST::Node* astroot);

protected:
    using QmlJS::AST::Visitor::visit;

    virtual bool visit(QmlJS::AST::UiObjectDefinition* ast);
    virtual bool visit(QmlJS::AST::UiScriptBinding *ast);
    virtual bool visit(QmlJS::AST::UiObjectBinding* ast);
    virtual bool visit(QmlJS::AST::UiPublicMember* ast);

    virtual void endVisit(QmlJS::AST::UiObjectDefinition* ast);
    virtual void endVisit(QmlJS::AST::UiScriptBinding *ast);
    virtual void endVisit(QmlJS::AST::UiObjectBinding* ast);
    virtual void endVisit(QmlJS::AST::UiPublicMember* ast);

private:
    QDocumentQmlValueObjects*            m_parent;
    QDocumentQmlValueObjects::RangeItem* m_lastAppend;
};

QDocumentQmlValueObjectsVisitor::QDocumentQmlValueObjectsVisitor(QDocumentQmlValueObjects* parent)
    : m_parent(parent)
{
}

QDocumentQmlValueObjectsVisitor::~QDocumentQmlValueObjectsVisitor(){
}

void QDocumentQmlValueObjectsVisitor::operator()(QmlJS::AST::Node* astroot){
    delete m_parent->m_root;
    m_parent->m_root = 0;
    m_lastAppend = 0;
    if ( astroot )
        astroot->accept(this);
}

bool QDocumentQmlValueObjectsVisitor::visit(QmlJS::AST::UiObjectDefinition *ast){
    if (ast->initializer && ast->initializer->lbraceToken.length){
        QDocumentQmlValueObjects::RangeObject* obj = new QDocumentQmlValueObjects::RangeObject;
        obj->ast   = ast;
        obj->begin = ast->firstSourceLocation().begin();
        obj->end   = ast->initializer->rbraceToken.end();
        if ( !m_parent->m_root ){
            m_parent->m_root = obj;
            m_lastAppend = obj;
        } else {
            m_lastAppend->appendObject(obj);
            obj->parent = m_lastAppend;
        }
        m_lastAppend = obj;
    }
    return true;
}

bool QDocumentQmlValueObjectsVisitor::visit(QmlJS::AST::UiScriptBinding *ast){
    QDocumentQmlValueObjects::RangeProperty* property = new QDocumentQmlValueObjects::RangeProperty;
    property->ast         = ast;
    property->parent      = m_lastAppend;
    property->begin       = ast->firstSourceLocation().begin();
    property->propertyEnd = ast->firstSourceLocation().end();
    if ( ast->statement )
        property->valueBegin = ast->statement->firstSourceLocation().begin();
    property->end        = ast->lastSourceLocation().end();
    m_lastAppend->appendProperty(property);

    if ( QmlJS::AST::cast<QmlJS::AST::Block *>(ast->statement)){
        m_lastAppend = property;
    }

    return true;
}

bool QDocumentQmlValueObjectsVisitor::visit(QmlJS::AST::UiObjectBinding *ast){
    QDocumentQmlValueObjects::RangeProperty* property = 0;
    if ( m_lastAppend && m_lastAppend->getAst()->kind == QmlJS::AST::Node::Kind_UiPublicMember ){
        property = static_cast<QDocumentQmlValueObjects::RangeProperty*>(m_lastAppend);
    } else {
        property = new QDocumentQmlValueObjects::RangeProperty;
        property->ast         = ast;
        property->parent      = m_lastAppend;
        property->begin       = ast->firstSourceLocation().begin();
        property->propertyEnd = ast->firstSourceLocation().end();
    }
    if ( ast->initializer )
        property->valueBegin = ast->initializer->firstSourceLocation().begin();
    property->end = ast->lastSourceLocation().end();
    m_lastAppend->appendProperty(property);

    QDocumentQmlValueObjects::RangeObject* obj = new QDocumentQmlValueObjects::RangeObject;
    obj->ast   = ast;
    obj->begin = ast->initializer->firstSourceLocation().begin();
    obj->end   = ast->initializer->lastSourceLocation().end();

    property->child = obj;
    obj->parent     = property;

    m_lastAppend = obj;

    return true;
}

bool QDocumentQmlValueObjectsVisitor::visit(QmlJS::AST::UiPublicMember *ast){
    QDocumentQmlValueObjects::RangeProperty* property = new QDocumentQmlValueObjects::RangeProperty;
    property->ast         = ast;
    property->parent      = m_lastAppend;
    property->begin       = ast->identifierToken.begin();
    property->propertyEnd = ast->identifierToken.end();
    if (ast->statement)
        property->valueBegin = ast->statement->firstSourceLocation().begin();
    property->end      = ast->lastSourceLocation().end();
    m_lastAppend->appendProperty(property);

    m_lastAppend = property;

    return true;
}

void QDocumentQmlValueObjectsVisitor::endVisit(QmlJS::AST::UiObjectDefinition *ast){
    if ( m_lastAppend->getAst() == ast )
        m_lastAppend = m_lastAppend->getParent();
}

void QDocumentQmlValueObjectsVisitor::endVisit(QmlJS::AST::UiScriptBinding *ast){
    if ( m_lastAppend->getAst() == ast ){
        m_lastAppend = m_lastAppend->getParent();
    }
}

void QDocumentQmlValueObjectsVisitor::endVisit(QmlJS::AST::UiObjectBinding *ast){
    if ( m_lastAppend->getAst() == ast ){
        m_lastAppend = m_lastAppend->getParent()->getParent();
    }
}

void QDocumentQmlValueObjectsVisitor::endVisit(QmlJS::AST::UiPublicMember *ast){
    if ( m_lastAppend->getAst() == ast ){
        m_lastAppend = m_lastAppend->getParent();
    }
}

// Class QDocumentQmlValueObjects
// ------------------------------

QDocumentQmlValueObjects::QDocumentQmlValueObjects()
    : m_root(0)
{
}

QDocumentQmlValueObjects::~QDocumentQmlValueObjects(){
    delete m_root;
}

QDocumentQmlValueObjects::Ptr QDocumentQmlValueObjects::create(){
    return QDocumentQmlValueObjects::Ptr(new QDocumentQmlValueObjects);
}

QString QDocumentQmlValueObjects::toString() const{
    if ( !m_root )
        return QString();

    return toStringRecursive(m_root);
}

void QDocumentQmlValueObjects::visit(QmlJS::AST::Node *astroot){
    QDocumentQmlValueObjectsVisitor visitor(this);
    visitor(astroot);
}

QList<QDocumentQmlValueObjects::RangeProperty*> QDocumentQmlValueObjects::propertiesBetween(
        int start,
        int end,
        RangeObject *root)
{
    QList<QDocumentQmlValueObjects::RangeProperty*> properties;
    if ( root == 0 )
        root = m_root;

    for ( int i = 0; i < root->properties.size(); ++i ){
        if ( root->properties[i]->begin >= start && root->properties[i]->propertyEnd <= end ){
            properties.append(root->properties[i]);
        } else if (
                root->properties[i]->child != 0 &&
                ( root->properties[i]->child->begin < end  || root->properties[i]->child->end > start )
            )
        {
            properties.append(propertiesBetween(start, end, root->properties[i]->child));
        }
    }

    for ( int i = 0; i < root->children.size(); ++i ){
        if ( root->children[i]->begin < end || root->children[i]->end > start ){
            properties.append(propertiesBetween(start, end, root->children[i]));
        }
    }

    return properties;
}

QString QDocumentQmlValueObjects::toStringRecursive(
        QDocumentQmlValueObjects::RangeObject *object,
        int indent) const
{
    QString indentStr;
    for ( int i = 0; i < indent; ++i )
        indentStr += "  ";

    QString base;
    base += indentStr + "<Object:" + QString::number(object->begin) + "," + QString::number(object->end) + ">\n";

    for ( int i = 0; i < object->properties.size(); ++i ){
        base += toStringRecursive(object->properties[i], indent + 1);
    }

    for ( int i = 0; i < object->children.size(); ++i ){
        base += toStringRecursive(object->children[i], indent + 1);
    }

    return base;
}

QString QDocumentQmlValueObjects::toStringRecursive(
        QDocumentQmlValueObjects::RangeProperty *property,
        int indent) const
{
    QString indentStr;
    for ( int i = 0; i < indent; ++i )
        indentStr += "  ";

    QString base = indentStr + "<Property:" + QString::number(property->begin) + "," + QString::number(property->end) + ">\n";

    if ( property->child != 0 )
        base += toStringRecursive(property->child, indent + 1);

    return base;
}

QStringList QDocumentQmlValueObjects::RangeProperty::name() const{
    QStringList base;

    if ( ast->kind == QmlJS::AST::Node::Kind_UiScriptBinding ){
        QmlJS::AST::UiQualifiedId* qi = static_cast<QmlJS::AST::UiScriptBinding*>(ast)->qualifiedId;
        while ( qi != 0 ){
            base.append(qi->name.toString());
            qi = qi->next;
        }
    } else if ( ast->kind == QmlJS::AST::Node::Kind_UiObjectBinding ){
        QmlJS::AST::UiQualifiedId* qi = static_cast<QmlJS::AST::UiObjectBinding*>(ast)->qualifiedId;
        while ( qi != 0 ){
            base.append(qi->name.toString());
            qi = qi->next;
        }
    } else if ( ast->kind == QmlJS::AST::Node::Kind_UiPublicMember ){
        base.append(static_cast<QmlJS::AST::UiPublicMember*>(ast)->name.toString());
    }


    return base;
}

QStringList QDocumentQmlValueObjects::RangeProperty::object() const{
    QStringList base;

    if ( !parent )
        return base;

    if ( parent->getAst()->kind == QmlJS::AST::Node::Kind_UiObjectBinding ){
        QmlJS::AST::UiQualifiedId* qi = static_cast<QmlJS::AST::UiObjectBinding*>(parent->getAst())->qualifiedTypeNameId;
        while ( qi != 0 ){
            base.append(qi->name.toString());
            qi = qi->next;
        }
    } else if ( parent->getAst()->kind == QmlJS::AST::Node::Kind_UiObjectDefinition ){
        QmlJS::AST::UiQualifiedId* qi = static_cast<QmlJS::AST::UiObjectBinding*>(parent->getAst())->qualifiedId;
        while ( qi != 0 ){
            base.append(qi->name.toString());
            qi = qi->next;
        }
    }

    return base;
}

QString QDocumentQmlValueObjects::RangeProperty::type() const{
    if ( ast->kind == QmlJS::AST::Node::Kind_UiPublicMember ){
        return static_cast<QmlJS::AST::UiPublicMember*>(ast)->memberType.toString();
    }
    return "";
}

bool QDocumentQmlValueObjects::RangeProperty::hasType() const{
    return ast->kind == QmlJS::AST::Node::Kind_UiPublicMember;
}

}// namespace
