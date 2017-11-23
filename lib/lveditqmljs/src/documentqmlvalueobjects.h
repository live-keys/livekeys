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

#ifndef LVDOCUMENTQMLVALUEOBJECTS_H
#define LVDOCUMENTQMLVALUEOBJECTS_H

#include "live/lveditqmljsglobal.h"

#include <QList>
#include <QSharedPointer>

namespace QmlJS{ namespace AST {
    class Node;
}}

namespace lv{

class DocumentQmlValueObjectsVisitor;

class LV_EDITQMLJS_EXPORT DocumentQmlValueObjects{

public:
    friend class DocumentQmlValueObjectsVisitor;

    typedef QSharedPointer<DocumentQmlValueObjects>       Ptr;
    typedef QSharedPointer<const DocumentQmlValueObjects> ConstPtr;

    class RangeObject;
    class RangeProperty;

    class RangeItem{
    public:
        virtual ~RangeItem(){}

        virtual QmlJS::AST::Node* getAst() = 0;
        virtual RangeItem* getParent() = 0;
        virtual void appendObject(RangeObject* child) = 0;
        virtual void appendProperty(RangeProperty*){}

    };

    class RangeProperty : public RangeItem{

    public:
        RangeProperty() : ast(0), parent(0), child(0), begin(0), propertyEnd(0), valueBegin(0), end(0){}
        ~RangeProperty(){}

        void appendObject(RangeObject* object){ child = object; }
        QmlJS::AST::Node* getAst(){ return ast; }
        RangeItem* getParent(){ return parent; }
        QStringList name() const;
        QStringList object() const;
        QString type() const;
        bool hasType() const;

    public:
        QmlJS::AST::Node* ast;

        RangeItem* parent;
        RangeObject* child;

        int begin;
        int propertyEnd;
        int valueBegin;
        int end;
    };

    class RangeObject : public RangeItem{

    public:
        RangeObject() : ast(0), parent(0), begin(0), end(0){}
        ~RangeObject(){
            for ( int i = 0; i < children.size(); ++i )
                delete children[i];
        }

        void appendObject(RangeObject *child) { children.append(child); }
        void appendProperty(RangeProperty *property){ properties.append(property); }
        QmlJS::AST::Node* getAst(){ return ast; }
        RangeItem* getParent(){ return parent; }

    public:
        QmlJS::AST::Node* ast;

        RangeItem* parent;
        QList<RangeObject*>   children;
        QList<RangeProperty*> properties;

        int begin;
        int end;
    };

public:
    ~DocumentQmlValueObjects();

    static DocumentQmlValueObjects::Ptr create();

    RangeObject* root();

    QString toString() const;

    void visit(QmlJS::AST::Node* astroot);

    QList<RangeProperty*> propertiesBetween(int start, int end, RangeObject* root = 0);

private:
    DocumentQmlValueObjects();

    QString toStringRecursive(RangeObject* object, int indent = 0) const;
    QString toStringRecursive(RangeProperty* property, int indent = 0) const;
    DocumentQmlValueObjects::RangeObject* appendRange(QmlJS::AST::Node* ast, int begin, int end);

    RangeObject* m_root;
};

inline DocumentQmlValueObjects::RangeObject *DocumentQmlValueObjects::root(){
    return m_root;
}

}// namespace

#endif // LVDOCUMENTQMLVALUEOBJECTS_H
