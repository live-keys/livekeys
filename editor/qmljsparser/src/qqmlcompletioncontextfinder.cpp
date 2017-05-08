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

#include "qqmlcompletioncontextfinder.h"
#include "qmljs/qmljscompletioncontextfinder.h"

#include <qmljs/parser/qmljsast_p.h>
#include <qmljs/qmljsscanner.h>
#include <qmljs/qmljsdocument.h>

#include <QDebug>

// ExpressionUnderCursor
// ---------------------

using namespace QmlJS;
using namespace QmlJS::AST;

namespace {

class ExpressionUnderCursor
{
    QTextCursor _cursor;
    Scanner scanner;

public:
    ExpressionUnderCursor()
        : startOffset(-1), end(0)
    {}

    int startOffset, end;

    int startState(const QTextBlock &block) const
    {
        int state = block.previous().userState();
        if (state == -1)
            return 0;
        return state & 0xff;
    }

    QString operator()(const QTextCursor &cursor, QStringList* path)
    {
        return process(cursor, path);
    }

    int startOfExpression(const QList<Token> &tokens) const
    {
        return startOfExpression(tokens, tokens.size() - 1);
    }

    int startOfExpression(const QList<Token> &tokens, int index) const
    {
        if (index != -1) {
            const Token &tk = tokens.at(index);

            if (tk.is(Token::Identifier)) {
                if (index > 0 && tokens.at(index - 1).is(Token::Dot))
                    index = startOfExpression(tokens, index - 2);
            } else if (tk.is(Token::Dot)) {
                if (index > 0 && tokens.at(index - 1).is(Token::Identifier))
                    index = startOfExpression(tokens, index - 1);
            } else if (tk.is(Token::RightParenthesis)) {
                do { --index; }
                while (index != -1 && tokens.at(index).isNot(Token::LeftParenthesis));
                if (index > 0 && tokens.at(index - 1).is(Token::Identifier))
                    index = startOfExpression(tokens, index - 1);

            } else if (tk.is(Token::RightBracket)) {
                do { --index; }
                while (index != -1 && tokens.at(index).isNot(Token::LeftBracket));
                if (index > 0 && tokens.at(index - 1).is(Token::Identifier))
                    index = startOfExpression(tokens, index - 1);
            }
        }

        return index;
    }

    QString process(const QTextCursor &cursor, QStringList* path)
    {
        _cursor = cursor;

        QTextBlock block = _cursor.block();
        const QString blockText = block.text().left(cursor.positionInBlock());

        scanner.setScanComments(false);
        const QList<Token> tokens = scanner(blockText, startState(block));
        int start = startOfExpression(tokens);
        if (start == -1)
            return QString();

        startOffset = block.position() + tokens.at(start).begin();

        if ( path ){
            path->clear();
            for ( int i = start; i < tokens.size(); ++i ){
                if ( tokens.at(i).kind == Token::Identifier )
                    path->append(blockText.mid(tokens.at(i).begin(), tokens.at(i).length));
            }
            if ( tokens.size() > 0 ){
                if ( tokens.last().kind == Token::Dot )
                    path->append("");
                else if ( tokens.last().kind == Token::Identifier ){
                    if ( cursor.positionInBlock() > tokens.last().end() ){
                        path->clear();
                        startOffset = block.position() + cursor.positionInBlock();
                    }
//                    qDebug() << "CURSOR:" << cursor() << tokens.last().end();
                }
            }
        }

        const Token &tk = tokens.at(start);
        return blockText.mid(tk.begin(), tokens.last().end() - tk.begin());
    }
};

} // anonymous namespace


// CompletionContextFinder
// -----------------------

namespace lcv{

QQmlCompletionContextFinder::QQmlCompletionContextFinder(){

}

QQmlCompletionContextFinder::~QQmlCompletionContextFinder(){

}

QQmlCompletionContext *QQmlCompletionContextFinder::getContext(const QTextCursor &cursor){
    QmlJS::CompletionContextFinder finder(cursor);

    QStringList path;
    int context = 0;
    if ( finder.isInImport() )
        context |= QQmlCompletionContext::InImport;
    if ( finder.isInQmlContext() )
        context |= QQmlCompletionContext::InQml;
    if ( finder.isInLhsOfBinding() )
        context |= QQmlCompletionContext::InLhsOfBinding;
    if ( finder.isInRhsOfBinding() )
        context |= QQmlCompletionContext::InRhsofBinding;
    if ( finder.isAfterOnInLhsOfBinding() )
        context |= QQmlCompletionContext::InAfterOnLhsOfBinding;
    if ( finder.isInStringLiteral() )
        context |= QQmlCompletionContext::InStringLiteral;

    int propertyNamePosition = finder.propertyNamePosition();

    if ( !finder.isInStringLiteral() ){
        ExpressionUnderCursor euc;
        euc(cursor, &path);
        if ( finder.isInLhsOfBinding() )
            propertyNamePosition = euc.startOffset;
    }
    QStringList objectTypePath = finder.qmlObjectTypeName();

    if ( finder.isInImport() && !finder.libVersionImport().isEmpty() ){
        if ( path.isEmpty() ){
            context |= QQmlCompletionContext::InImportVersion;
            objectTypePath = finder.libVersionImport().split(".");
        }
    }

    return new QQmlCompletionContext(
        context,
        objectTypePath,
        finder.bindingPropertyName(),
        path,
        propertyNamePosition
    );
}

} // namespace
