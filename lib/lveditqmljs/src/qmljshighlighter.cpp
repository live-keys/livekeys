/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "qmljshighlighter_p.h"
#include "documentqmlvaluescanner_p.h"

namespace lv{

QmlJsHighlighter::QmlJsHighlighter(QmlJsSettings* settings, DocumentHandler*, QTextDocument *parent)
    : SyntaxHighlighter(parent)
    , m_settings(settings)
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

void QmlJsHighlighter::collapse(const QTextBlock &tb, int &numLines, QString &replacement){
    int blockState   = tb.previous().userState();

    bool prevSectionExceeded = (blockState >> 4) & 1;
    int bracketLevel         = blockState >> 5;
    int state                = blockState & 15;


    if (blockState < 0) {
        prevSectionExceeded = false;
        bracketLevel        = 0;
        state               = QmlJS::Scanner::Normal;
    }

    QmlJS::Scanner scanner;
    QList<QmlJS::Token> tokens = scanner(tb.text(), state);
    QList<QmlJS::Token>::iterator it = tokens.begin();
    while ( it != tokens.end() ){
        QmlJS::Token& tk = *it;
        if ( tk.kind == QmlJS::Token::LeftBrace ){
            QTextBlock bScan = tb;
            if ( DocumentQmlValueScanner::getBlockEnd(bScan, tb.position() + tk.end()) == 0 )
            {
                numLines = tb.document()->blockCount() - tb.blockNumber();
            } else {

                numLines = bScan.blockNumber() - tb.blockNumber();
            }
            replacement = "{ ... }";

            return;
        }
        ++it;
    }
}

void QmlJsHighlighter::highlightBlock(const QString &text){
    QList<int> bracketPositions;
    int blockState   = previousBlockState();

    bool prevSectionExceeded = (blockState >> 4) & 1;
    int bracketLevel         = blockState >> 5;
    int state                = blockState & 15;


    if (blockState < 0) {
        prevSectionExceeded = false;
        bracketLevel        = 0;
        state               = QmlJS::Scanner::Normal;
    }

    QmlJsSettings& settings = *m_settings;

    QmlJS::Scanner scanner;
    QList<QmlJS::Token> tokens = scanner(text, state);
    state = scanner.state();

    lv::ProjectDocumentBlockData *blockData =
            reinterpret_cast<lv::ProjectDocumentBlockData*>(currentBlock().userData());
    if (!blockData) {
        blockData = new lv::ProjectDocumentBlockData;
        blockData->setCollapse(&QmlJsHighlighter::collapse);
        blockData->setCollapsible(false);
        currentBlock().setUserData(blockData);
    } else {
        blockData->resetCollapseParams();
    }

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
            break;
        case QmlJS::Token::LeftBrace:
            blockData->setCollapse(&QmlJsHighlighter::collapse);
            blockData->setStateChangeFlag(true);
            blockData->setCollapsible(true);
            document()->markContentsDirty(currentBlock().position(), currentBlock().length());
            break;
        case QmlJS::Token::RightBrace:
            if (blockData->isCollapsible())
                blockData->setCollapsible(false);
            break;
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

    blockData->m_exceededSections.clear();

    if (!bracketPositions.isEmpty()) {
        blockData->bracketPositions = bracketPositions;
    }

    bool exceeded = false;

    if ( prevSectionExceeded ){
        QTextBlock prevBlock = currentBlock().previous();
        if ( prevBlock.isValid() && prevBlock.userData() ){
            lv::ProjectDocumentBlockData *prevBlockData =
                    reinterpret_cast<lv::ProjectDocumentBlockData*>(prevBlock.userData());

            foreach (const ProjectDocumentSection::Ptr& section, prevBlockData->m_exceededSections ){
                if ( section->isValid() ){
                    highlightSection(section, blockData, exceeded);
                }
            }
        }
    }


    if ( blockData ){
        foreach(const ProjectDocumentSection::Ptr& section, blockData->m_sections ){
            highlightSection(section, blockData, exceeded);
        }
    }

    blockState = (state & 15) | (exceeded << 4) | (bracketLevel << 5);
    setCurrentBlockState(blockState);
}

QList<SyntaxHighlighter::TextFormatRange> QmlJsHighlighter::highlight(int lastUserState, int position, const QString &text)
{
    int blockState = lastUserState;

    bool prevSectionExceeded    = false;
    int bracketLevel            = 0;
    int state                   = QmlJS::Scanner::Normal;

    if (blockState >= 0) {
        prevSectionExceeded     = (blockState >> 4) & 1;
        bracketLevel            = blockState >> 5;
        state                   = blockState & 15;
    }

    QmlJsSettings& settings = *m_settings;

    int internalPosition = 0;
    QStringList blocks = text.split('\n');
    QList<SyntaxHighlighter::TextFormatRange> result;

    for (QString block: blocks)
    {
        QmlJS::Scanner scanner;
        QList<QmlJS::Token> tokens = scanner(block, state);
        state = scanner.state();

        int formatType = -1;
        int collapsibleType = -1;

        QList<QmlJS::Token>::iterator it = tokens.begin();

        // empty block handling
        if (tokens.empty())
        {
            TextFormatRange r;
            r.start = position + internalPosition;
            r.length = 0;
            r.userstate = state;
            r.userstateFollows = state;
            result.push_back(r);
        }

        //    blockState = (state & 15) | (exceeded << 4) | (bracketLevel << 5);

        while ( it != tokens.end() ){
            QmlJS::Token& tk = *it;
            TextFormatRange r;
            formatType = -1;
            collapsibleType = -1;

            bool lastToken = (it == std::prev(tokens.end()));

            switch(tk.kind){

            case QmlJS::Token::Keyword:
                formatType = QmlJsSettings::Keyword;
                break;
            case QmlJS::Token::Identifier:{
                QString tktext = text.mid(internalPosition + tk.begin(), tk.length);
                if ( m_knownIds.contains(tktext) ){
                    formatType = QmlJsSettings::Identifier;
                } else if ( tktext == "true" || tktext == "false" ){
                    formatType = QmlJsSettings::Keyword;
                } else {
                    QList<QmlJS::Token>::iterator lait = it;
                    QmlJsHighlighter::LookAheadType la = lookAhead(text, tokens, ++lait, state);
                    if ( la == QmlJsHighlighter::Property ) {
                        formatType = QmlJsSettings::QmlProperty;
                    }
                    else if ( la == QmlJsHighlighter::Type ) {
                        formatType = QmlJsSettings::QmlType;
                    }
                }
                break;
            }
            case QmlJS::Token::String:
                formatType = QmlJsSettings::String;
                break;
            case QmlJS::Token::Comment:
                formatType = QmlJsSettings::Comment;
                break;
            case QmlJS::Token::Number:
                formatType = QmlJsSettings::Number;
                break;
            case QmlJS::Token::LeftParenthesis:
            case QmlJS::Token::RightParenthesis:
                break;
            case QmlJS::Token::LeftBrace:
                collapsibleType = 1;
                break;
            case QmlJS::Token::RightBrace:
                collapsibleType = 0;
                break;
            case QmlJS::Token::LeftBracket:
            case QmlJS::Token::RightBracket:
            case QmlJS::Token::EndOfFile:
                break;
            case QmlJS::Token::Semicolon:
            case QmlJS::Token::Colon:
            case QmlJS::Token::Comma:
            case QmlJS::Token::Dot:
            case QmlJS::Token::Delimiter:
                formatType = QmlJsSettings::Operator;
                break;
            case QmlJS::Token::RegExp:
                formatType = QmlJsSettings::String;
                break;
            }

            r.start = position + internalPosition + tk.begin();
            r.length = tk.length;
            r.userstate = tk.state;
            r.userstateFollows = lastToken ? state: std::next(it)->state;

            if (formatType != -1)
                r.format = settings[static_cast<QmlJsSettings::ColorComponent>(formatType)];

            if (collapsibleType != -1)
            {
                r.collapsible = collapsibleType ? true : false;
                r.function = &QmlJsHighlighter::collapse;
            } else {
                r.collapsible = false;
                r.function = nullptr;
            }

            result.push_back(r);

            ++it;
        }

        internalPosition += block.length() + 1;
    }

    return result;
}

QList<SyntaxHighlighter::TextFormatRange> QmlJsHighlighter::highlightSections(const QList<ProjectDocumentSection::Ptr>& sectionList)
{
    QmlJsSettings& settings = *m_settings;
    QList<TextFormatRange> result;
    for (auto section: sectionList)
    {
        if (section->type() != lv::QmlEditFragment::Section || !section->userData()) continue;

        lv::QmlEditFragment* def = reinterpret_cast<lv::QmlEditFragment*>(section->userData());

        if (def->bindingPalette())
        {
            TextFormatRange formatRangeIdentifier;
            formatRangeIdentifier.start = def->declaration()->position();
            formatRangeIdentifier.length = def->declaration()->identifierLength();
            formatRangeIdentifier.format = settings[QmlJsSettings::QmlRuntimeBoundProperty];

            result.push_back(formatRangeIdentifier);
        }

        TextFormatRange formatRangeValue;
        formatRangeValue.start = def->valuePosition();
        formatRangeValue.length = def->valueLength();

        if (def->totalPalettes())
        {
            formatRangeValue.format = settings[QmlJsSettings::QmlEdit];
            result.push_back(formatRangeValue);
        }
        else if (def->bindingPalette())
        {
            formatRangeValue.format = settings[QmlJsSettings::QmlRuntimeModifiedValue];
            result.push_back(formatRangeValue);
        }
    }

    std::sort(result.begin(), result.end(), [](TextFormatRange r1, TextFormatRange r2){ return r1.start < r2.start; });

    return result;
}

void QmlJsHighlighter::highlightSection(const ProjectDocumentSection::Ptr &section, ProjectDocumentBlockData *blockData, bool &exceeded){
    QmlJsSettings& settings = *m_settings;
    if ( section->type() == lv::QmlEditFragment::Section && section->userData() ){
        lv::QmlEditFragment* def = reinterpret_cast<lv::QmlEditFragment*>(section->userData());
        if ( def->bindingPalette() ){
            int identifierBegin = def->declaration()->position() - currentBlock().position();
            int identifierEnd   = identifierBegin + def->declaration()->identifierLength();

            if ( identifierEnd > 0 ){
                if ( identifierBegin < 0 )
                    identifierBegin = 0;
                setFormat(
                    identifierBegin,
                    identifierEnd - identifierBegin,
                    settings[QmlJsSettings::QmlRuntimeBoundProperty]
                );
            }

            int valueBegin = def->valuePosition() - currentBlock().position();
            int valueEnd   = def->valuePosition() + def->declaration()->valueLength();

            if ( valueBegin < 0 )
                valueBegin = 0;

            setFormat(valueBegin, valueEnd - valueBegin, settings[QmlJsSettings::QmlRuntimeModifiedValue]);
            if ( valueEnd > currentBlock().position() + currentBlock().length() ){
                exceeded = true;
                blockData->m_exceededSections.append(section);
            }
        }
        if ( def->totalPalettes() ){
            int valueBegin = def->valuePosition() - currentBlock().position();
            int valueEnd   = def->valuePosition() + def->valueLength();

            if ( valueBegin < 0 )
                valueBegin = 0;

            setFormat(valueBegin, valueEnd - valueBegin, settings[QmlJsSettings::QmlEdit]);
            if ( valueEnd > currentBlock().position() + currentBlock().length() ){
                exceeded = true;
                blockData->m_exceededSections.append(section);
            }
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
