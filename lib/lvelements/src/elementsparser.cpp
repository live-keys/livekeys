#include "elementsparser.h"
#include "tree_sitter/api.h"
#include "tree_sitter/parser.h"

#include "elementsparserinternal.h"
#include "elementsnodes_p.h"
#include "elementssections_p.h"

#include "live/visuallog.h"

#include <fstream>
#include <queue>

namespace lv{ namespace el{


Parser::ASTRef::ASTRef()
    : m_node(new TSNode){
}

Parser::ASTRef::ASTRef(const Parser::ASTRef &other)
    : m_node(new TSNode(*reinterpret_cast<TSNode*>(other.m_node)))
{
}

Parser::ASTRef::~ASTRef(){
    TSNode* node = reinterpret_cast<TSNode*>(m_node);
    delete node;
}

Parser::ASTRef &Parser::ASTRef::operator =(const Parser::ASTRef &other){
    TSNode* node = reinterpret_cast<TSNode*>(m_node);
    *node = *reinterpret_cast<TSNode*>(other.m_node);
    return *this;
}

SourceRange Parser::ASTRef::range() const{
    TSNode* node = reinterpret_cast<TSNode*>(m_node);
    return SourceRange(ts_node_start_byte(*node), ts_node_end_byte((*node)));
}

uint32_t Parser::ASTRef::childCount() const{
    TSNode* node = reinterpret_cast<TSNode*>(m_node);
    return ts_node_child_count(*node);
}

Parser::ASTRef Parser::ASTRef::childAt(uint32_t index) const{
    TSNode* node = reinterpret_cast<TSNode*>(m_node);
    return Parser::ASTRef(new TSNode(ts_node_child(*node, index)));
}

Parser::ASTRef Parser::ASTRef::parent() const{
    TSNode* node = reinterpret_cast<TSNode*>(m_node);
    return Parser::ASTRef(new TSNode(ts_node_parent(*node)));
}

std::string Parser::ASTRef::typeString() const{
    TSNode* node = reinterpret_cast<TSNode*>(m_node);
    return ts_node_type(*node);
}

Parser::ASTRef::ASTRef(void *node)
    : m_node(node)
{
}

std::string slice(const std::string& source, TSNode& node){
    return source.substr(ts_node_start_byte(node), ts_node_end_byte(node) - ts_node_start_byte(node));
}

Parser::Parser()
    : m_parser(ts_parser_new())
{
    ts_parser_set_language(m_parser, tree_sitter_elements());
}

Parser::~Parser(){
    ts_parser_delete(m_parser);
}

Parser::AST *Parser::parse(const std::string &source) const{
    return reinterpret_cast<Parser::AST*>(ts_parser_parse_string(m_parser, nullptr, source.c_str(), (uint32_t)source.size()));
}

void Parser::destroy(Parser::AST *ast) const{
    if ( ast )
        ts_tree_delete(reinterpret_cast<TSTree*>(ast));
}

Parser::ComparisonResult Parser::compare(
    const std::string &source1, Parser::AST *ast1, const std::string &source2, Parser::AST *ast2)
{
    TSTree* tree1 = reinterpret_cast<TSTree*>(ast1);
    TSTree* tree2 = reinterpret_cast<TSTree*>(ast2);
    std::queue<TSNode> q1;
    std::queue<TSNode> q2;

    q1.push(ts_tree_root_node(tree1));
    q2.push(ts_tree_root_node(tree2));

    while (!q1.empty() && !q2.empty())
    {
        TSNode node1 = q1.front(); q1.pop();
        TSNode node2 = q2.front(); q2.pop();

        if (strcmp(ts_node_type(node1), ts_node_type(node2)) != 0){
            ComparisonResult cr(false);
            cr.m_source1Col = ts_node_start_point(node1).column;
            cr.m_source1Row = ts_node_start_point(node1).row;
            cr.m_source1Offset = ts_node_start_byte(node1);

            cr.m_source2Col = ts_node_start_point(node2).column;
            cr.m_source2Row = ts_node_start_point(node2).row;
            cr.m_source2Offset = ts_node_start_byte(node2);

            return cr;
        }

        if (strcmp(ts_node_type(node1), "identifier") == 0 ||
            strcmp(ts_node_type(node1), "property_identifier") == 0)
        {
            if ( strcmp(slice(source1, node1).c_str(), slice(source2, node2).c_str()) != 0 ){
                ComparisonResult cr(false);
                cr.m_source1Col = ts_node_start_point(node1).column;
                cr.m_source1Row = ts_node_start_point(node1).row;
                cr.m_source1Offset = ts_node_start_byte(node1);

                cr.m_source2Col = ts_node_start_point(node2).column;
                cr.m_source2Row = ts_node_start_point(node2).row;
                cr.m_source2Offset = ts_node_start_byte(node2);

                return cr;
            }
        }

        if (strcmp(ts_node_type(node1), "string") == 0 ||
            strcmp(ts_node_type(node1), "number") == 0)
        {
            if ( strcmp(slice(source1, node1).c_str(), slice(source2, node2).c_str()) != 0 ){
                ComparisonResult cr(false);
                cr.m_source1Col = ts_node_start_point(node1).column;
                cr.m_source1Row = ts_node_start_point(node1).row;
                cr.m_source1Offset = ts_node_start_byte(node1);

                cr.m_source2Col = ts_node_start_point(node2).column;
                cr.m_source2Row = ts_node_start_point(node2).row;
                cr.m_source2Offset = ts_node_start_byte(node2);

                return cr;
            }
        }

        for (uint32_t i = 0; i < ts_node_child_count(node1); ++i)
            if ( strcmp( ts_node_type(ts_node_child(node1, i)), "comment") != 0 )
                q1.push(ts_node_child(node1, i));

        for (uint32_t i = 0; i < ts_node_child_count(node2); ++i)
            if ( strcmp( ts_node_type(ts_node_child(node2, i)), "comment") != 0 )
                q2.push(ts_node_child(node2, i));

        if (q1.size() != q2.size()){
            ComparisonResult cr(false);
            cr.m_source1Col = ts_node_start_point(node1).column;
            cr.m_source1Row = ts_node_start_point(node1).row;
            cr.m_source1Offset = ts_node_start_byte(node1);

            cr.m_source2Col = ts_node_start_point(node2).column;
            cr.m_source2Row = ts_node_start_point(node2).row;
            cr.m_source2Offset = ts_node_start_byte(node2);

            return cr;
        }
    }

    return ComparisonResult(true);
}

std::string Parser::toString(Parser::AST *ast){
    char* str = ts_node_string(ts_tree_root_node(reinterpret_cast<TSTree*>(ast)));
    std::string result(str);
    free(str);
    return result;
}

std::string Parser::toJs(const std::string &contents) const{
    Parser::AST* ast = parse(contents);

    std::string result = toJs(contents, ast);

    destroy(ast);

    return result;
}

std::string Parser::toJs(const std::string& contents, AST *ast) const{
    if ( !ast )
        return nullptr;

    std::string result;

    BaseNode* root = el::BaseNode::visit(ast);

    el::JSSection* section = new el::JSSection;
    section->from = 0;
    section->to   = static_cast<int>(contents.size());

    root->convertToJs(contents, section->m_children);

    std::vector<std::string> flatten;
    section->flatten(contents, flatten);

    for ( const std::string& s : flatten ){
        result += s;
    }

    delete section;
    delete root;

    return result;
}

std::list<std::string> Parser::parseExportNames(const std::string& moduleFile){
    if ( moduleFile.rfind(".lv.js") != std::string::npos ){
        return parseExportNamesJs(moduleFile);
    }

    std::list<std::string> exportNames;
    return exportNames;
}

std::list<std::string> Parser::parseExportNamesJs(const std::string &jsModuleFile){
    std::ifstream instream(jsModuleFile, std::ifstream::in | std::ifstream::binary);
    if ( !instream.is_open() ){
        THROW_EXCEPTION(lv::Exception, std::string("Cannot open file: ") + jsModuleFile, Exception::toCode("~Path"));
    }

    instream.seekg(0, std::ios::end);
    size_t size = instream.tellg();
    std::string buffer(size, ' ');
    instream.seekg(0);
    instream.read(&buffer[0], size);

    std::list<std::string> exportNames;

    //TODO: Quick hack for testing purposes, this will need implementation
    size_t start = 0;
    while ((start = buffer.find("module.exports[\"", start)) != std::string::npos) {
        size_t end = buffer.find("\"]", start);
        if ( end != std::string::npos ){
            exportNames.push_back(buffer.substr(start + 16, end - start - 16));
        }
        start = end + 1;
    }

    return exportNames;
}

Parser::ComparisonResult::ComparisonResult(bool isEqual)
    : m_isEqual(isEqual)
    , m_source1Col(0)
    , m_source1Row(0)
    , m_source1Offset(0)
    , m_source2Col(0)
    , m_source2Row(0)
    , m_source2Offset(0)
{
}

}} // namespace lv, el
