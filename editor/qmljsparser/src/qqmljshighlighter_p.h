/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

// The QCodeJSHighlighter class is a modified version of the JSHighlighter clas present
// in Ofi Labs X2 project, which retains the following copyright notice :
//
// Copyright (C) 2011 Ariya Hidayat <ariya.hidayat@gmail.com>
// Copyright (C) 2010 Ariya Hidayat <ariya.hidayat@gmail.com>
// ------------------------------------------------------------------------------------

#ifndef QCODEJSHIGHLIGHTER_P_HPP
#define QCODEJSHIGHLIGHTER_P_HPP

#include <QTextBlockUserData>
#include <QSyntaxHighlighter>

#include "qprojectdocument.h"

/**
 * @brief The QCodeJSHighlighter is a private class used internally by QCodeHandler.
 */
class QQmlJsHighlighter : public QSyntaxHighlighter{

public:
    enum ColorComponent{
        Background,
        Normal,
        Comment,
        Number,
        String,
        Operator,
        Identifier,
        Keyword,
        BuiltIn,
        Sidebar,
        LineNumber,
        Cursor,
        Marker,
        BracketMatch,
        BracketError,
        FoldIndicator
    };

public:
    QQmlJsHighlighter(QTextDocument *parent = 0);

    void setColor(ColorComponent component, const QColor &color);
    void mark(const QString &str, Qt::CaseSensitivity caseSensitivity);

    QStringList keywords() const;
    void setKeywords(const QStringList &keywords);

protected:
    void highlightBlock(const QString &text);

private:
    QSet<QString> m_keywords;
    QSet<QString> m_knownIds;
    QHash<ColorComponent, QColor> m_colors;
    QString m_markString;
    Qt::CaseSensitivity m_markCaseSensitivity;
};

