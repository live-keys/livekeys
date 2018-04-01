/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "qmljshighlighter_p.h"
#include "live/coderuntimebinding.h"

namespace lv{

QmlJsHighlighter::QmlJsHighlighter(QmlJsSettings* settings, QTextDocument *parent, lv::DocumentHandlerState *state)
    : QSyntaxHighlighter(parent)
    , m_settings(settings)
    , m_documentState(state)
{
}

// Walks through ab.bc.cd to find whether it preceedes a colon or an opening brace
// depending on the case, this can be an qml type or a qml property
// Anything else is discarded
QmlJsHighlighter::LookAheadType QmlJsHighlighter::lookAhead(
        const QString& text,
        const QList<QmlJS::Token> &tokens,
        QList<QmlJS::Token>::ConstIterator it,
        int state)
{
    bool identifierExpected = false;
    while ( it != tokens.end() ){
        const QmlJS::Token& tk = *it;
        if ( tk.is(QmlJS::Token::Colon) ){
            return QmlJsHighlighter::Property;
        } else if ( tk.is(QmlJS::Token::LeftBrace) ){
            return QmlJsHighlighter::Type;
        } else if ( tk.is(QmlJS::Token::Identifier ) ){
            if ( !identifierExpected ){
                if ( tk.length == 2 && text.mid(tk.begin(), tk.length) == "on" )
                     return QmlJsHighlighter::Type;

                return QmlJsHighlighter::Unknown;
            }
            identifierExpected = false;
        } else if ( tk.is(QmlJS::Token::Dot) ){
            identifierExpected = true;
        } else if ( tk.isNot(QmlJS::Token::Comment) ){
            return QmlJsHighlighter::Unknown;
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
                return QmlJsHighlighter::Property;
            } else if ( tk.is(QmlJS::Token::LeftBrace) ){
                return QmlJsHighlighter::Type;
            } else if ( tk.is(QmlJS::Token::Identifier ) ){
                if ( !identifierExpected ){
                    if ( tk.length == 2 && text.mid(tk.begin(), tk.length) == "on" )
                         return QmlJsHighlighter::Type;

                    return QmlJsHighlighter::Unknown;
                }
                identifierExpected = false;
            } else if ( tk.is(QmlJS::Token::Dot) ){
                identifierExpected = true;
            } else if ( tk.isNot(QmlJS::Token::Comment) ){
                return QmlJsHighlighter::Unknown;
            }
            ++it;
        }

        bl = bl.next();
    }

    return QmlJsHighlighter::Unknown;
}

