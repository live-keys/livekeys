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

#include "qmltokenizer_p.h"

#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"

#include <QQmlEngine>

#include "qmljs/qmljsscanner.h"

namespace lv{

QmlTokenizer::QmlTokenizer(QObject *parent)
    : QObject(parent)
{
    QQmlEngine* qmlengine = qobject_cast<QQmlEngine*>(parent);
    if ( qmlengine ){
        m_engine = qobject_cast<ViewEngine*>(qmlengine->property("viewEngine").value<QObject*>());
    }

    if ( m_engine ){
        m_tokenKind = m_engine->engine()->newObject();
        m_tokenKind.setProperty("endOfFile",        QQmlJS::Token::EndOfFile);
        m_tokenKind.setProperty("keyword",          QQmlJS::Token::Keyword);
        m_tokenKind.setProperty("identifier",       QQmlJS::Token::Identifier);
        m_tokenKind.setProperty("string",           QQmlJS::Token::String);
        m_tokenKind.setProperty("comment",          QQmlJS::Token::Comment);
        m_tokenKind.setProperty("number",           QQmlJS::Token::Number);
        m_tokenKind.setProperty("leftParenthesis",  QQmlJS::Token::LeftParenthesis);
        m_tokenKind.setProperty("rightParenthesis", QQmlJS::Token::RightParenthesis);
        m_tokenKind.setProperty("leftBrace",        QQmlJS::Token::LeftBrace);
        m_tokenKind.setProperty("rightBrace",       QQmlJS::Token::RightBrace);
        m_tokenKind.setProperty("leftBracket",      QQmlJS::Token::LeftBracket);
        m_tokenKind.setProperty("rightBracket",     QQmlJS::Token::RightBracket);

        m_tokenKind.setProperty("semicolon", QQmlJS::Token::Semicolon);
        m_tokenKind.setProperty("colon",     QQmlJS::Token::Colon);
        m_tokenKind.setProperty("comma",     QQmlJS::Token::Comma);
        m_tokenKind.setProperty("dot",       QQmlJS::Token::Dot);
        m_tokenKind.setProperty("delimiter", QQmlJS::Token::Delimiter);
        m_tokenKind.setProperty("regExp",    QQmlJS::Token::RegExp);

        m_tokenState = m_engine->engine()->newObject();
        m_tokenState.setProperty("normal", QQmlJS::Scanner::Normal);
        m_tokenState.setProperty("multiLineComment", QQmlJS::Scanner::MultiLineComment);
        m_tokenState.setProperty("multiLineStringDQuote", QQmlJS::Scanner::MultiLineStringDQuote);
        m_tokenState.setProperty("multiLineStringSQuote", QQmlJS::Scanner::MultiLineStringSQuote);
        m_tokenState.setProperty("multiLineMask", QQmlJS::Scanner::MultiLineMask);
        m_tokenState.setProperty("regexpMayFollow", QQmlJS::Scanner::RegexpMayFollow);
    }
}

QmlTokenizer::~QmlTokenizer(){

}

QJSValue QmlTokenizer::scan(const QString &text){
    if ( !m_engine )
        return QJSValue();


    QQmlJS::Scanner scanner;
    QList<QQmlJS::Token> tokens = scanner(text);
    QList<QQmlJS::Token>::iterator it = tokens.begin();

    QJSValue result = m_engine->engine()->newArray(static_cast<quint32>(tokens.size()));

    quint32 index = 0;

    while ( it != tokens.end() ){
        QQmlJS::Token& tk = *it;

        QJSValue tokenObject = m_engine->engine()->newObject();
        tokenObject.setProperty("position", tk.offset);
        tokenObject.setProperty("length", tk.length);
        tokenObject.setProperty("kind", tk.kind);
        tokenObject.setProperty("state", tk.state);

        result.setProperty(index++, tokenObject);

        ++it;
    }

    return result;
}

}// namespace