QQmlJsHighlighter::QQmlJsHighlighter(QTextDocument *parent)
: QSyntaxHighlighter(parent)
, m_markCaseSensitivity(Qt::CaseInsensitive){

    // default color scheme
    m_colors[Normal]     = QColor("#fff");
    m_colors[Comment]    = QColor("#56748a");
    m_colors[Number]     = QColor("#ca7000");
    m_colors[String]     = QColor("#358d37");
    m_colors[Operator]   = QColor("#c0a000");
    m_colors[Identifier] = QColor("#93672f");
    m_colors[Keyword]    = QColor("#a0a000");
    m_colors[BuiltIn]    = QColor("#93672f");
    m_colors[Marker]     = QColor("#ffff00");

    // https://developer.mozilla.org/en/JavaScript/Reference/Reserved_Words
    m_keywords << "break";
    m_keywords << "case";
    m_keywords << "catch";
    m_keywords << "continue";
    m_keywords << "default";
    m_keywords << "delete";
    m_keywords << "do";
    m_keywords << "else";
    m_keywords << "finally";
    m_keywords << "for";
    m_keywords << "function";
    m_keywords << "if";
    m_keywords << "in";
    m_keywords << "instanceof";
    m_keywords << "new";
    m_keywords << "return";
    m_keywords << "switch";
    m_keywords << "this";
    m_keywords << "throw";
    m_keywords << "try";
    m_keywords << "typeof";
    m_keywords << "var";
    m_keywords << "void";
    m_keywords << "while";
    m_keywords << "with";
    m_keywords << "true";
    m_keywords << "false";
    m_keywords << "null";

    // built-in and other popular objects + properties
    m_knownIds << "Object";
    m_knownIds << "prototype";
    m_knownIds << "create";
    m_knownIds << "property";
    m_knownIds << "defineProperty";
    m_knownIds << "defineProperties";
    m_knownIds << "getOwnPropertyDescriptor";
    m_knownIds << "keys";
    m_knownIds << "getOwnPropertyNames";
    m_knownIds << "constructor";
    m_knownIds << "__parent__";
    m_knownIds << "__proto__";
    m_knownIds << "__defineGetter__";
    m_knownIds << "__defineSetter__";
    m_knownIds << "eval";
    m_knownIds << "hasOwnProperty";
    m_knownIds << "isPrototypeOf";
    m_knownIds << "__lookupGetter__";
    m_knownIds << "__lookupSetter__";
    m_knownIds << "__noSuchMethod__";
    m_knownIds << "propertyIsEnumerable";
    m_knownIds << "toSource";
    m_knownIds << "toLocaleString";
    m_knownIds << "toString";
    m_knownIds << "unwatch";
    m_knownIds << "valueOf";
    m_knownIds << "watch";
    m_knownIds << "Function";
    m_knownIds << "arguments";
    m_knownIds << "arity";
    m_knownIds << "caller";
    m_knownIds << "constructor";
    m_knownIds << "length";
    m_knownIds << "name";
    m_knownIds << "apply";
    m_knownIds << "bind";
    m_knownIds << "call";
    m_knownIds << "String";
    m_knownIds << "fromCharCode";
    m_knownIds << "length";
    m_knownIds << "charAt";
    m_knownIds << "charCodeAt";
    m_knownIds << "concat";
    m_knownIds << "indexOf";
    m_knownIds << "lastIndexOf";
    m_knownIds << "localCompare";
    m_knownIds << "match";
    m_knownIds << "quote";
    m_knownIds << "replace";
    m_knownIds << "search";
    m_knownIds << "slice";
    m_knownIds << "split";
    m_knownIds << "substr";
    m_knownIds << "substring";
    m_knownIds << "toLocaleLowerCase";
    m_knownIds << "toLocaleUpperCase";
    m_knownIds << "toLowerCase";
    m_knownIds << "toUpperCase";
    m_knownIds << "trim";
    m_knownIds << "trimLeft";
    m_knownIds << "trimRight";
    m_knownIds << "Array";
    m_knownIds << "isArray";
    m_knownIds << "index";
    m_knownIds << "pop";
    m_knownIds << "push";
    m_knownIds << "reverse";
    m_knownIds << "shift";
    m_knownIds << "sort";
    m_knownIds << "splice";
    m_knownIds << "unshift";
    m_knownIds << "concat";
    m_knownIds << "join";
    m_knownIds << "filter";
    m_knownIds << "forEach";
    m_knownIds << "every";
    m_knownIds << "map";
    m_knownIds << "some";
    m_knownIds << "reduce";
    m_knownIds << "reduceRight";
    m_knownIds << "RegExp";
    m_knownIds << "global";
    m_knownIds << "ignoreCase";
    m_knownIds << "lastIndex";
    m_knownIds << "multiline";
    m_knownIds << "source";
    m_knownIds << "exec";
    m_knownIds << "test";
    m_knownIds << "JSON";
    m_knownIds << "parse";
    m_knownIds << "stringify";
    m_knownIds << "decodeURI";
    m_knownIds << "decodeURIComponent";
    m_knownIds << "encodeURI";
    m_knownIds << "encodeURIComponent";
    m_knownIds << "eval";
    m_knownIds << "isFinite";
    m_knownIds << "isNaN";
    m_knownIds << "parseFloat";
    m_knownIds << "parseInt";
    m_knownIds << "Infinity";
    m_knownIds << "NaN";
    m_knownIds << "undefined";
    m_knownIds << "Math";
    m_knownIds << "E";
    m_knownIds << "LN2";
    m_knownIds << "LN10";
    m_knownIds << "LOG2E";
    m_knownIds << "LOG10E";
    m_knownIds << "PI";
    m_knownIds << "SQRT1_2";
    m_knownIds << "SQRT2";
    m_knownIds << "abs";
    m_knownIds << "acos";
    m_knownIds << "asin";
    m_knownIds << "atan";
    m_knownIds << "atan2";
    m_knownIds << "ceil";
    m_knownIds << "cos";
    m_knownIds << "exp";
    m_knownIds << "floor";
    m_knownIds << "log";
    m_knownIds << "max";
    m_knownIds << "min";
    m_knownIds << "pow";
    m_knownIds << "random";
    m_knownIds << "round";
    m_knownIds << "sin";
    m_knownIds << "sqrt";
    m_knownIds << "tan";
    m_knownIds << "document";
    m_knownIds << "window";
    m_knownIds << "navigator";
    m_knownIds << "userAgent";
    m_knownIds << "import";
    m_knownIds << "string";
    m_knownIds << "int";
    m_knownIds << "variant";
    m_knownIds << "signal";
}

void QQmlJsHighlighter::setColor(ColorComponent component, const QColor &color){

    m_colors[component] = color;
    rehighlight();
}

