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

#include "documentqmlranges_p.h"

namespace lv{

DocumentQmlRanges::DocumentQmlRanges(){
}

QList<DocumentQmlRanges::Range> DocumentQmlRanges::operator()(QmlJS::Document::Ptr doc){
    m_ranges.clear();
    if (doc && doc->ast() != 0)
        doc->ast()->accept(this);
    return m_ranges;
}

bool DocumentQmlRanges::visit(QmlJS::AST::UiObjectBinding *ast){
    if (ast->initializer && ast->initializer->lbraceToken.length)
        m_ranges.append(createRange(
            ast, ast->initializer->firstSourceLocation(), ast->initializer->lastSourceLocation()
        ));
    return true;
}

bool DocumentQmlRanges::visit(QmlJS::AST::UiObjectDefinition *ast){
    if (ast->initializer && ast->initializer->lbraceToken.length)
        m_ranges.append(createRange(ast, ast->initializer));
    return true;
}

bool DocumentQmlRanges::visit(QmlJS::AST::FunctionExpression *ast){
    m_ranges.append(createRange(ast));
    return true;
}

bool DocumentQmlRanges::visit(QmlJS::AST::FunctionDeclaration *ast){
    m_ranges.append(createRange(ast));
    return true;
}

bool DocumentQmlRanges::visit(QmlJS::AST::BinaryExpression *ast){
    auto field = QmlJS::AST::cast<QmlJS::AST::FieldMemberExpression *>(ast->left);
    auto funcExpr = QmlJS::AST::cast<QmlJS::AST::FunctionExpression *>(ast->right);

    if (field && funcExpr && funcExpr->body && (ast->op == QSOperator::Assign))
        m_ranges.append(createRange(ast, ast->firstSourceLocation(), ast->lastSourceLocation()));
    return true;
}

bool DocumentQmlRanges::visit(QmlJS::AST::UiScriptBinding *ast){
    if (QmlJS::AST::Block *block = QmlJS::AST::cast<QmlJS::AST::Block *>(ast->statement))
        m_ranges.append(createRange(ast, block));
    return true;
}

DocumentQmlRanges::Range DocumentQmlRanges::createRange(
        QmlJS::AST::UiObjectMember *member,
        QmlJS::AST::UiObjectInitializer *ast)
{
    //        qDebug() << "Create range for uiobject member."
    //         << member->firstSourceLocation().startLine << ast->rbraceToken.startLine;
    return createRange(member, member->firstSourceLocation(), ast->rbraceToken);
}

DocumentQmlRanges::Range DocumentQmlRanges::createRange(QmlJS::AST::FunctionExpression *ast){
    return createRange(ast, ast->lbraceToken, ast->rbraceToken);
}

DocumentQmlRanges::Range DocumentQmlRanges::createRange(
        QmlJS::AST::UiScriptBinding *ast,
        QmlJS::AST::Block *block)
{
    return createRange(ast, block->lbraceToken, block->rbraceToken);
}

DocumentQmlRanges::Range DocumentQmlRanges::createRange(
        QmlJS::AST::Node *ast,
        QmlJS::AST::SourceLocation start,
        QmlJS::AST::SourceLocation end)
{
    Range range;

    range.ast = ast;
    range.begin = start.begin();
    range.end   = end.end();

    return range;
}

DocumentQmlRanges::Range DocumentQmlRanges::findClosestRange(int position) const{
    DocumentQmlRanges::Range base;
    foreach( Range r, m_ranges ){
        if ( r.begin <= position && r.end > position )
            base = r;
    }
    return base;
}

QList<DocumentQmlRanges::Range> DocumentQmlRanges::findRangePath(int position) const{
    QList<DocumentQmlRanges::Range> base;
    foreach( Range r, m_ranges ){
        if ( r.begin <= position && r.end > position ){
            base.append(r);
        }
    }
    return base;
}

} // namespace
