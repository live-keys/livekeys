#ifndef QDOCUMENTQMLRANGES_H
#define QDOCUMENTQMLRANGES_H

#include "qmljs/qmljsdocument.h"
#include "qmljs/parser/qmljsastvisitor_p.h"
#include "qmljs/parser/qmljsast_p.h"

namespace lcv{

class QDocumentQmlRanges: protected QmlJS::AST::Visitor{

public:
    class Range{
    public:
        Range(): ast(0) {}

    public: // attributes
        QmlJS::AST::Node *ast;
        int begin;
        int end;
    };

public:
    QDocumentQmlRanges();

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

#endif // QDOCUMENTQMLRANGES_H
