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

#ifndef LVDOCUMENTQMLRANGES_H
#define LVDOCUMENTQMLRANGES_H

#include "qmljs/qmljsdocument.h"
#include "qmljs/parser/qmljsastvisitor_p.h"
#include "qmljs/parser/qmljsast_p.h"

namespace lv{

/// \private
class DocumentQmlRanges: protected QmlJS::AST::Visitor{

public:
    /// \private
    class Range{
    public:
        Range(): ast(0) {}

    public: // attributes
        QmlJS::AST::Node *ast;
        int begin;
        int end;
    };

public:
    DocumentQmlRanges();

    QList<Range> operator()(QmlJS::AST::Node* ast);
    QList<Range> operator()(QmlJS::Document::Ptr doc);

    Range findClosestRange(int position) const;
    QList<Range> findRangePath(int position) const;

protected:
    using QmlJS::AST::Visitor::visit;

    virtual bool visit(QmlJS::AST::UiObjectBinding *ast);
    virtual bool visit(QmlJS::AST::UiObjectDefinition *ast);

    virtual bool visit(QmlJS::AST::FunctionExpression *ast);
    virtual bool visit(QmlJS::AST::FunctionDeclaration *ast);

    virtual bool visit(QmlJS::AST::BinaryExpression *ast);

    virtual bool visit(QmlJS::AST::UiScriptBinding *ast);

    Range createRange(QmlJS::AST::UiObjectMember *member, QmlJS::AST::UiObjectInitializer *ast);
    Range createRange(QmlJS::AST::FunctionExpression *ast);
    Range createRange(QmlJS::AST::UiScriptBinding *ast, QmlJS::AST::Block *block);
    Range createRange(QmlJS::AST::Node *ast, QmlJS::AST::SourceLocation start, QmlJS::AST::SourceLocation end);

private:
    QList<Range> m_ranges;
};

}// namespace

#endif // LVDOCUMENTQMLRANGES_H
