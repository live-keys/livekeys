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

#ifndef LVLANGUAGEQUERY_H
#define LVLANGUAGEQUERY_H

#include "live/elements/compiler/lvelcompilerglobal.h"
#include "live/elements/compiler/languageparser.h"

#include "live/exception.h"

#include <memory>
#include <map>
#include <functional>

namespace lv{ namespace el{

class LV_ELEMENTS_COMPILER_EXPORT LanguageQueryException : public Exception{

public:
    LanguageQueryException(
            const Utf8& message,
            uint32_t offset,
            lv::Exception::Code code = 0,
            const lv::Exception::SourceTrace& st = lv::Exception::SourceTrace());
    uint32_t offset() const{ return m_offset; }

private:
    uint32_t m_offset;
};

class LV_ELEMENTS_COMPILER_EXPORT LanguageQuery{

public:
    class LV_ELEMENTS_COMPILER_EXPORT Cursor{

    public:
        friend class LanguageQuery;
        typedef std::shared_ptr<Cursor> Ptr;

        bool nextMatch();

        uint16_t totalMatchCaptures() const;
        uint16_t matchPatternIndex() const;
        SourceRange captureRange(uint16_t captureIndex);
        uint32_t captureId(uint16_t captureIndex);

        void temp();

        ~Cursor();

    private:
        DISABLE_COPY(Cursor);

        Cursor();

        void* m_cursor;
        void* m_currentMatch;
    };

    class LV_ELEMENTS_COMPILER_EXPORT PredicateData{
    public:
        SourceRange m_range;
        Utf8        m_value;
    };

    typedef std::shared_ptr<LanguageQuery>       Ptr;
    typedef std::shared_ptr<const LanguageQuery> ConstPtr;

public:
    static LanguageQuery::Ptr create(LanguageParser::Language *, const std::string& query);
    ~LanguageQuery();

    uint32_t captureCount() const;
    std::string captureName(uint32_t captureIndex) const;

    Cursor::Ptr exec(LanguageParser::AST* ast);
    Cursor::Ptr exec(LanguageParser::AST* ast, uint32_t start, uint32_t end);

    bool predicateMatch(const Cursor::Ptr& cursor, void* payload = nullptr);

    void addPredicate(const std::string& name, std::function<bool(const std::vector<PredicateData>&, void* payload)> callback);

private:
    DISABLE_COPY(LanguageQuery);
    LanguageQuery(void* query);

    std::map<std::string, std::function<bool(const std::vector<PredicateData>&, void* payload)> > m_predicates;

    void* m_query;
};

}}// namespace lv, el

#endif // LVLANGUAGEQUERY_H
