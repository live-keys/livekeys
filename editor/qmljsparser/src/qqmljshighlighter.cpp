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

#include "qqmljshighlighter_p.h"
#include "qcoderuntimebinding.h"
#include <QDebug>

namespace lcv{

QQmlJsHighlighter::QQmlJsHighlighter(QTextDocument *parent, lcv::QDocumentHandlerState *state)
    : QSyntaxHighlighter(parent)
    , m_documentState(state)
{
    m_formatRoles["text"]        = QQmlJsHighlighter::Text;
    m_formatRoles["comment"]     = QQmlJsHighlighter::Comment;
    m_formatRoles["number"]      = QQmlJsHighlighter::Number;
    m_formatRoles["string"]      = QQmlJsHighlighter::String;
    m_formatRoles["operator"]    = QQmlJsHighlighter::Operator;
    m_formatRoles["identifier"]  = QQmlJsHighlighter::Identifier;
    m_formatRoles["keyword"]     = QQmlJsHighlighter::Keyword;
    m_formatRoles["builtin"]     = QQmlJsHighlighter::BuiltIn;
    m_formatRoles["qmlproperty"] = QQmlJsHighlighter::QmlProperty;
    m_formatRoles["qmltype"]     = QQmlJsHighlighter::QmlType;
    m_formatRoles["qmlruntimeboundproperty"] = QQmlJsHighlighter::QmlRuntimeBoundProperty;
    m_formatRoles["qmlruntimemodifiedvalue"] = QQmlJsHighlighter::QmlRuntimeModifiedValue;
    m_formatRoles["qmledit"] = QQmlJsHighlighter::QmlEdit;

    m_formats[QQmlJsHighlighter::Text]        = createFormat("#fff");
    m_formats[QQmlJsHighlighter::Comment]     = createFormat("#56748a");
    m_formats[QQmlJsHighlighter::Number]      = createFormat("#bc900c");
    m_formats[QQmlJsHighlighter::String]      = createFormat("#809747");
    m_formats[QQmlJsHighlighter::Operator]    = createFormat("#c0a000");
    m_formats[QQmlJsHighlighter::Identifier]  = createFormat("#93672f");
    m_formats[QQmlJsHighlighter::Keyword]     = createFormat("#a0a000");
    m_formats[QQmlJsHighlighter::BuiltIn]     = createFormat("#93672f");

    m_formats[QQmlJsHighlighter::QmlProperty] = createFormat("#ccc");
    m_formats[QQmlJsHighlighter::QmlType]     = createFormat("#0080a0");
    m_formats[QQmlJsHighlighter::QmlRuntimeBoundProperty] = createFormat("#26539f");
    m_formats[QQmlJsHighlighter::QmlRuntimeModifiedValue] = createFormat("#0080a0");
    m_formats[QQmlJsHighlighter::QmlEdit] = createFormat("#fff", "#0b273f");


    // built-in and other popular objects + properties
    m_knownIds << "Object";
    m_knownIds << "prototype";
    m_knownIds << "property";
    m_knownIds << "__parent__";
    m_knownIds << "__proto__";
    m_knownIds << "__defineGetter__";
    m_knownIds << "__defineSetter__";
    m_knownIds << "__lookupGetter__";
    m_knownIds << "__lookupSetter__";
    m_knownIds << "__noSuchMethod__";
    m_knownIds << "Function";
    m_knownIds << "String";
    m_knownIds << "Array";
    m_knownIds << "RegExp";
    m_knownIds << "global";
    m_knownIds << "NaN";
    m_knownIds << "undefined";
    m_knownIds << "Math";
    m_knownIds << "import";
    m_knownIds << "string";
    m_knownIds << "int";
    m_knownIds << "variant";
    m_knownIds << "signal";
}

// Walks through ab.bc.cd to find whether it preceedes a colon or an opening brace
// depending on the case, this can be an qml type or a qml property
// Anything else is discarded
QQmlJsHighlighter::LookAheadType QQmlJsHighlighter::lookAhead(
        const QString& text,
        const QList<QmlJS::Token> &tokens,
        QList<QmlJS::Token>::ConstIterator it,
        int state)
{
    bool identifierExpected = false;
    while ( it != tokens.end() ){
        const QmlJS::Token& tk = *it;
        if ( tk.is(QmlJS::Token::Colon) ){
            return QQmlJsHighlighter::Property;
        } else if ( tk.is(QmlJS::Token::LeftBrace) ){
            return QQmlJsHighlighter::Type;
        } else if ( tk.is(QmlJS::Token::Identifier ) ){
            if ( !identifierExpected ){
                if ( tk.length == 2 && text.mid(tk.begin(), tk.length) == "on" )
                     return QQmlJsHighlighter::Type;

                return QQmlJsHighlighter::Unknown;
            }
            identifierExpected = false;
        } else if ( tk.is(QmlJS::Token::Dot) ){
            identifierExpected = true;
        } else if ( tk.isNot(QmlJS::Token::Comment) ){
            return QQmlJsHighlighter::Unknown;
        }
        ++it;
    }

    QTextBlock bl = currentBlock().next();
    while ( bl.isValid() ){
        QmlJS::Scanner scn;
        QList<QmlJS::Token> tks = scn(bl.text(), state);
        state = scn.state();
        it = tks.begin();

        while ( it != tks.end() ){
            const QmlJS::Token& tk = *it;
            if ( tk.is(QmlJS::Token::Colon) ){
                return QQmlJsHighlighter::Property;
            } else if ( tk.is(QmlJS::Token::LeftBrace) ){
                return QQmlJsHighlighter::Type;
            } else if ( tk.is(QmlJS::Token::Identifier ) ){
                if ( !identifierExpected ){
                    if ( tk.length == 2 && text.mid(tk.begin(), tk.length) == "on" )
                         return QQmlJsHighlighter::Type;

                    return QQmlJsHighlighter::Unknown;
                }
                identifierExpected = false;
            } else if ( tk.is(QmlJS::Token::Dot) ){
                identifierExpected = true;
            } else if ( tk.isNot(QmlJS::Token::Comment) ){
                return QQmlJsHighlighter::Unknown;
            }
            ++it;
        }

        bl = bl.next();
    }

    return QQmlJsHighlighter::Unknown;
}

void QQmlJsHighlighter::highlightBlock(const QString &text){
    QList<int> bracketPositions;
    int blockState   = previousBlockState();

    bool prevGenerated   = (blockState >> 4) & 1;
    int bracketLevel = blockState >> 5;
    int state        = blockState & 15;


    if (blockState < 0) {
        prevGenerated = false;
        bracketLevel = 0;
        state = QmlJS::Scanner::Normal;
    }

    QmlJS::Scanner scanner;
    QList<QmlJS::Token> tokens = scanner(text, state);
    state = scanner.state();

    QList<QmlJS::Token>::iterator it = tokens.begin();
    while ( it != tokens.end() ){
        QmlJS::Token& tk = *it;
        switch(tk.kind){
        case QmlJS::Token::Keyword:
            setFormat(tk.begin(), tk.length, m_formats[QQmlJsHighlighter::Keyword]);
            break;
        case QmlJS::Token::Identifier:{
            QString tktext = text.mid(tk.begin(), tk.length);
            if ( m_knownIds.contains(tktext) ){
                setFormat(tk.begin(), tk.length, m_formats[QQmlJsHighlighter::Identifier]);
            } else if ( tktext == "true" || tktext == "false" ){
                setFormat(tk.begin(), tk.length, m_formats[QQmlJsHighlighter::Keyword]);
            } else {
                QList<QmlJS::Token>::iterator lait = it;
                QQmlJsHighlighter::LookAheadType la = lookAhead(text, tokens, ++lait, state);
                if ( la == QQmlJsHighlighter::Property )
                    setFormat(tk.begin(), tk.length, m_formats[QQmlJsHighlighter::QmlProperty]);
                else if ( la == QQmlJsHighlighter::Type )
                    setFormat(tk.begin(), tk.length, m_formats[QQmlJsHighlighter::QmlType]);
            }
            break;
        }
        case QmlJS::Token::String:
            setFormat(tk.begin(), tk.length, m_formats[QQmlJsHighlighter::String]);
            break;
        case QmlJS::Token::Comment:
            setFormat(tk.begin(), tk.length, m_formats[QQmlJsHighlighter::Comment]);
            break;
        case QmlJS::Token::Number:
            setFormat(tk.begin(), tk.length, m_formats[QQmlJsHighlighter::Number]);
            break;
        case QmlJS::Token::LeftParenthesis:
        case QmlJS::Token::RightParenthesis:
        case QmlJS::Token::LeftBrace:
        case QmlJS::Token::RightBrace:
        case QmlJS::Token::LeftBracket:
        case QmlJS::Token::RightBracket:
        case QmlJS::Token::EndOfFile:
            break;
        case QmlJS::Token::Semicolon:
        case QmlJS::Token::Colon:
        case QmlJS::Token::Comma:
        case QmlJS::Token::Dot:
        case QmlJS::Token::Delimiter:
            setFormat(tk.begin(), tk.length, m_formats[QQmlJsHighlighter::Operator]);
            break;
        case QmlJS::Token::RegExp:
            setFormat(tk.begin(), tk.length, m_formats[QQmlJsHighlighter::String]);
            break;
        }

        ++it;
    }

    lcv::QProjectDocumentBlockData *blockData =
            reinterpret_cast<lcv::QProjectDocumentBlockData*>(currentBlock().userData());

    if (!bracketPositions.isEmpty()) {
        if (!blockData) {
            blockData = new lcv::QProjectDocumentBlockData;
            currentBlock().setUserData(blockData);
        }
        blockData->bracketPositions = bracketPositions;
    }

    bool generated = false;

    if ( prevGenerated ){
        QTextBlock prevBlock = currentBlock().previous();
        if ( prevBlock.isValid() && prevBlock.userData() ){
            lcv::QProjectDocumentBlockData *prevBlockData =
                    reinterpret_cast<lcv::QProjectDocumentBlockData*>(prevBlock.userData());

            if ( prevBlockData->exceededBindingLength > 0 ){
                int currentExceededLength = prevBlockData->exceededBindingLength - currentBlock().length();
                if ( currentExceededLength > 0 ){
                    setFormat(0, currentBlock().length(), m_formats[QQmlJsHighlighter::QmlEdit]);

                    if (!blockData) {
                        blockData = new lcv::QProjectDocumentBlockData;
                        currentBlock().setUserData(blockData);
                    }
                    blockData->exceededBindingLength = currentExceededLength;
                    generated = true;
                } else {
                    setFormat(0, prevBlockData->exceededBindingLength, m_formats[QQmlJsHighlighter::QmlEdit]);
                }
            }
        }
    }


    if ( blockData ){
        foreach(lcv::QCodeRuntimeBinding* bind, blockData->m_bindings ){
            setFormat(
                bind->position() - currentBlock().position(),
                bind->declaration()->identifierLength(),
                m_formats[QQmlJsHighlighter::QmlRuntimeBoundProperty]
            );

            if ( bind->isModifiedByEngine() ){
                int valueFrom = bind->position() + bind->declaration()->identifierLength() + bind->declaration()->valueOffset();
                setFormat(
                    valueFrom - currentBlock().position(),
                    bind->declaration()->valueLength(),
                    m_formats[QQmlJsHighlighter::QmlRuntimeModifiedValue]
                );
                if ( valueFrom + bind->declaration()->valueLength() > currentBlock().position() + currentBlock().length() ){
                    generated = true;
                    blockData->exceededBindingLength =
                        bind->declaration()->valueLength() - (currentBlock().length() - (valueFrom - currentBlock().position()));
                }
            }
        }
    }

    blockState = (state & 15) | (generated << 4) | (bracketLevel << 5);
    setCurrentBlockState(blockState);

    if ( m_documentState && m_documentState->editingFragment() ){
        int position = m_documentState->editingFragment()->valuePosition();
        int length   = m_documentState->editingFragment()->valueLength();
        if ( position + length >= currentBlock().position() &&
             position < currentBlock().position() + currentBlock().length())
        {
            int from = position - currentBlock().position();
            setFormat(from < 0 ? 0 : from, length, m_formats[QQmlJsHighlighter::QmlEdit]);
        }
    }
}

}// namespace

