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

#ifndef LVDOCUMENTQMLVALUEOBJECTS_H
#define LVDOCUMENTQMLVALUEOBJECTS_H

#include "live/lveditqmljsglobal.h"

#include <QList>
#include <QSharedPointer>

namespace QQmlJS{ namespace AST {
    class Node;
}}

namespace lv{

class DocumentQmlValueObjectsVisitor;

class LV_EDITQMLJS_EXPORT DocumentQmlValueObjects{

public:
    friend class DocumentQmlValueObjectsVisitor;

    /// Shared Pointer to a lv::DocumentQmlValueObjects
    typedef QSharedPointer<DocumentQmlValueObjects>       Ptr;
    /// Const Shared Pointer to a lv::DocumentQmlValueObjects
    typedef QSharedPointer<const DocumentQmlValueObjects> ConstPtr;

    class RangeObject;
    class RangeProperty;

    class RangeItem{
    public:
        enum RangeType{
            Property,
            Object
        };

    public:
        virtual ~RangeItem(){}

        virtual RangeType rangeType() const = 0;
        virtual QQmlJS::AST::Node* getAst() = 0;
        virtual RangeItem* getParent() = 0;
        virtual void appendObject(RangeObject* child) = 0;
        virtual void appendProperty(RangeProperty*){}
    };

    class LV_EDITQMLJS_EXPORT RangeProperty : public RangeItem{

    public:
        RangeProperty() : ast(nullptr), parent(nullptr), child(nullptr), begin(0), propertyEnd(0), valueBegin(0), end(0){}
        ~RangeProperty(){}

        void appendObject(RangeObject* object){ child = object; }
        RangeType rangeType() const{ return RangeItem::Property; }
        QQmlJS::AST::Node* getAst(){ return ast; }
        RangeItem* getParent(){ return parent; }
        QStringList name() const;
        QStringList object() const;
        QString type() const;
        bool hasType() const;

    public:
        QQmlJS::AST::Node* ast;

        RangeItem*   parent;
        RangeObject* child;

        int begin;
        int propertyEnd;
        int valueBegin;
        int end;
    };

    class LV_EDITQMLJS_EXPORT RangeObject : public RangeItem{

    public:
        RangeObject() : ast(nullptr), parent(nullptr), begin(0), end(0), identifierEnd(0){}
        ~RangeObject(){
            for ( int i = 0; i < children.size(); ++i )
                delete children[i];
        }

        RangeType rangeType() const{ return RangeItem::Object; }
        void appendObject(RangeObject *child) { children.append(child); }
        void appendProperty(RangeProperty *property){ properties.append(property); }
        QQmlJS::AST::Node* getAst(){ return ast; }
        RangeItem* getParent(){ return parent; }
        QStringList name() const;

        bool isComponent() const{ QStringList n = name(); return n.length() == 1 && ( n[0] == "Component" || n[0] == "ComponentSource"); }

    public:
        QQmlJS::AST::Node* ast;

        RangeItem* parent;
        QList<RangeObject*>   children;
        QList<RangeProperty*> properties;

        int begin;
        int end;
        int identifierEnd;
    };

public:
    ~DocumentQmlValueObjects();

    static DocumentQmlValueObjects::Ptr create();

    RangeObject* root();

    QString toString() const;

    void visit(QQmlJS::AST::Node* astroot);

    QList<RangeProperty*> propertiesBetween(int start, int end, RangeObject* root = nullptr);
    QList<RangeObject*>   objectsBetween(int start, int end, RangeObject* root = nullptr);
    RangeObject* objectAtPosition(int position, RangeObject* root = nullptr);
    RangeObject* objectThatWrapsPosition(int position, RangeObject* root = nullptr);

private:
    DocumentQmlValueObjects();

    QString toStringRecursive(RangeObject* object, int indent = 0) const;
    QString toStringRecursive(RangeProperty* property, int indent = 0) const;
    DocumentQmlValueObjects::RangeObject* appendRange(QQmlJS::AST::Node* ast, int begin, int end);

    RangeObject* m_root;
};

/// \brief Returns the root node of this ierarchy
inline DocumentQmlValueObjects::RangeObject *DocumentQmlValueObjects::root(){
    return m_root;
}

}// namespace

#endif // LVDOCUMENTQMLVALUEOBJECTS_H
