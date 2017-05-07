#include "qdocumentqmlranges_p.h"

namespace lcv{

QDocumentQmlRanges::QDocumentQmlRanges(){
}

QList<QDocumentQmlRanges::Range> QDocumentQmlRanges::operator()(QmlJS::Document::Ptr doc){
    m_ranges.clear();
    if (doc && doc->ast() != 0)
        doc->ast()->accept(this);
    return m_ranges;
}

bool QDocumentQmlRanges::visit(QmlJS::AST::UiObjectBinding *ast){
    if (ast->initializer && ast->initializer->lbraceToken.length)
        m_ranges.append(createRange(
            ast, ast->initializer->firstSourceLocation(), ast->initializer->lastSourceLocation()
        ));
    return true;
}

bool QDocumentQmlRanges::visit(QmlJS::AST::UiObjectDefinition *ast){
    if (ast->initializer && ast->initializer->lbraceToken.length)
        m_ranges.append(createRange(ast, ast->initializer));
    return true;
}

bool QDocumentQmlRanges::visit(QmlJS::AST::FunctionExpression *ast){
    m_ranges.append(createRange(ast));
    return true;
}

bool QDocumentQmlRanges::visit(QmlJS::AST::FunctionDeclaration *ast){
    m_ranges.append(createRange(ast));
    return true;
}

bool QDocumentQmlRanges::visit(QmlJS::AST::BinaryExpression *ast){
    auto field = QmlJS::AST::cast<QmlJS::AST::FieldMemberExpression *>(ast->left);
    auto funcExpr = QmlJS::AST::cast<QmlJS::AST::FunctionExpression *>(ast->right);

    if (field && funcExpr && funcExpr->body && (ast->op == QSOperator::Assign))
        m_ranges.append(createRange(ast, ast->firstSourceLocation(), ast->lastSourceLocation()));
    return true;
}

bool QDocumentQmlRanges::visit(QmlJS::AST::UiScriptBinding *ast){
    if (QmlJS::AST::Block *block = QmlJS::AST::cast<QmlJS::AST::Block *>(ast->statement))
        m_ranges.append(createRange(ast, block));
    return true;
}

QDocumentQmlRanges::Range QDocumentQmlRanges::createRange(
        QmlJS::AST::UiObjectMember *member,
        QmlJS::AST::UiObjectInitializer *ast)
{
    //        qDebug() << "Create range for uiobject member."
    //         << member->firstSourceLocation().startLine << ast->rbraceToken.startLine;
    return createRange(member, member->firstSourceLocation(), ast->rbraceToken);
}

QDocumentQmlRanges::Range QDocumentQmlRanges::createRange(QmlJS::AST::FunctionExpression *ast){
    return createRange(ast, ast->lbraceToken, ast->rbraceToken);
}

QDocumentQmlRanges::Range QDocumentQmlRanges::createRange(
        QmlJS::AST::UiScriptBinding *ast,
        QmlJS::AST::Block *block)
{
    return createRange(ast, block->lbraceToken, block->rbraceToken);
}

QDocumentQmlRanges::Range QDocumentQmlRanges::createRange(
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

QDocumentQmlRanges::Range QDocumentQmlRanges::findClosestRange(int position) const{
    QDocumentQmlRanges::Range base;
    foreach( Range r, m_ranges ){
        if ( r.begin <= position && r.end > position )
            base = r;
    }
    return base;
}

QList<QDocumentQmlRanges::Range> QDocumentQmlRanges::findRangePath(int position) const{
    QList<QDocumentQmlRanges::Range> base;
    foreach( Range r, m_ranges ){
        if ( r.begin <= position && r.end > position ){
            base.append(r);
        }
    }
    return base;
}

} // namespace
