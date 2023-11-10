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

#include "languagequery.h"
#include "live/visuallog.h"
#include "tree_sitter/api.h"

namespace lv{ namespace el{

// LanguageQueryException
// -----------------------------------------------------------------------------

LanguageQueryException::LanguageQueryException(
        const Utf8& message,
        uint32_t offset,
        lv::Exception::Code code,
        lv::Exception::SourceTrace& st)
    : Exception(message, code, st)
    , m_offset(offset)
{
}

// LanguageQuery::Cursor
// -----------------------------------------------------------------------------

bool LanguageQuery::Cursor::nextMatch(){
    TSQueryCursor* cursor = reinterpret_cast<TSQueryCursor*>(m_cursor);
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    return ts_query_cursor_next_match(cursor, currentMatch);
}

uint16_t LanguageQuery::Cursor::totalMatchCaptures() const{
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    return currentMatch->capture_count;
}

uint16_t LanguageQuery::Cursor::matchPatternIndex() const{
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    return currentMatch->pattern_index;
}

SourceRange LanguageQuery::Cursor::captureRange(uint16_t captureIndex){
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    TSNode node = currentMatch->captures[captureIndex].node;

    uint32_t start = ts_node_start_byte(node);
    uint32_t end   = ts_node_end_byte(node);

    return SourceRange(start, end - start);
}

uint32_t LanguageQuery::Cursor::captureId(uint16_t captureIndex){
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    return currentMatch->captures[captureIndex].index;
}

LanguageQuery::Cursor::Cursor()
    : m_cursor(ts_query_cursor_new())
    , m_currentMatch(new TSQueryMatch)
{
}

LanguageQuery::Cursor::~Cursor(){
    TSQueryCursor* cursor = reinterpret_cast<TSQueryCursor*>(m_cursor);
    TSQueryMatch* currentMatch = reinterpret_cast<TSQueryMatch*>(m_currentMatch);
    ts_query_cursor_delete(cursor);
    delete currentMatch;
}


// LanguageQuery
// -----------------------------------------------------------------------------

LanguageQuery::Ptr LanguageQuery::create(LanguageParser::Language* language, const std::string &queryString){
    uint32_t errorOffset;
    TSQueryError errorType;

    TSQuery * query = ts_query_new(
      reinterpret_cast<const TSLanguage*>(language),
      queryString.c_str(),
      static_cast<uint32_t>(queryString.size()),
      &errorOffset,
      &errorType
    );

    if ( errorType != TSQueryErrorNone ){
        ts_query_delete(query);
        throw LanguageQueryException("Language query error.", 0, errorType, SOURCE_TRACE());
    }

    return LanguageQuery::Ptr(new LanguageQuery(static_cast<void*>(query)));
}

LanguageQuery::~LanguageQuery(){
    TSQuery* query = reinterpret_cast<TSQuery*>(m_query);
    ts_query_delete(query);
}

uint32_t LanguageQuery::captureCount() const{
    TSQuery* query = reinterpret_cast<TSQuery*>(m_query);
    return ts_query_capture_count(query);
}

std::string LanguageQuery::captureName(uint32_t captureIndex) const{
    TSQuery* query = reinterpret_cast<TSQuery*>(m_query);

    uint32_t captureLength;
    const char* captureName = ts_query_capture_name_for_id(query, captureIndex, &captureLength);
    return std::string(captureName, captureLength);
}

LanguageQuery::Cursor::Ptr LanguageQuery::exec(LanguageParser::AST *ast){
    TSTree* tree = reinterpret_cast<TSTree*>(ast);
    TSNode root = ts_tree_root_node(tree);

    TSQuery* query = reinterpret_cast<TSQuery*>(m_query);

    Cursor::Ptr cursor(new Cursor);
    TSQueryCursor* cursorInternal = reinterpret_cast<TSQueryCursor*>(cursor->m_cursor);

    ts_query_cursor_exec(cursorInternal, query, root);
    return cursor;
}

LanguageQuery::Cursor::Ptr LanguageQuery::exec(LanguageParser::AST *ast, uint32_t start, uint32_t end){
    TSTree* tree = reinterpret_cast<TSTree*>(ast);
    TSNode root = ts_tree_root_node(tree);

    TSQuery* query = reinterpret_cast<TSQuery*>(m_query);

    Cursor::Ptr cursor(new Cursor);
    TSQueryCursor* cursorInternal = reinterpret_cast<TSQueryCursor*>(cursor->m_cursor);

    ts_query_cursor_set_byte_range(cursorInternal, start, end);
    ts_query_cursor_exec(cursorInternal, query, root);

    return cursor;
}

bool LanguageQuery::predicateMatch(const Cursor::Ptr &cursor, void *payload){
    TSQuery* query = reinterpret_cast<TSQuery*>(m_query);
    uint32_t length;
    const TSQueryPredicateStep* step = ts_query_predicates_for_pattern(query, cursor->matchPatternIndex(), &length);
    if ( length == 0 )
        return true;

    std::string functionToCall;
    uint32_t strLen = 0;

    std::vector<LanguageQuery::PredicateData> args;

    for( uint32_t i = 0; i < length; ++i ){
        if ( functionToCall.empty() ){
            functionToCall = ts_query_string_value_for_id(query, step[i].value_id, &strLen);
        } else if ( step[i].type == TSQueryPredicateStepTypeString ){
            LanguageQuery::PredicateData pd;
            pd.m_value = ts_query_string_value_for_id(query, step[i].value_id, &strLen);
            args.push_back(pd);
        } else if ( step[i].type == TSQueryPredicateStepTypeCapture ){
            uint16_t captures = cursor->totalMatchCaptures();
            for ( uint16_t captureIndex = 0; captureIndex < captures; ++captureIndex ){
                uint32_t captureId = cursor->captureId(captureIndex);
                if ( captureId == step[i].value_id ){
                    LanguageQuery::PredicateData pd;
                    pd.m_range = cursor->captureRange(captureIndex);
                    args.push_back(pd);
                    break;
                }
            }
        } else if ( step[i].type == TSQueryPredicateStepTypeDone ){
            auto it = m_predicates.find(functionToCall);
            if ( it == m_predicates.end() ){
                THROW_EXCEPTION(Exception, "LanguageQuery: Failed to find function \'" + functionToCall + "\'", Exception::toCode("~Function"));
            }
            if ( !it->second(args, payload) )
                return false;
            functionToCall = "";
        }
    }

    return true;
}

void LanguageQuery::addPredicate(const std::string &name, std::function<bool (const std::vector<LanguageQuery::PredicateData> &, void *)> callback){
    m_predicates[name] = callback;
}

LanguageQuery::LanguageQuery(void *query)
    : m_query(query)
{
}

}}// namespace lv, el
