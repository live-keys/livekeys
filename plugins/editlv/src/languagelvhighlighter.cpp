#include "languagelvhighlighter.h"

#include "live/elements/parseddocument.h"
#include "live/visuallog.h"
#include "live/elements/parseddocument.h"

#include <math.h>

namespace lv{

LanguageLvHighlighter::LanguageLvHighlighter(EditLvSettings *settings, DocumentHandler *, QTextDocument *parent)
    : SyntaxHighlighter(parent)
    , m_parser(el::LanguageParser::createForElements())
    , m_languageQuery(nullptr)
    , m_settings(settings)
    , m_currentAst(nullptr)
    , m_textDocumentData(new TextDocumentData)
{
    // capture index to formats

    std::string pattern =
        "(this) @variable.builtin \n"
        "(super) @variable.builtin \n"
        "(true) @constant.builtin \n"
        "(false) @constant.builtin \n"
        "(comment) @comment \n"
        "(string) @string \n"
        "(number) @number \n"
        /*===============================*/
        "\"as\" @keyword \n"
        "\"async\" @keyword \n"
        "\"await\" @keyword \n"
        "\"break\" @keyword \n"
        "\"case\" @keyword \n"
        "\"catch\" @keyword \n"
        "\"class\" @keyword \n"
        "\"const\" @keyword \n"
        "\"continue\" @keyword \n"
        "\"debugger\" @keyword \n"
        "\"default\" @keyword \n"
        "\"delete\" @keyword \n"
        "\"else\" @keyword \n"
        "\"extends\" @keyword \n"
        "\"finally\" @keyword \n"
        "\"for\" @keyword \n"
        "\"function\" @keyword \n"
        "\"get\" @keyword \n"
        "\"if\" @keyword \n"
        "\"import\" @keyword \n"
        "\"in\" @keyword \n"
        "\"instanceof\" @keyword \n"
        "\"let\" @keyword \n"
        "\"new\" @keyword \n"
        "\"of\" @keyword \n"
        "\"return\" @keyword \n"
        "\"set\" @keyword \n"
        "\"static\" @keyword \n"
        "\"switch\" @keyword \n"
        "\"target\" @keyword \n"
        "\"throw\" @keyword \n"
        "\"try\" @keyword \n"
        "\"typeof\" @keyword \n"
        "\"var\" @keyword \n"
        "\"void\" @keyword \n"
        "\"while\" @keyword \n"
        "\"with\" @keyword \n"
        "\"yield\" @keyword \n"
        // "\"component\" @keyword \n"
        "\"fn\" @keyword \n"
        "\"constructor\" @keyword \n"
        "\"instance\" @keyword \n"
        /*===============================*/
        "\"--\" @operator \n"
        "\"-\" @operator \n"
        "\"-=\" @operator \n"
        "\"&&\" @operator \n"
        "\"+\" @operator \n"
        "\"++\" @operator \n"
        "\"+=\" @operator \n"
        "\"<\" @operator \n"
        "\"<<\" @operator \n"
        "\"=\" @operator \n"
        "\"==\" @operator \n"
        "\"===\" @operator \n"
        "\"=>\" @operator \n"
        "\">\" @operator \n"
        "\">>\" @operator \n"
        "\"||\" @operator \n"
        /*===============================*/
        "((identifier) @keyword (eq-or? @keyword"
        "    \"Object\""
        "    \"prototype\""
        "    \"property\""
        "    \"Function\""
        "    \"String\""
        "    \"Array\""
        "    \"RegExp\""
        "    \"global\""
        "    \"NaN\""
        "    \"undefined\""
        "    \"Math\""
        "    \"string\""
        "    \"int\""
        "    \"variant\""
        "    \"signal\""
        ")) \n"
        /*===============================*/
        "( (identifier) "
        "  @variable.builtin "
        "  (eq-or? @variable.builtin \"console\" \"parent\")"
        ") \n"
        /*===============================*/
        "(property_declaration "
        "   type: (identifier) @type"
        ") \n"
        /*===============================*/
        "(property_declaration "
        "   name: (property_identifier) @property"
        ") \n"
        /*===============================*/
        "(new_component_expression"
        "   name: (identifier) @type)\n"
        /*==============================*/
        "(component_declaration"
        "    name: (identifier) @type"
        ") \n"
        /*==============================*/
        "(component_heritage (identifier) @type) \n"
        /*==============================*/
        "(typed_function_declaration "
        "    name: (property_identifier) @property) \n"
        /*==============================*/
        "(identifier_property_assignment name: \"id\" @property) \n"
        /*==============================*/
        "(formal_type_parameter"
        "     parameter_type: (identifier) @type \n"
        ") \n"
        /*==============================*/
        "(regex_pattern) @builtin \n"
    ;

    m_languageQuery = el::LanguageQuery::create(m_parser->language(), pattern);
    m_languageQuery->addPredicate("eq?", &LanguageLvHighlighter::predicateEq);
    m_languageQuery->addPredicate("eq-or?", &LanguageLvHighlighter::predicateEqOr);

    uint32_t totalCaptures = m_languageQuery->captureCount();
    for( uint32_t i = 0; i < totalCaptures; ++i ){
        m_captureToFormatMap.insert(i, (*m_settings)[m_languageQuery->captureName(i)]);
    }

    m_captureToFormatMap.insert(UINT_MAX-1, (*m_settings)["comment"]);
    m_captureToFormatMap.insert(UINT_MAX, (*m_settings)["text"]);

    std::string content = parent->toPlainText().toStdString();
    m_currentAst = m_parser->parse(content);
}

LanguageLvHighlighter::~LanguageLvHighlighter(){
}

bool LanguageLvHighlighter::predicateEq(const std::vector<el::LanguageQuery::PredicateData> &args, void *payload){
    QTextDocument* doc = reinterpret_cast<QTextDocument*>(payload);
    if ( args.size() != 2 )
        THROW_EXCEPTION(lv::Exception, "Predicate eq? requires 2 arguments.", Exception::toCode("~Arguments"));

    QString compare1;
    QString compare2;
    if ( args[0].m_range.isValid() ){
        compare1 = slice(doc, args[0].m_range.from() / sizeof(ushort), (args[0].m_range.from() + args[0].m_range.length()) / sizeof(ushort));
    } else {
        compare1 = QString::fromStdString(args[0].m_value.data());
    }
    if ( args[1].m_range.isValid() ){
        compare2 = slice(doc, args[1].m_range.from() / sizeof(ushort), (args[1].m_range.from() + args[1].m_range.length())  / sizeof(ushort));
    } else {
        compare2 = QString::fromStdString(args[1].m_value.data());
    }

    return compare1 == compare2;
}

bool LanguageLvHighlighter::predicateEqOr(const std::vector<el::LanguageQuery::PredicateData> &args, void *payload){
    QTextDocument* doc = reinterpret_cast<QTextDocument*>(payload);
    if ( args.size() < 2)
        THROW_EXCEPTION(lv::Exception, "Predicate eq-or? must have at least 2 arguments", Exception::toCode("~Arguments"));

    QString compare1;
    QString compare2;
    if ( args[0].m_range.isValid() ){
        compare1 = slice(doc, args[0].m_range.from() / sizeof(ushort), (args[0].m_range.from() + args[0].m_range.length()) / sizeof(ushort));
    } else {
        compare1 = QString::fromStdString(args[0].m_value.data());
    }

    for (int i = 1; i < args.size(); ++i)
    {
        if ( args[i].m_range.isValid() ){
            compare2 = slice(doc, args[i].m_range.from() / sizeof(ushort), (args[i].m_range.from() + args[i].m_range.length())  / sizeof(ushort));
        } else {
            compare2 = QString::fromStdString(args[i].m_value.data());
        }

        if (compare1 == compare2) return true;
    }

    return false;
}

const char *LanguageLvHighlighter::parsingCallback(void *payload, uint32_t, TSPoint position, uint32_t *bytes_read)
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

void LanguageLvHighlighter::documentChanged(int pos, int removed, int added){

    QTextDocument* doc = static_cast<QTextDocument*>(parent());

    std::vector<std::pair<unsigned, unsigned>> editPoints =
            m_textDocumentData->contentsChange(doc, pos, removed, added);

    uint32_t start = pos*sizeof(ushort)/sizeof(char);
    uint32_t old_end = (pos + removed)*sizeof(ushort)/sizeof(char);
    uint32_t new_end = (pos + added)*sizeof(ushort)/sizeof(char);

    TSInputEdit edit = {start, old_end, new_end,
                        TSPoint{editPoints[0].first, editPoints[0].second},
                        TSPoint{editPoints[1].first, editPoints[1].second},
                        TSPoint{editPoints[2].first, editPoints[2].second}};
    TSInput input = {m_textDocumentData, LanguageLvHighlighter::parsingCallback, TSInputEncodingUTF16};

    m_parser->editParseTree(m_currentAst, edit, input);
}

QList<SyntaxHighlighter::TextFormatRange> LanguageLvHighlighter::highlight(
        int, int position, const QString &text)
{
    QTextDocument* doc = static_cast<QTextDocument*>(parent());
    QList<SyntaxHighlighter::TextFormatRange> ranges;

    if ( !m_currentAst )
        return ranges;

    el::LanguageQuery::Cursor::Ptr cursor = m_languageQuery->exec(m_currentAst, position * sizeof(ushort), (position + text.length())* sizeof(ushort));
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
                }
                else if (name == "comment")
                {
                    r.userstate = 1;
                    if (r.start + r.length == from + length) r.userstateFollows = 0;
                    else r.userstateFollows = 1;
                } else {
                    r.userstate = 0;
                    r.userstateFollows = 0;
                }
                r.format = m_captureToFormatMap[captureId];

                ranges.append(r);
            }
        }
    }

    if (ranges.empty())
    {
        TextFormatRange r;
        r.start = position;
        r.length = text.length();
        r.userstate = 0;
        r.userstateFollows = 0;
        r.format = m_captureToFormatMap[UINT_MAX]; // regular text

        ranges.append(r);
    }


    return ranges;
}

QList<SyntaxHighlighter::TextFormatRange> LanguageLvHighlighter::highlightSections(const QList<ProjectDocumentSection::Ptr> &){
    return QList<SyntaxHighlighter::TextFormatRange>();
}

QString LanguageLvHighlighter::slice(QTextDocument *doc, int from, int to)
{
    QTextBlock block = doc->findBlock(from);
    QTextBlock last = doc->findBlock(to);

    if (block == last) return block.text().mid(from - block.position(), to - from);

    QString text = block.text();
    QString result = text.right(block.position() + text.length() - from) + "\n";
    block = block.next();

    while (block != last)
    {
        result += block.text() + "\n";
        block = block.next();
    }

    text = last.text();
    result += text.left(to - last.position());

    return result;
}

}// namespace
