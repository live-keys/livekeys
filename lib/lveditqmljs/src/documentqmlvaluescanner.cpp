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

#include "documentqmlvaluescanner_p.h"
#include <QDebug>

namespace lv{

DocumentQmlValueScanner::DocumentQmlValueScanner(ProjectDocument *document, int position, int length)
    : m_position(position)
    , m_length(length)
    , m_colonPosition(-1)
    , m_valuePosition(-1)
    , m_valueEnd(-1)
    , m_document(document)
{

}

DocumentQmlValueScanner::~DocumentQmlValueScanner(){
}

bool DocumentQmlValueScanner::operator()(){

    int startFrom = m_position + m_length;
    if (!m_document->textDocument()) return false;

    QTextBlock block = m_document->textDocument()->findBlock(startFrom);
    int blockTrim    = startFrom - block.position();

    while ( block.isValid() ){
        m_colonPosition = findColonInTokenSet(m_scanner(block.text().mid(blockTrim) ) );
        if ( m_colonPosition == -2 )
            return false;
        else if ( m_colonPosition == -1 ){
            block = block.next();
            blockTrim = 0;
        } else {
            m_colonPosition += blockTrim + block.position();
            break;
        }
    }
    if ( m_colonPosition == -1 )
        return false;


    blockTrim = m_colonPosition + 1 - block.position();

    QList<QQmlJS::Token> tokens = m_scanner(block.text().mid(blockTrim));

    int nestingDepth        = 0;
    bool identifierExpected = true;

    m_valueEnd        = 0;
    int blockPosition = block.position() + blockTrim;

    while(true){
        for ( QList<QQmlJS::Token>::iterator it = tokens.begin(); it != tokens.end(); ++it ){
            if ( m_valuePosition == -1 )
                m_valuePosition = blockPosition + it->offset;

            if ( it->is(QQmlJS::Token::LeftBrace) ||
                 it->is(QQmlJS::Token::LeftParenthesis) ||
                 it->is(QQmlJS::Token::LeftBracket ) )
            {
                identifierExpected = true;
                ++nestingDepth;
            } else if ( it->is(QQmlJS::Token::RightBrace ) ||
                        it->is(QQmlJS::Token::RightBracket) ||
                        it->is(QQmlJS::Token::RightParenthesis) )
            {
                --nestingDepth;
                if ( nestingDepth < 0 ){
                    return true;
                }
                identifierExpected = false;
                m_valueEnd = blockPosition + it->offset + it->length;

            // There's no point iterating tokens inside brackets
            } else if ( nestingDepth == 0 ){

                if ( it->is(QQmlJS::Token::Identifier) || it->is(QQmlJS::Token::Keyword) ){
                    if ( !identifierExpected )
                        return true;

                    if ( it->is(QQmlJS::Token::Identifier) )
                        identifierExpected = false;
                    m_valueEnd = blockPosition + it->offset + it->length;

                } else if ( it->is(QQmlJS::Token::RegExp) ||
                            it->is(QQmlJS::Token::String) ||
                            it->is(QQmlJS::Token::Number) )
                {
                    identifierExpected = false;
                    m_valueEnd = blockPosition + it->offset + it->length;
                } else if ( it->is(QQmlJS::Token::Semicolon) ){
                    return true;

                } else if ( !it->is(QQmlJS::Token::Comment) ){
                    identifierExpected = true;
                }
            }
        }

        block = block.next();
        if ( !block.isValid() ){
            return false;
        }
        blockPosition = block.position();
        tokens        = m_scanner(block.text(), m_scanner.state());
    }

}

int DocumentQmlValueScanner::getExpressionExtent(
        QTextDocument *document,
        int position,
        QStringList *expressionPath,
        QChar* endDelimiter)
{
    QString lastWord;
    if ( expressionPath )
        lastWord = expressionPath->size() > 0 ? expressionPath->takeLast() : "";

    QTextBlock block = document->findBlock(position);

    int propertyLength = 0;

    bool wasSpace = false;

    int positionInBlock = position - block.position();
    while( block.isValid() ){
        QString blockText = block.text().mid(positionInBlock);
        for ( QString::iterator blockCh = blockText.begin(); blockCh != blockText.end(); ++blockCh ){
            if ( *blockCh == QChar('.') ){
                if ( expressionPath ){
                    expressionPath->append(lastWord);
                    lastWord = "";
                }
                wasSpace = false;
            } else if ( blockCh->isLetter() || *blockCh == QChar::fromLatin1('_') ){
                if ( expressionPath ){
                    lastWord += *blockCh;
                }
                wasSpace = false;
            } else if ( blockCh->isNumber() ){
                if ( wasSpace )
                    return 0;
                if ( expressionPath )
                    lastWord += *blockCh;
                wasSpace = false;
            } else if ( blockCh->isSpace( ) ){
                wasSpace = true;
            } else {
                if ( expressionPath )
                    expressionPath->append(lastWord);
                if ( endDelimiter )
                    *endDelimiter = *blockCh;
                return propertyLength;
            }
            ++propertyLength;
        }

        block           = block.next();
        wasSpace        = true;
        positionInBlock = 0;
    }

    return propertyLength;
}

int DocumentQmlValueScanner::getPropertyExpressionExtent(
        QTextDocument *document,
        int position,
        QStringList *expressionPath,
        QChar *endDelimiter,
        int* newPosition)
{
    QString lastWord;
    if ( expressionPath )
        lastWord = expressionPath->size() > 0 ? expressionPath->takeLast() : "";

    QTextBlock block = document->findBlock(position);

    int propertyEnd = position;

    bool wasSpace = false;
    bool wasDot = false;

    if (newPosition )
        *newPosition = -1;

    int positionInBlock = position - block.position();
    while( block.isValid() ){
        QString blockText = block.text().mid(positionInBlock);
        for ( int i = 0; i < blockText.length(); ++i ){
            propertyEnd = block.position() + positionInBlock + i;
            QChar blockCh = blockText[i];
            if ( blockCh == QChar('.') ){
                if ( expressionPath ){
                    expressionPath->append(lastWord);
                    lastWord = "";
                }
                wasSpace = false;
                wasDot = true;
            } else if ( blockCh.isLetter() || blockCh == QChar::fromLatin1('_') ){
                if ( wasSpace && !wasDot ){ // reset
                    if ( newPosition )
                        *newPosition = block.position() + positionInBlock + i;
                    if ( expressionPath ){
                        expressionPath->clear();
                        lastWord = "";
                    }
                }

                if ( expressionPath ){
                    // reset position if no expression up to this point was found
                    if ( expressionPath->isEmpty() && lastWord.isEmpty() ){
                        if ( newPosition )
                            *newPosition = block.position() + positionInBlock + i;
                    }

                    lastWord += blockCh;
                }
                wasSpace = false;
                wasDot = false;
            } else if ( blockCh.isNumber() ){
                if ( wasSpace )
                    return 0;
                if ( expressionPath )
                    lastWord += blockCh;
                wasSpace = false;
                wasDot = false;
            } else if ( blockCh.isSpace( ) ){
                wasSpace = true;
            } else {
                if ( expressionPath )
                    expressionPath->append(lastWord);
                if ( endDelimiter )
                    *endDelimiter = blockCh;
                return propertyEnd;
            }
        }

        block           = block.next();
        wasSpace        = true;
        positionInBlock = 0;
    }

    return propertyEnd;
}

int DocumentQmlValueScanner::getBlockStart(int position){
    QTextBlock block = m_document->textDocument()->findBlock(position);
    if ( !block.isValid() )
        return 0;

    int state = 0;
    int nestingDepth = 0;
    while(true){
        int blockState = block.userState();
        if ( blockState < 0 ){
            state = 0;
        } else {
            state = blockState & 15;
        }

        QList<QQmlJS::Token> tokens = m_scanner(block.text(), state);
        for ( auto it = tokens.rbegin(); it != tokens.rend(); ++it ){
            int tokenPosition = block.position() + it->offset;

            if ( it->is(QQmlJS::Token::LeftBrace) ||
                 it->is(QQmlJS::Token::LeftParenthesis) ||
                 it->is(QQmlJS::Token::LeftBracket ) )
            {
                if ( tokenPosition < position )
                    --nestingDepth;
                if ( nestingDepth == -1 ){
                    return tokenPosition;
                }
            } else if ( it->is(QQmlJS::Token::RightBrace ) ||
                        it->is(QQmlJS::Token::RightBracket) ||
                        it->is(QQmlJS::Token::RightParenthesis) )
            {
                if ( tokenPosition < position )
                    ++nestingDepth;
            }
        }

        block = block.previous();
        if ( !block.isValid() ){
            return 0;
        }
    }

    return 0;
}

int DocumentQmlValueScanner::getBlockEnd(int position){
    QTextBlock block = m_document->textDocument()->findBlock(position);
    if ( !block.isValid() )
        return 0;

    int state = 0;
    int nestingDepth = 0;
    while(true){
        int blockState = block.userState();
        if ( blockState < 0 ){
            state = 0;
        } else {
            state = blockState & 15;
        }

        QList<QQmlJS::Token> tokens = m_scanner(block.text(), state);
        for ( auto it = tokens.begin(); it != tokens.end(); ++it ){
            int tokenPosition = block.position() + it->offset;

            if ( it->is(QQmlJS::Token::LeftBrace) ||
                 it->is(QQmlJS::Token::LeftParenthesis) ||
                 it->is(QQmlJS::Token::LeftBracket ) )
            {
                if ( tokenPosition > position )
                    ++nestingDepth;
            } else if ( it->is(QQmlJS::Token::RightBrace ) ||
                        it->is(QQmlJS::Token::RightBracket) ||
                        it->is(QQmlJS::Token::RightParenthesis) )
            {
                --nestingDepth;
                if ( nestingDepth < 0 ){
                    return tokenPosition;
                }
            }
        }

        block = block.next();
        if ( !block.isValid() ){
            return 0;
        }
    }

    return 0;
}

int DocumentQmlValueScanner::getBlockExtent(int from){
    QTextBlock block = m_document->textDocument()->findBlock(from);
    int blockTrim = from - block.position();

    QList<QQmlJS::Token> tokens = m_scanner(block.text().mid(blockTrim));

    int nestingDepth        = 0;
    bool identifierExpected = true;

    int valueEnd        = from;
    int blockPosition = block.position() + blockTrim;

    while(true){
        for ( QList<QQmlJS::Token>::iterator it = tokens.begin(); it != tokens.end(); ++it ){
            if ( it->is(QQmlJS::Token::LeftBrace) ||
                 it->is(QQmlJS::Token::LeftParenthesis) ||
                 it->is(QQmlJS::Token::LeftBracket ) )
            {
                identifierExpected = true;
                ++nestingDepth;
            } else if ( it->is(QQmlJS::Token::RightBrace ) ||
                        it->is(QQmlJS::Token::RightBracket) ||
                        it->is(QQmlJS::Token::RightParenthesis) )
            {
                --nestingDepth;
                if ( nestingDepth < 0 ){
                    return valueEnd - from;
                }
                identifierExpected = false;
                valueEnd = blockPosition + it->offset + it->length;

            // There's no point iterating tokens inside brackets
            } else if ( nestingDepth == 0 ){

                if ( it->is(QQmlJS::Token::Identifier) || it->is(QQmlJS::Token::Keyword) ){
                    if ( !identifierExpected )
                        return valueEnd - from;

                    if ( it->is(QQmlJS::Token::Identifier) )
                        identifierExpected = false;
                    valueEnd = blockPosition + it->offset + it->length;

                } else if ( it->is(QQmlJS::Token::RegExp) ||
                            it->is(QQmlJS::Token::String) ||
                            it->is(QQmlJS::Token::Number) )
                {
                    identifierExpected = false;
                    valueEnd = blockPosition + it->offset + it->length;
                } else if ( it->is(QQmlJS::Token::Semicolon) ){
                    return -1;

                } else if ( !it->is(QQmlJS::Token::Comment) ){
                    identifierExpected = true;
                }
            }
        }

        block = block.next();
        if ( !block.isValid() ){
            return -1;
        }
        blockPosition = block.position();
        tokens        = m_scanner(block.text(), m_scanner.state());
    }

    return -1;
}

int DocumentQmlValueScanner::getBlockEnd(QTextBlock &block, int position){
    if ( !block.isValid() )
        return 0;

    int state = 0;
    int nestingDepth = 0;

    QQmlJS::Scanner scanner;

    while(true){
        int blockState = block.userState();
        if ( blockState < 0 ){
            state = 0;
        } else {
            state = blockState & 15;
        }

        QList<QQmlJS::Token> tokens = scanner(block.text(), state);
        for ( auto it = tokens.begin(); it != tokens.end(); ++it ){
            int tokenPosition = block.position() + it->offset;

            if ( it->is(QQmlJS::Token::LeftBrace) ||
                 it->is(QQmlJS::Token::LeftParenthesis) ||
                 it->is(QQmlJS::Token::LeftBracket ) )
            {
                if ( tokenPosition > position )
                    ++nestingDepth;
            } else if ( it->is(QQmlJS::Token::RightBrace ) ||
                        it->is(QQmlJS::Token::RightBracket) ||
                        it->is(QQmlJS::Token::RightParenthesis) )
            {
                if ( tokenPosition > position ){
                    --nestingDepth;
                    if ( nestingDepth < 0 ){
                        return tokenPosition;
                    }
                }
            }
        }

        block = block.next();
        if ( !block.isValid() ){
            return 0;
        }
    }

    return 0;
}

int DocumentQmlValueScanner::findColonInTokenSet(const QList<QQmlJS::Token> &tokens){
    for ( QList<QQmlJS::Token>::ConstIterator it = tokens.begin(); it != tokens.end(); ++it ){
        if ( it->is(QQmlJS::Token::Colon) ){
            return it->offset;
        } else if ( it->is(QQmlJS::Token::Comma) ){
            return -1;
        } else {
            return -2;
        }
    }
    return -1;
}


}// namespace
