#include "queryhighlighter.h"

#include "live/elements/parseddocument.h"
#include "live/visuallog.h"

namespace lv{


QueryHighlighter::QueryHighlighter(
        QmlLanguageObject* language,
        const MLNode &settings,
        const std::string &pattern,
        DocumentHandler *,
        QTextDocument *parent)
    : SyntaxHighlighter(parent)
    , m_languageQuery(nullptr)
    , m_currentAst(nullptr)
{
    m_languageQuery = el::LanguageQuery::create(m_parser, pattern);
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

    //TODO: Create parser from language
//    std::string content = parent->toPlainText().toStdString();
//        m_currentAst = m_parser.parse(content);
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

void QueryHighlighter::documentChanged(int, int, int){
    QTextDocument* doc = static_cast<QTextDocument*>(parent());
    std::string content = doc->toPlainText().toStdString();
    m_parser.destroy(m_currentAst);
    m_currentAst = m_parser.parse(content);
}

QList<SyntaxHighlighter::TextFormatRange> QueryHighlighter::highlight(
        int lastUserState, int position, const QString &text)
{
    QTextDocument* doc = static_cast<QTextDocument*>(parent());
    QList<SyntaxHighlighter::TextFormatRange> ranges;

    if ( !m_currentAst )
        return ranges;

    el::LanguageQuery::Cursor::Ptr cursor = m_languageQuery->exec(m_currentAst, position, position + text.length());
    while ( cursor->nextMatch() ){
        uint16_t captures = cursor->totalMatchCaptures();

        if ( m_languageQuery->predicateMatch(cursor, doc) ){
            for ( uint16_t captureIndex = 0; captureIndex < captures; ++captureIndex ){
                uint32_t captureId = cursor->captureId(captureIndex);

                el::SourceRange range = cursor->captureRange(0);
                TextFormatRange r;
                r.start = static_cast<int>(range.from());
                r.length = static_cast<int>(range.length());
                r.userstate = 0;
                r.userstateFollows = 0;
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
