#include "queryhighlighter.h"

#include "live/elements/compiler/parseddocument.h"
#include "live/visuallog.h"

#include <math.h>

namespace lv{

QueryHighlighter::QueryHighlighter(
        QmlLanguageObject* language,
        const MLNode &settings,
        const std::string &pattern,
        CodeHandler *,
        QTextDocument *parent)
    : SyntaxHighlighter(parent)
    , m_parser(nullptr)
    , m_languageQuery(nullptr)
    , m_currentAst(nullptr)
    , m_textDocumentData(new TextDocumentData)
{
    m_parser = el::LanguageParser::create(language->language());
    m_languageQuery = el::LanguageQuery::create(m_parser->language(), pattern);
    m_languageQuery->addPredicate("eq?", &QueryHighlighter::predicateEq);

    QMap<std::string, QTextCharFormat> styleMap;
    for ( auto it = settings.begin(); it != settings.end(); ++it ){
        QTextCharFormat fmt;
        if ( it.value().type() == MLNode::Object ){
            fmt.setBackground(QBrush(QColor(QString::fromStdString(it.value()["background"].asString()))));
            fmt.setForeground(QBrush(QColor(QString::fromStdString(it.value()["foreground"].asString()))));
        } else {
            fmt.clearBackground();
            fmt.setForeground(QBrush(QColor(QString::fromStdString(it.value().asString()))));
        }
        styleMap[it.key()] = fmt;
    }

    uint32_t totalCaptures = m_languageQuery->captureCount();
    for( uint32_t i = 0; i < totalCaptures; ++i ){
        m_captureToFormatMap.insert(i, styleMap[m_languageQuery->captureName(i)]);
    }

    std::string content = parent->toPlainText().toStdString();
        m_currentAst = m_parser->parse(content);
}

QueryHighlighter::~QueryHighlighter(){
}

bool QueryHighlighter::predicateEq(const std::vector<el::LanguageQuery::PredicateData> &args, void *payload){
    QTextDocument* doc = reinterpret_cast<QTextDocument*>(payload);
    if ( args.size() != 2 )
        THROW_EXCEPTION(lv::Exception, "Predicate eq? requires 2 arguments.", Exception::toCode("~Arguments"));

    QString compare1;
    QString compare2;
    if ( args[0].m_range.isValid() ){
        compare1 = doc->toPlainText().mid(args[0].m_range.from(), args[0].m_range.length());
    } else {
        compare1 = QString::fromStdString(args[0].m_value.data());
    }
    if ( args[1].m_range.isValid() ){
        compare2 = doc->toPlainText().mid(args[1].m_range.from(), args[1].m_range.length());
    } else {
        compare2 = QString::fromStdString(args[1].m_value.data());
    }

    return compare1 == compare2;
}

const char *QueryHighlighter::parsingCallback(void *payload, uint32_t, TSPoint position, uint32_t *bytes_read)
{
    TextDocumentData* textDocumentData = reinterpret_cast<TextDocumentData*>(payload);
    unsigned ushortsize = sizeof(ushort) / sizeof(char);

    if (position.row >= textDocumentData->size())
    {
        *bytes_read = 0;
        return nullptr;
    }
    std::u16string& row = textDocumentData->rowAt(position.row);
    if (position.column >= row.size() * ushortsize)
    {
        *bytes_read = 0;
        return nullptr;
    }

    *bytes_read = row.size()*ushortsize - position.column;
    return reinterpret_cast<const char*>(row.data() + position.column / ushortsize);
}

void QueryHighlighter::documentChanged(int pos, int removed, int added){
    QTextDocument* doc = static_cast<QTextDocument*>(parent());

    std::vector<std::pair<uint32_t, uint32_t>> editPoints =
            m_textDocumentData->contentsChange(doc, pos, removed, added);

    uint32_t start = pos*sizeof(ushort)/sizeof(char);
    uint32_t old_end = (pos + removed)*sizeof(ushort)/sizeof(char);
    uint32_t new_end = (pos + added)*sizeof(ushort)/sizeof(char);

    TSInputEdit edit = {start, old_end, new_end,
                        TSPoint{editPoints[0].first, editPoints[0].second},
                        TSPoint{editPoints[1].first, editPoints[1].second},
                        TSPoint{editPoints[2].first, editPoints[2].second}};
    TSInput input = {m_textDocumentData, QueryHighlighter::parsingCallback, TSInputEncodingUTF16};

    m_parser->editParseTree(m_currentAst, edit, input);
}

QList<SyntaxHighlighter::TextFormatRange> QueryHighlighter::highlight(
        int lastUserState, int position, const QString &text)
{
    QTextDocument* doc = static_cast<QTextDocument*>(parent());
    QList<SyntaxHighlighter::TextFormatRange> ranges;

    if ( !m_currentAst )
        return ranges;

    el::LanguageQuery::Cursor::Ptr cursor = m_languageQuery->exec(m_currentAst, position*sizeof(ushort), (position + text.length())*sizeof(ushort));
    while ( cursor->nextMatch() ){
        uint16_t captures = cursor->totalMatchCaptures();

        if ( m_languageQuery->predicateMatch(cursor, doc) ){
            for ( uint16_t captureIndex = 0; captureIndex < captures; ++captureIndex ){
                uint32_t captureId = cursor->captureId(captureIndex);

                el::SourceRange range = cursor->captureRange(0);
                int from = static_cast<int>(range.from()) / sizeof(ushort);
                int length = static_cast<int>(range.length()) / sizeof(ushort);

                TextFormatRange r;
                r.start = fmax(from, position);
                r.length = fmin(from + length, position + text.length()) - r.start;
                r.userstate = 0;
                auto name = m_languageQuery->captureName(captureId);

                if (name == "string")
                {
                    r.userstate = 2;
                    if (r.start + r.length == from + length) r.userstateFollows = 0;
                    else r.userstateFollows = 2;
                } else {
                    r.userstate = 0;
                    r.userstateFollows = 0;
                }

                r.format = m_captureToFormatMap[captureId];
                ranges.append(r);
            }
        }
    }

    return ranges;
}

QList<SyntaxHighlighter::TextFormatRange> QueryHighlighter::highlightSections(const QList<ProjectDocumentSection::Ptr> &){
    return QList<SyntaxHighlighter::TextFormatRange>();
}

}// namespace
