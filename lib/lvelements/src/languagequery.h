#ifndef LVLANGUAGEQUERY_H
#define LVLANGUAGEQUERY_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/parser.h"

#include "live/exception.h"

#include <memory>

namespace lv{ namespace el{

class LV_ELEMENTS_EXPORT LanguageQueryException : public Exception{

public:
    LanguageQueryException(const std::string& message, uint32_t offset, lv::Exception::Code code = 0);
    uint32_t offset() const{ return m_offset; }

private:
    uint32_t m_offset;
};

class LV_ELEMENTS_EXPORT LanguageQuery{

public:
    class LV_ELEMENTS_EXPORT Cursor{

    public:
        friend class LanguageQuery;
        typedef std::shared_ptr<Cursor> Ptr;

        bool nextMatch();

        uint16_t totalMatchCaptures() const;
        uint16_t matchPatternIndex() const;
        SourceRange captureRange(uint16_t captureIndex);
        uint32_t captureId(uint16_t captureIndex);

        ~Cursor();

    private:
        DISABLE_COPY(Cursor);

        Cursor();

        void* m_cursor;
        void* m_currentMatch;
    };

    typedef std::shared_ptr<LanguageQuery>       Ptr;
    typedef std::shared_ptr<const LanguageQuery> ConstPtr;

public:
    static LanguageQuery::Ptr create(const Parser& parser, const std::string& query);
    ~LanguageQuery();

    uint32_t captureCount() const;
    std::string captureName(uint32_t captureIndex) const;

    Cursor::Ptr exec(Parser::AST* ast);
    Cursor::Ptr exec(Parser::AST* ast, uint32_t start, uint32_t end);

private:
    DISABLE_COPY(LanguageQuery);

    LanguageQuery(void* query);
    void* m_query;
};

}}// namespace lv, el

#endif // LVLANGUAGEQUERY_H
