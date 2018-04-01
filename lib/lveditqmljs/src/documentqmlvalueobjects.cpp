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

#include "live/documentqmlvalueobjects.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/parser/qmljsastvisitor_p.h"
#include "qmljs/parser/qmljsast_p.h"

namespace lv{

class DocumentQmlValueObjectsVisitor : protected QmlJS::AST::Visitor{

public:
    DocumentQmlValueObjectsVisitor(DocumentQmlValueObjects *parent);
    ~DocumentQmlValueObjectsVisitor();

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
    DocumentQmlValueObjects*            m_parent;
    DocumentQmlValueObjects::RangeItem* m_lastAppend;
};

DocumentQmlValueObjectsVisitor::DocumentQmlValueObjectsVisitor(DocumentQmlValueObjects* parent)
    : m_parent(parent)
{
}

DocumentQmlValueObjectsVisitor::~DocumentQmlValueObjectsVisitor(){
}

void DocumentQmlValueObjectsVisitor::operator()(QmlJS::AST::Node* astroot){
    delete m_parent->m_root;
    m_parent->m_root = 0;
    m_lastAppend = 0;
    if ( astroot )
        astroot->accept(this);
}

bool DocumentQmlValueObjectsVisitor::visit(QmlJS::AST::UiObjectDefinition *ast){
    if (ast->initializer && ast->initializer->lbraceToken.length){
        DocumentQmlValueObjects::RangeObject* obj = new DocumentQmlValueObjects::RangeObject;
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

bool DocumentQmlValueObjectsVisitor::visit(QmlJS::AST::UiScriptBinding *ast){
    DocumentQmlValueObjects::RangeProperty* property = new DocumentQmlValueObjects::RangeProperty;
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

bool DocumentQmlValueObjectsVisitor::visit(QmlJS::AST::UiObjectBinding *ast){
    DocumentQmlValueObjects::RangeProperty* property = 0;
    if ( m_lastAppend && m_lastAppend->getAst()->kind == QmlJS::AST::Node::Kind_UiPublicMember ){
        property = static_cast<DocumentQmlValueObjects::RangeProperty*>(m_lastAppend);
    } else {
        property = new DocumentQmlValueObjects::RangeProperty;
        property->ast         = ast;
        property->parent      = m_lastAppend;
        property->begin       = ast->firstSourceLocation().begin();
        property->propertyEnd = ast->firstSourceLocation().end();
    }
    if ( ast->initializer )
        property->valueBegin = ast->initializer->firstSourceLocation().begin();
    property->end = ast->lastSourceLocation().end();
    m_lastAppend->appendProperty(property);

    DocumentQmlValueObjects::RangeObject* obj = new DocumentQmlValueObjects::RangeObject;
    obj->ast   = ast;
    obj->begin = ast->initializer->firstSourceLocation().begin();
    obj->end   = ast->initializer->lastSourceLocation().end();

    property->child = obj;
    obj->parent     = property;

    m_lastAppend = obj;

    return true;
}

bool DocumentQmlValueObjectsVisitor::visit(QmlJS::AST::UiPublicMember *ast){
    DocumentQmlValueObjects::RangeProperty* property = new DocumentQmlValueObjects::RangeProperty;
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

void DocumentQmlValueObjectsVisitor::endVisit(QmlJS::AST::UiObjectDefinition *ast){
    if ( m_lastAppend->getAst() == ast )
        m_lastAppend = m_lastAppend->getParent();
}

void DocumentQmlValueObjectsVisitor::endVisit(QmlJS::AST::UiScriptBinding *ast){
    if ( m_lastAppend->getAst() == ast ){
        m_lastAppend = m_lastAppend->getParent();
    }
}

void DocumentQmlValueObjectsVisitor::endVisit(QmlJS::AST::UiObjectBinding *ast){
    if ( m_lastAppend->getAst() == ast ){
        m_lastAppend = m_lastAppend->getParent()->getParent();
    }
}

void DocumentQmlValueObjectsVisitor::endVisit(QmlJS::AST::UiPublicMember *ast){
    if ( m_lastAppend->getAst() == ast ){
        m_lastAppend = m_lastAppend->getParent();
    }
}

// Class QDocumentQmlValueObjects
// ------------------------------

DocumentQmlValueObjects::DocumentQmlValueObjects()
    : m_root(0)
{
}

DocumentQmlValueObjects::~DocumentQmlValueObjects(){
    delete m_root;
}

DocumentQmlValueObjects::Ptr DocumentQmlValueObjects::create(){
    return DocumentQmlValueObjects::Ptr(new DocumentQmlValueObjects);
}

QString DocumentQmlValueObjects::toString() const{
    if ( !m_root )
        return QString();

    return toStringRecursive(m_root);
}

void DocumentQmlValueObjects::visit(QmlJS::AST::Node *astroot){
    DocumentQmlValueObjectsVisitor visitor(this);
    visitor(astroot);
}

QList<DocumentQmlValueObjects::RangeProperty*> DocumentQmlValueObjects::propertiesBetween(
        int start,
        int end,
        RangeObject *root)
{
    QList<DocumentQmlValueObjects::RangeProperty*> properties;
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

QString DocumentQmlValueObjects::toStringRecursive(
        DocumentQmlValueObjects::RangeObject *object,
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

QString DocumentQmlValueObjects::toStringRecursive(
        DocumentQmlValueObjects::RangeProperty *property,
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

QStringList DocumentQmlValueObjects::RangeProperty::name() const{
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

QStringList DocumentQmlValueObjects::RangeProperty::object() const{
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

QString DocumentQmlValueObjects::RangeProperty::type() const{
    if ( ast->kind == QmlJS::AST::Node::Kind_UiPublicMember ){
        return static_cast<QmlJS::AST::UiPublicMember*>(ast)->memberType.toString();
    }
    return "";
}

bool DocumentQmlValueObjects::RangeProperty::hasType() const{
    return ast->kind == QmlJS::AST::Node::Kind_UiPublicMember;
}

}// namespace
