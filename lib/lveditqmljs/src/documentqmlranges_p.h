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

#ifndef LVDOCUMENTQMLRANGES_H
#define LVDOCUMENTQMLRANGES_H

#include "qmljs/qmljsdocument.h"
#include "qmljs/parser/qqmljsastvisitor_p.h"
#include "qmljs/parser/qqmljsast_p.h"

namespace lv{

/// \private
class DocumentQmlRanges: protected QQmlJS::AST::Visitor{

public:
    /// \private
    class Range{
    public:
        Range(): ast(0) {}

    public: // attributes
        QQmlJS::AST::Node *ast;
        int begin;
        int end;
    };

public:
    DocumentQmlRanges();

    QList<Range> operator()(QQmlJS::AST::Node* ast);
    QList<Range> operator()(QQmlJS::Document::Ptr doc);

    Range findClosestRange(int position) const;
    QList<Range> findRangePath(int position) const;

protected:
    using QQmlJS::AST::Visitor::visit;

    virtual bool visit(QQmlJS::AST::UiObjectBinding *ast) override;
    virtual bool visit(QQmlJS::AST::UiObjectDefinition *ast) override;

    virtual bool visit(QQmlJS::AST::FunctionExpression *ast) override;
    virtual bool visit(QQmlJS::AST::FunctionDeclaration *ast) override;

    virtual bool visit(QQmlJS::AST::BinaryExpression *ast) override;

    virtual bool visit(QQmlJS::AST::UiScriptBinding *ast) override;

    void throwRecursionDepthError() override;

    Range createRange(QQmlJS::AST::UiObjectMember *member, QQmlJS::AST::UiObjectInitializer *ast);
    Range createRange(QQmlJS::AST::FunctionExpression *ast);
    Range createRange(QQmlJS::AST::UiScriptBinding *ast, QQmlJS::AST::Block *block);
    Range createRange(QQmlJS::AST::Node *ast, QQmlJS::AST::SourceLocation start, QQmlJS::AST::SourceLocation end);

private:
    QList<Range> m_ranges;
};

}// namespace

#endif // LVDOCUMENTQMLRANGES_H
