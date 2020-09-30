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
        m_tokenKind.setProperty("endOfFile",        QmlJS::Token::EndOfFile);
        m_tokenKind.setProperty("keyword",          QmlJS::Token::Keyword);
        m_tokenKind.setProperty("identifier",       QmlJS::Token::Identifier);
        m_tokenKind.setProperty("string",           QmlJS::Token::String);
        m_tokenKind.setProperty("comment",          QmlJS::Token::Comment);
        m_tokenKind.setProperty("number",           QmlJS::Token::Number);
        m_tokenKind.setProperty("leftParenthesis",  QmlJS::Token::LeftParenthesis);
        m_tokenKind.setProperty("rightParenthesis", QmlJS::Token::RightParenthesis);
        m_tokenKind.setProperty("leftBrace",        QmlJS::Token::LeftBrace);
        m_tokenKind.setProperty("rightBrace",       QmlJS::Token::RightBrace);
        m_tokenKind.setProperty("leftBracket",      QmlJS::Token::LeftBracket);
        m_tokenKind.setProperty("rightBracket",     QmlJS::Token::RightBracket);

        m_tokenKind.setProperty("semicolon", QmlJS::Token::Semicolon);
        m_tokenKind.setProperty("colon",     QmlJS::Token::Colon);
        m_tokenKind.setProperty("comma",     QmlJS::Token::Comma);
        m_tokenKind.setProperty("dot",       QmlJS::Token::Dot);
        m_tokenKind.setProperty("delimiter", QmlJS::Token::Delimiter);
        m_tokenKind.setProperty("regExp",    QmlJS::Token::RegExp);

        m_tokenState = m_engine->engine()->newObject();
        m_tokenState.setProperty("normal", QmlJS::Scanner::Normal);
        m_tokenState.setProperty("multiLineComment", QmlJS::Scanner::MultiLineComment);
        m_tokenState.setProperty("multiLineStringDQuote", QmlJS::Scanner::MultiLineStringDQuote);
        m_tokenState.setProperty("multiLineStringSQuote", QmlJS::Scanner::MultiLineStringSQuote);
        m_tokenState.setProperty("multiLineMask", QmlJS::Scanner::MultiLineMask);
        m_tokenState.setProperty("regexpMayFollow", QmlJS::Scanner::RegexpMayFollow);
    }
}

QmlTokenizer::~QmlTokenizer(){

}

QJSValue QmlTokenizer::scan(const QString &text){
    if ( !m_engine )
        return QJSValue();


    QmlJS::Scanner scanner;
    QList<QmlJS::Token> tokens = scanner(text);
    QList<QmlJS::Token>::iterator it = tokens.begin();

    QJSValue result = m_engine->engine()->newArray(static_cast<quint32>(tokens.size()));

    quint32 index = 0;

    while ( it != tokens.end() ){
        QmlJS::Token& tk = *it;

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
