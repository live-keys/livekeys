#ifndef QDOCUMENTQMLRANGES_H
#define QDOCUMENTQMLRANGES_H

#include <QTextCursor>
#include <QTextDocument>
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
        QTextCursor begin;
        QTextCursor end;
    };
public:
    QTextDocument *_textDocument;
    QList<Range> _ranges;

public:
    QDocumentQmlRanges(QTextDocument* textDocument){
        _textDocument = textDocument;
    }

    QList<Range> operator()(QmlJS::AST::Node* ast){
        ast->accept(this);
        return _ranges;
    }

    QList<Range> operator()(QTextDocument *textDocument, QmlJS::Document::Ptr doc)
    {
        _textDocument = textDocument;
        _ranges.clear();
        if (doc && doc->ast() != 0)
            doc->ast()->accept(this);
        return _ranges;
    }

protected:
    using QmlJS::AST::Visitor::visit;

    virtual bool visit(QmlJS::AST::UiObjectBinding *ast){
        if (ast->initializer && ast->initializer->lbraceToken.length)
            _ranges.append(createRange(ast, ast->initializer));
        return true;
    }

    virtual bool visit(QmlJS::AST::UiObjectDefinition *ast){
        if (ast->initializer && ast->initializer->lbraceToken.length)
            _ranges.append(createRange(ast, ast->initializer));
        return true;
    }

    virtual bool visit(QmlJS::AST::FunctionExpression *ast){
        _ranges.append(createRange(ast));
        return true;
    }

    virtual bool visit(QmlJS::AST::FunctionDeclaration *ast){
        _ranges.append(createRange(ast));
        return true;
    }

    bool visit(QmlJS::AST::BinaryExpression *ast){
        auto field = QmlJS::AST::cast<QmlJS::AST::FieldMemberExpression *>(ast->left);
        auto funcExpr = QmlJS::AST::cast<QmlJS::AST::FunctionExpression *>(ast->right);

        if (field && funcExpr && funcExpr->body && (ast->op == QSOperator::Assign))
            _ranges.append(createRange(ast, ast->firstSourceLocation(), ast->lastSourceLocation()));
        return true;
    }

    virtual bool visit(QmlJS::AST::UiScriptBinding *ast){
        if (QmlJS::AST::Block *block = QmlJS::AST::cast<QmlJS::AST::Block *>(ast->statement))
            _ranges.append(createRange(ast, block));
        return true;
    }

    Range createRange(QmlJS::AST::UiObjectMember *member, QmlJS::AST::UiObjectInitializer *ast){
//        qDebug() << "Create range for uiobject member."
//         << member->firstSourceLocation().startLine << ast->rbraceToken.startLine;
        return createRange(member, member->firstSourceLocation(), ast->rbraceToken);
    }

    Range createRange(QmlJS::AST::FunctionExpression *ast){
        return createRange(ast, ast->lbraceToken, ast->rbraceToken);
    }

    Range createRange(QmlJS::AST::UiScriptBinding *ast, QmlJS::AST::Block *block){
        return createRange(ast, block->lbraceToken, block->rbraceToken);
    }

    Range createRange(QmlJS::AST::Node *ast, QmlJS::AST::SourceLocation start, QmlJS::AST::SourceLocation end){
        Range range;

        range.ast = ast;

        range.begin = QTextCursor(_textDocument);
        range.begin.setPosition(start.begin());

        range.end = QTextCursor(_textDocument);
        range.end.setPosition(end.end());

        return range;
    }
};

}// namespace

#endif // QDOCUMENTQMLRANGES_H