void QmlJsHighlighter::highlightBlock(const QString &text){
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

    QmlJsSettings& settings = *m_settings;

    QmlJS::Scanner scanner;
    QList<QmlJS::Token> tokens = scanner(text, state);
    state = scanner.state();

    QList<QmlJS::Token>::iterator it = tokens.begin();
    while ( it != tokens.end() ){
        QmlJS::Token& tk = *it;
        switch(tk.kind){
        case QmlJS::Token::Keyword:
            setFormat(tk.begin(), tk.length, settings[QmlJsSettings::Keyword]);
            break;
        case QmlJS::Token::Identifier:{
            QString tktext = text.mid(tk.begin(), tk.length);
            if ( m_knownIds.contains(tktext) ){
                setFormat(tk.begin(), tk.length, settings[QmlJsSettings::Identifier]);
            } else if ( tktext == "true" || tktext == "false" ){
                setFormat(tk.begin(), tk.length, settings[QmlJsSettings::Keyword]);
            } else {
                QList<QmlJS::Token>::iterator lait = it;
                QmlJsHighlighter::LookAheadType la = lookAhead(text, tokens, ++lait, state);
                if ( la == QmlJsHighlighter::Property )
                    setFormat(tk.begin(), tk.length, settings[QmlJsSettings::QmlProperty]);
                else if ( la == QmlJsHighlighter::Type )
                    setFormat(tk.begin(), tk.length, settings[QmlJsSettings::QmlType]);
            }
            break;
        }
        case QmlJS::Token::String:
            setFormat(tk.begin(), tk.length, settings[QmlJsSettings::String]);
            break;
        case QmlJS::Token::Comment:
            setFormat(tk.begin(), tk.length, settings[QmlJsSettings::Comment]);
            break;
        case QmlJS::Token::Number:
            setFormat(tk.begin(), tk.length, settings[QmlJsSettings::Number]);
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
            setFormat(tk.begin(), tk.length, settings[QmlJsSettings::Operator]);
            break;
        case QmlJS::Token::RegExp:
            setFormat(tk.begin(), tk.length, settings[QmlJsSettings::String]);
            break;
        }

        ++it;
    }

    lv::ProjectDocumentBlockData *blockData =
            reinterpret_cast<lv::ProjectDocumentBlockData*>(currentBlock().userData());

    if (!bracketPositions.isEmpty()) {
        if (!blockData) {
            blockData = new lv::ProjectDocumentBlockData;
            currentBlock().setUserData(blockData);
        }
        blockData->bracketPositions = bracketPositions;
    }

    bool generated = false;

    if ( prevGenerated ){
        QTextBlock prevBlock = currentBlock().previous();
        if ( prevBlock.isValid() && prevBlock.userData() ){
            lv::ProjectDocumentBlockData *prevBlockData =
                    reinterpret_cast<lv::ProjectDocumentBlockData*>(prevBlock.userData());

            if ( prevBlockData->exceededBindingLength > 0 ){
                int currentExceededLength = prevBlockData->exceededBindingLength - currentBlock().length();
                if ( currentExceededLength > 0 ){
                    setFormat(0, currentBlock().length(), settings[QmlJsSettings::QmlEdit]);

                    if (!blockData) {
                        blockData = new lv::ProjectDocumentBlockData;
                        currentBlock().setUserData(blockData);
                    }
                    blockData->exceededBindingLength = currentExceededLength;
                    generated = true;
                } else {
                    setFormat(0, prevBlockData->exceededBindingLength, settings[QmlJsSettings::QmlEdit]);
                }
            }
        }
    }


    if ( blockData ){
        foreach(lv::CodeRuntimeBinding* bind, blockData->m_bindings ){
            setFormat(
                bind->position() - currentBlock().position(),
                bind->declaration()->identifierLength(),
                settings[QmlJsSettings::QmlRuntimeBoundProperty]
            );

            if ( bind->isModifiedByEngine() ){
                int valueFrom = bind->position() + bind->declaration()->identifierLength() + bind->declaration()->valueOffset();
                setFormat(
                    valueFrom - currentBlock().position(),
                    bind->declaration()->valueLength(),
                    settings[QmlJsSettings::QmlRuntimeModifiedValue]
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
            setFormat(from < 0 ? 0 : from, length, settings[QmlJsSettings::QmlEdit]);
        }
    }
}

QSet<QString> QmlJsHighlighter::m_knownIds = QmlJsHighlighter::createKnownIds();

QSet<QString> QmlJsHighlighter::createKnownIds(){
    QSet<QString> knownIds;

    // built-in and other popular objects + properties
    knownIds << "Object";
    knownIds << "prototype";
    knownIds << "property";
    knownIds << "__parent__";
    knownIds << "__proto__";
    knownIds << "__defineGetter__";
    knownIds << "__defineSetter__";
    knownIds << "__lookupGetter__";
    knownIds << "__lookupSetter__";
    knownIds << "__noSuchMethod__";
    knownIds << "Function";
    knownIds << "String";
    knownIds << "Array";
    knownIds << "RegExp";
    knownIds << "global";
    knownIds << "NaN";
    knownIds << "undefined";
    knownIds << "Math";
    knownIds << "import";
    knownIds << "string";
    knownIds << "int";
    knownIds << "variant";
    knownIds << "signal";

    return knownIds;
}

}// namespace

