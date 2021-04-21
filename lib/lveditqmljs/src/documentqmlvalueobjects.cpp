/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "live/documentqmlvalueobjects.h"

#include "qmljs/qmljsdocument.h"
#include "qmljs/parser/qqmljsastvisitor_p.h"
#include "qmljs/parser/qqmljsast_p.h"

namespace lv{

// DocumentQmlValueObjectsVisitor
// -----------------------------------------------------------------------------

/// \private
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
    m_parent->m_root = nullptr;
    m_lastAppend = nullptr;
    if ( astroot )
        astroot->accept(this);
}

bool DocumentQmlValueObjectsVisitor::visit(QmlJS::AST::UiObjectDefinition *ast){
    if (ast->initializer && ast->initializer->lbraceToken.length){
        DocumentQmlValueObjects::RangeObject* obj = new DocumentQmlValueObjects::RangeObject;
        obj->ast   = ast;
        obj->begin = static_cast<int>(ast->firstSourceLocation().begin());
        obj->identifierEnd = static_cast<int>(ast->qualifiedTypeNameId->lastSourceLocation().end());
        obj->end   = static_cast<int>(ast->initializer->rbraceToken.end());
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
    property->begin       = static_cast<int>(ast->firstSourceLocation().begin());
    property->propertyEnd = static_cast<int>(ast->firstSourceLocation().end());
    if ( ast->statement )
        property->valueBegin = static_cast<int>(ast->statement->firstSourceLocation().begin());
    property->end        = static_cast<int>(ast->lastSourceLocation().end());
    m_lastAppend->appendProperty(property);

    if ( QmlJS::AST::cast<QmlJS::AST::Block *>(ast->statement)){
        m_lastAppend = property;
    }

    return true;
}

bool DocumentQmlValueObjectsVisitor::visit(QmlJS::AST::UiObjectBinding *ast){
    DocumentQmlValueObjects::RangeProperty* property = nullptr;
    if ( m_lastAppend && m_lastAppend->getAst()->kind == QmlJS::AST::Node::Kind_UiPublicMember ){
        property = static_cast<DocumentQmlValueObjects::RangeProperty*>(m_lastAppend);
    } else {
        property = new DocumentQmlValueObjects::RangeProperty;
        property->ast         = ast;
        property->parent      = m_lastAppend;
        property->begin       = ast->firstSourceLocation().begin();
        property->propertyEnd = ast->firstSourceLocation().end();
    }
    if ( ast->qualifiedTypeNameId )
        property->valueBegin = ast->qualifiedTypeNameId->firstSourceLocation().begin();
    property->end = ast->lastSourceLocation().end();
    m_lastAppend->appendProperty(property);

    DocumentQmlValueObjects::RangeObject* obj = new DocumentQmlValueObjects::RangeObject;
    obj->ast   = ast;
    obj->begin = ast->qualifiedTypeNameId->firstSourceLocation().begin();
    obj->identifierEnd = ast->qualifiedTypeNameId->lastSourceLocation().end();
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

// Class QDocumentQmlValueObjects::RangeItem
// -----------------------------------------------------------------------------

/**
 * \class lv::DocumentQmlValueObjects::RangeItem
 * \ingroup lveditqmljs
 * \brief Base range item acting as a node in the hierarchy
 */

/**
 * \fn lv::DocumentQmlValueObjects::RangeItem::getAst()
 * \brief Returns internal AST node.
 */

/**
 * \fn lv::DocumentQmlValueObjects::RangeItem::getParent()
 * \brief Returns the parent of this item, or nullptr if there isn't any
 */

/**
 * \fn lv::DocumentQmlValueObjects::RangeItem::appendObject()
 * \brief Appends an object to this item
 */

/**
 * \fn lv::DocumentQmlValueObjects::RangeItem::appendProperty()
 * \brief Appends a property to this item
 */

// Class QDocumentQmlValueObjects::RangeProperty
// -----------------------------------------------------------------------------

/**
 * \class lv::DocumentQmlValueObjects::RangeProperty
 * \ingroup lveditqmljs
 * \brief Property range item inside the ierarchy
 */


/**
 * \fn lv::DocumentQmlValueObjects::RangeProperty::getAst()
 * \brief Returns internal AST node.
 */

/**
 * \fn lv::DocumentQmlValueObjects::RangeProperty::getParent()
 * \brief Returns the parent of this item, or nullptr if there isn't any
 */

/**
 * \fn lv::DocumentQmlValueObjects::RangeProperty::appendObject()
 * \brief Sets the value of this property to the specified object
 */

/**
 * \fn lv::DocumentQmlValueObjects::RangeProperty::name()
 * \brief Returns the name chain of this property
 */


/**
 * \fn lv::DocumentQmlValueObjects::RangeProperty::object()
 * \brief Returns the declared object chain this property belongs to
 */


/**
 * \fn lv::DocumentQmlValueObjects::RangeProperty::type()
 * \brief Returns the declared type of this property if it has any
 */


/**
 * \fn lv::DocumentQmlValueObjects::RangeProperty::hasType()
 * \brief Returns true if this property's type was declared within this document,
 * false otherwise
 */


/**
 * \property lv::DocumentQmlValueObjects::RangeProperty::ast
 * \brief Internal AST node
 */


/**
 * \property lv::DocumentQmlValueObjects::RangeProperty::parent
 * \brief Parent of this item
 */


/**
 * \property lv::DocumentQmlValueObjects::RangeProperty::child
 * \brief Child object of this property if it has any
 */

/**
 * \property lv::DocumentQmlValueObjects::RangeProperty::begin
 * \brief Start range of this property
 */

/**
 * \property lv::DocumentQmlValueObjects::RangeProperty::propertyEnd
 * \brief Returns the end of this property's identifier declaration
 */

/**
 * \property lv::DocumentQmlValueObjects::RangeProperty::valueBegin
 * \brief Returns the starting position of this property's value declaration
 */

/**
 * \property lv::DocumentQmlValueObjects::RangeProperty::end
 * \brief Returns the end of this property
 */

// Class QDocumentQmlValueObjects::RangeObject
// -----------------------------------------------------------------------------

/**
 * \class lv::DocumentQmlValueObjects::RangeObject
 * \ingroup lveditqmljs
 * \brief Object range item inside the hierarchy
 */


/**
 * \fn lv::DocumentQmlValueObjects::RangeObject::getAst()
 * \brief Returns internal AST node.
 */

/**
 * \fn lv::DocumentQmlValueObjects::RangeObject::getParent()
 * \brief Returns the parent of this object, or nullptr if there isn't any
 */

/**
 * \fn lv::DocumentQmlValueObjects::RangeObject::appendObject()
 * \brief Appends a child object to this object
 */

/**
 * \fn lv::DocumentQmlValueObjects::RangeObject::appendProperty()
 * \brief Appends a property to this object
 */

/**
 * \property lv::DocumentQmlValueObjects::RangeObject::ast
 * \brief Internal AST node
 */

/**
 * \property lv::DocumentQmlValueObjects::RangeObject::parent
 * \brief Parent of this object
 */


/**
 * \property lv::DocumentQmlValueObjects::RangeObject::children
 * \brief Children for this object
 */

/**
 * \property lv::DocumentQmlValueObjects::RangeObject::properties
 * \brief Properties for this obejct
 */

/**
 * \property lv::DocumentQmlValueObjects::RangeObject::begin
 * \brief Start position for this object
 */

/**
 * \property lv::DocumentQmlValueObjects::RangeObject::end
 * \brief End position for this object
 */

// Class QDocumentQmlValueObjects
// -----------------------------------------------------------------------------

/**
 * \class DocumentQmlValueObjects
 * \ingroup lveditqmljs
 * \brief Contains the ierarchy of Qml objects and a reference to their AST
 *
 * This class can be used to extract parsed objects and their properties using their range and position
 * in code.
 */

/**
 * \brief DocumentQmlValueObjects constructor
 */
DocumentQmlValueObjects::DocumentQmlValueObjects()
    : m_root(nullptr)
{
}


/**
 * \brief DocumentQmlValueObjects destructor
 */
DocumentQmlValueObjects::~DocumentQmlValueObjects(){
    delete m_root;
}

/**
 * \brief Creates a new lv::DocumentQmlValueObjects object
 * \returns Shared Pointer to a lv::DocumentQmlValueObjects object
 */
DocumentQmlValueObjects::Ptr DocumentQmlValueObjects::create(){
    return DocumentQmlValueObjects::Ptr(new DocumentQmlValueObjects);
}

/**
 * \brief String representation of this object
 *
 * Will output the hierarchy of nodes defined in this object
 */
QString DocumentQmlValueObjects::toString() const{
    if ( !m_root )
        return QString();

    return toStringRecursive(m_root);
}

/**
 * \brief Visitor method implementation
 */
void DocumentQmlValueObjects::visit(QmlJS::AST::Node *astroot){
    DocumentQmlValueObjectsVisitor visitor(this);
    visitor(astroot);
}

/**
 * \brief Returns the properties between \p start and \p end for the given \p root object
 */
QList<DocumentQmlValueObjects::RangeProperty*> DocumentQmlValueObjects::propertiesBetween(
        int start,
        int end,
        RangeObject *root)
{
    QList<DocumentQmlValueObjects::RangeProperty*> properties;
    if ( root == nullptr )
        root = m_root;

    for ( int i = 0; i < root->properties.size(); ++i ){
        if ( root->properties[i]->begin >= start && root->properties[i]->propertyEnd <= end ){
            properties.append(root->properties[i]);
        } else if (
                root->properties[i]->child != nullptr &&
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

QList<DocumentQmlValueObjects::RangeObject *> DocumentQmlValueObjects::objectsBetween(int start, int end, DocumentQmlValueObjects::RangeObject *root){
    QList<DocumentQmlValueObjects::RangeObject*> objects;
    if ( root == nullptr )
        root = m_root;

    for ( int i = 0; i < root->children.size(); ++i ){
        if ( root->children[i]->begin >= start && root->children[i]->end <= end ){
            objects.append(root->children[i]);
        }
        if ( root->children[i]->begin < end || root->children[i]->end > start ){
            objects.append(objectsBetween(start, end, root->children[i]));
        }
    }

    return objects;
}

DocumentQmlValueObjects::RangeObject *DocumentQmlValueObjects::objectAtPosition(int position, RangeObject* root){
    if ( root == nullptr )
        root = m_root;

    if ( root->begin == position )
        return root;


    for ( int i = 0; i < root->children.size(); ++i ){
        if ( root->children[i]->begin <= position && root->children[i]->end >= position ){
            DocumentQmlValueObjects::RangeObject* ro = objectAtPosition(position, root->children[i]);
            if (ro)
                return ro;
        }
    }

    return nullptr;
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

    if ( property->child != nullptr )
        base += toStringRecursive(property->child, indent + 1);

    return base;
}

/**
 * \brief Returns the name chain of this property
 *
 * For example
 * \code
 * border.size : 20 // will return border.size
 * \endcode
 */
QStringList DocumentQmlValueObjects::RangeProperty::name() const{
    QStringList base;

    if ( ast->kind == QmlJS::AST::Node::Kind_UiScriptBinding ){
        QmlJS::AST::UiQualifiedId* qi = static_cast<QmlJS::AST::UiScriptBinding*>(ast)->qualifiedId;
        while ( qi != nullptr ){
            base.append(qi->name.toString());
            qi = qi->next;
        }
    } else if ( ast->kind == QmlJS::AST::Node::Kind_UiObjectBinding ){
        QmlJS::AST::UiQualifiedId* qi = static_cast<QmlJS::AST::UiObjectBinding*>(ast)->qualifiedId;
        while ( qi != nullptr ){
            base.append(qi->name.toString());
            qi = qi->next;
        }
    } else if ( ast->kind == QmlJS::AST::Node::Kind_UiPublicMember ){
        base.append(static_cast<QmlJS::AST::UiPublicMember*>(ast)->name.toString());
    }


    return base;
}

/**
 * \brief Returns the object type chain for this property
 */
QStringList DocumentQmlValueObjects::RangeProperty::object() const{
    QStringList base;

    if ( !parent )
        return base;

    if ( parent->getAst()->kind == QmlJS::AST::Node::Kind_UiObjectBinding ){
        QmlJS::AST::UiQualifiedId* qi = static_cast<QmlJS::AST::UiObjectBinding*>(parent->getAst())->qualifiedTypeNameId;
        while ( qi != nullptr ){
            base.append(qi->name.toString());
            qi = qi->next;
        }
    } else if ( parent->getAst()->kind == QmlJS::AST::Node::Kind_UiObjectDefinition ){
        QmlJS::AST::UiQualifiedId* qi = static_cast<QmlJS::AST::UiObjectBinding*>(parent->getAst())->qualifiedId;
        while ( qi != nullptr ){
            base.append(qi->name.toString());
            qi = qi->next;
        }
    }

    return base;
}

/**
 * \brief Returns the type of this property
 */
QString DocumentQmlValueObjects::RangeProperty::type() const{
    if ( ast->kind == QmlJS::AST::Node::Kind_UiPublicMember ){
        return static_cast<QmlJS::AST::UiPublicMember*>(ast)->memberType->name.toString();
    }
    return "";
}

/**
 * \brief Checks wether this property was declared with a type
 *
 * \returns True on success, false otherwise
 */
bool DocumentQmlValueObjects::RangeProperty::hasType() const{
    return ast->kind == QmlJS::AST::Node::Kind_UiPublicMember;
}

}// namespace