void QQmlJsHighlighter::highlightBlock(const QString &text){

    // parsing state
    enum States{
        StateStart      = 0,
        StateNumber     = 1,
        StateIdentifier = 2,
        StateString     = 3,
        StateComment    = 4,
        StateRegex      = 5
    };

    QList<int> bracketPositions;
    int blockState   = previousBlockState();

    bool prevGenerated   = (blockState >> 4) & 1;
    int bracketLevel = blockState >> 5;
    int state        = blockState & 15;


    if (blockState < 0) {
        prevGenerated = false;
        bracketLevel = 0;
        state = StateStart;
    }

    int start = 0;
    int i     = 0;

    while (i <= text.length()) {
        QChar ch   = (i < text.length())     ? text.at(i) : QChar();
        QChar next = (i < text.length() - 1) ? text.at(i + 1) : QChar();
        switch (state) {
        case StateStart:
            start = i;
            if (ch.isSpace()) {
                ++i;
            } else if (ch.isDigit()) {
                ++i;
                state = StateNumber;
            } else if (ch.isLetter() || ch == '_') {
                ++i;
                state = StateIdentifier;
            } else if (ch == '\'' || ch == '\"') {
                ++i;
                state = StateString;
            } else if (ch == '/' && next == '*') {
                ++i;
                ++i;
                state = StateComment;
            } else if (ch == '/' && next == '/') {
                i = text.length();
                setFormat(start, text.length(), m_colors[Comment]);
            } else if (ch == '/' && next != '*') {
                ++i;
                state = StateRegex;
            } else {

                if (!QString("(){}[]").contains(ch))
                    setFormat(start, 1, m_colors[Operator]);

                if (ch =='{' || ch == '}') {
                    bracketPositions += i;
                    if (ch == '{')
                        bracketLevel++;
                    else
                        bracketLevel--;
                }
                ++i;
                state = StateStart;
            }
            break;

        case StateNumber:
            if (ch.isSpace() || !ch.isDigit()) {
                setFormat(start, i - start, m_colors[Number]);
                state = StateStart;
            } else {
                ++i;
            }
            break;

        case StateIdentifier:
            if (ch.isSpace() || !(ch.isDigit() || ch.isLetter() || ch == '_')) {
                QString token = text.mid(start, i - start).trimmed();
                if (m_keywords.contains(token))
                    setFormat(start, i - start, m_colors[Keyword]);
                else if (m_knownIds.contains(token))
                    setFormat(start, i - start, m_colors[BuiltIn]);
                state = StateStart;
            } else {
                ++i;
            }
            break;

        case StateString:
            if (ch == text.at(start)) {
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                if (prev != '\\') {
                    ++i;
                    setFormat(start, i - start, m_colors[String]);
                    state = StateStart;
                } else {
                    ++i;
                }
            } else {
                ++i;
            }
            break;

        case StateComment:
            if (ch == '*' && next == '/') {
                ++i;
                ++i;
                setFormat(start, i - start, m_colors[Comment]);
                state = StateStart;
            } else {
                ++i;
            }
            break;

        case StateRegex:
            if (ch == '/') {
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                if (prev != '\\') {
                    ++i;
                    setFormat(start, i - start, m_colors[String]);
                    state = StateStart;
                } else {
                    ++i;
                }
            } else {
                ++i;
            }
            break;

        default:
            state = StateStart;
            break;
        }
    }

    if (state == StateComment)
        setFormat(start, text.length(), m_colors[Comment]);
    else
        state = StateStart;

    if (!m_markString.isEmpty()) {
        int pos = 0;
        int len = m_markString.length();
        QTextCharFormat markerFormat;
        markerFormat.setBackground(m_colors[Marker]);
        markerFormat.setForeground(m_colors[Normal]);

        for (;;) {
            pos = text.indexOf(m_markString, pos, m_markCaseSensitivity);
            if (pos < 0)
                break;
            setFormat(pos, len, markerFormat);
            ++pos;
        }
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
                    setFormat(0, currentBlock().length(), QColor("#aa00aa") );

                    if (!blockData) {
                        blockData = new lcv::QProjectDocumentBlockData;
                        currentBlock().setUserData(blockData);
                    }
                    blockData->exceededBindingLength = currentExceededLength;
                    generated = true;
                } else {
                    setFormat(0, prevBlockData->exceededBindingLength, QColor("#aa00aa") );
                }
            }
        }
    }


    if ( blockData ){
        foreach(lcv::QProjectDocumentBinding* bind, blockData->m_bindings ){
            setFormat(bind->propertyPosition - currentBlock().position(), bind->propertyLength, QColor("#ff0000"));

            if ( bind->modifiedByEngine ){
                int valueFrom = bind->propertyPosition + bind->propertyLength + bind->valuePositionOffset;
                setFormat(valueFrom - currentBlock().position(), bind->valueLength, QColor("#ff00ff"));
                if ( valueFrom + bind->valueLength > currentBlock().position() + currentBlock().length() ){
                    generated = true;
                    blockData->exceededBindingLength =
                        bind->valueLength - (currentBlock().length() - (valueFrom - currentBlock().position()));
                }
            }
        }
    }

    blockState = (state & 15) | (generated << 4) | (bracketLevel << 5);
    setCurrentBlockState(blockState);
}

void QQmlJsHighlighter::mark(const QString &str, Qt::CaseSensitivity caseSensitivity){
    m_markString = str;
    m_markCaseSensitivity = caseSensitivity;
    rehighlight();
}

QStringList QQmlJsHighlighter::keywords() const{
    return m_keywords.toList();
}

void QQmlJsHighlighter::setKeywords(const QStringList &keywords){
    m_keywords = QSet<QString>::fromList(keywords);
    rehighlight();
}

#endif // QCODEJSHIGHLIGHTER_P_HPP
