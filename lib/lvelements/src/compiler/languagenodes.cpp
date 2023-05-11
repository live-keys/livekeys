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

#include "languagenodes_p.h"
#include "propertybindingcontainer_p.h"
#include "live/visuallog.h"
#include "live/stacktrace.h"

#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include <string.h>

namespace lv{ namespace el{

std::string indent(int i){
    std::string res;
    res.append(4 * i, ' ');
    return res;
}

std::string BaseNode::ConversionContext::baseComponentName(BaseNode::ConversionContext *ctx){
    return ctx && !ctx->baseComponent.empty() ? ctx->baseComponent : "Element";
}

std::string BaseNode::ConversionContext::baseComponentImport(BaseNode::ConversionContext *ctx){
    return ctx ? ctx->baseComponentImportUri : "";
}

bool BaseNode::ConversionContext::isImplicitType(BaseNode::ConversionContext *ctx, const std::string &type){
    if ( !ctx ){
        if ( type == "Element" )
            return true;
        return false;
    }

    if ( !ctx->baseComponent.empty() && ctx->baseComponent == type ){
        return true;
    } else if ( ctx->baseComponent.empty() ){
        if ( type == "Element" )
            return true;
    }

    for ( auto it = ctx->implicitTypes.begin(); it != ctx->implicitTypes.end(); ++it ){
        if ( *it == type )
            return true;
    }

    return false;
}

BaseNode::BaseNode(const TSNode &node, const std::string &typeString)
    : m_parent(nullptr)
    , m_node(node)
    , m_typeString(typeString)
{}

BaseNode::~BaseNode(){
    for ( auto it = m_children.begin(); it != m_children.end(); ++it ){
        delete *it;
    }
}

BaseNode *BaseNode::visit(const std::string &filePath, const std::string &fileName, LanguageParser::AST *ast){
    TSTree* tree = reinterpret_cast<TSTree*>(ast);
    TSNode root_node = ts_tree_root_node(tree);

    ProgramNode* node = new ProgramNode(root_node);
    node->setFileName(fileName);
    node->setFilePath(filePath);

    uint32_t count = ts_node_child_count(root_node);

    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(root_node, i);
        visit(node, child);
    }

    return node;
}

bool BaseNode::checkIdentifierDeclared(const std::string& source, BaseNode *node, std::string id, ConversionContext *ctx){
    if (id == "this" || id == "parent" || id == "import" )
        return true;
    if ( ConversionContext::isImplicitType(ctx, id) )
        return true;

    BaseNode* prog = nullptr;
    while (node){
        JsBlockNode* hasIds = dynamic_cast<JsBlockNode*>(node);

        if (!node->parent()){
            prog = node;
        }

        if (hasIds){
            for (auto it = hasIds->m_declarations.begin(); it != hasIds->m_declarations.end(); ++it){
                std::string declaredId = slice(source, *it);
                if (declaredId == id){
                    return true;
                }
            }
        }

        node = node->parent();
    }

    return false;
}

const std::vector<BaseNode *>& BaseNode::children() const{
    return m_children;
}

int BaseNode::startByte() const{
    return ts_node_start_byte(m_node);
}

int BaseNode::endByte() const{
    return ts_node_end_byte(m_node);
}

std::pair<int, int> BaseNode::startPoint() const{
    TSPoint tpnt = ts_node_start_point(m_node);
    return std::make_pair(tpnt.row, tpnt.column);
}

std::pair<int, int> BaseNode::endPoint() const{
    TSPoint tpnt = ts_node_end_point(m_node);
    return std::make_pair(tpnt.row, tpnt.column);
}

std::string BaseNode::rangeString() const{
    return std::string("[") + std::to_string(startByte()) + ", " + std::to_string(endByte()) + "]";
}

std::string BaseNode::pointRangeString() const{
    auto sp = startPoint();
    auto ep = endPoint();
    return std::string("[") + std::to_string(sp.first) + ":" + std::to_string(sp.second) + ", " + std::to_string(ep.first) + ":" + std::to_string(ep.second) + "]";
}

void BaseNode::convertToJs(
        const std::string &source,
        std::vector<ElementsInsertion *> &sections,
        int indentValue,
        ConversionContext *ctx)

{
    for ( BaseNode* node : m_children ){
        node->convertToJs(source, sections, indentValue, ctx);
    }
}

void BaseNode::collectImports(const std::string &source, std::vector<IdentifierNode*> &imp, ConversionContext *ctx){
    for ( BaseNode* node : m_children ){
        node->collectImports(source, imp, ctx);
    }
}

std::string BaseNode::slice(const std::string &source, uint32_t start, uint32_t end){
    return source.substr(start, end - start);
}

std::string BaseNode::slice(const std::string &source, BaseNode *node){
    return slice(source, node->startByte(), node->endByte());
}

JsBlockNode* BaseNode::addToDeclarations(BaseNode *parent, IdentifierNode *idNode){
    BaseNode* p = parent;
    while (p){
        JsBlockNode* jsbn = dynamic_cast<JsBlockNode*>(p);
        if (jsbn){
            jsbn->m_declarations.push_back(idNode);
            return jsbn;
            break;
        }
        p = p->parent();
    }
    return nullptr;
}

JsBlockNode *BaseNode::addUsedIdentifier(BaseNode *parent, IdentifierNode *idNode){
    BaseNode* p = parent;
    while (p){
        JsBlockNode* jsbn = dynamic_cast<JsBlockNode*>(p);
        if (jsbn){
            jsbn->m_usedIdentifiers.push_back(idNode);
            return jsbn;
        }
        p = p->parent();
    }

    return nullptr;
}

void BaseNode::assertValid(BaseNode *from, const TSNode &node, const std::string& message){
    if ( ts_node_is_null(node) ){
        SyntaxException se = CREATE_EXCEPTION(SyntaxException, "Syntax error: " + message, Exception::toCode("~LanguageNodes"));
        BaseNode* p = from;
        while (p && p->typeString() != "Program")
            p = p->parent();

        std::string fileName = p && p->typeString() == "Program" ? static_cast<ProgramNode*>(p)->m_filePath : "";
        int line = from ? ts_node_start_point(from->current()).row : -2;
        int column = from ? ts_node_start_point(from->current()).column : -2;
        int startByte = from ? ts_node_start_byte(from->current()) : -1;

        se.setParseLocation(line + 1, column + 1, startByte, fileName);
        throw se;
    }
    assertError(from, node, message);
}

void BaseNode::assertError(BaseNode *from, const TSNode &node, const std::string &message){
    if ( strcmp(ts_node_type(node), "ERROR") == 0 ){
        SyntaxException se = CREATE_EXCEPTION(SyntaxException, "Syntax error: " + message, Exception::toCode("~LanguageNodes"));
        BaseNode* p = from;
        while (p && p->typeString() != "Program")
            p = p->parent();

        std::string fileName = p && p->typeString() == "Program" ? static_cast<ProgramNode*>(p)->m_filePath : "";
        se.setParseLocation(ts_node_start_point(node).row + 1, ts_node_start_point(node).column + 1, ts_node_start_byte(node), fileName);
        throw se;
    }
}

void BaseNode::throwError(BaseNode *from, const TSNode &node, const std::string &message){
    SyntaxException se = CREATE_EXCEPTION(SyntaxException, "Syntax error: " + message, Exception::toCode("~LanguageNodes"));
    BaseNode* p = from;
    while (p && p->typeString() != "Program")
        p = p->parent();

    std::string fileName = p && p->typeString() == "Program" ? static_cast<ProgramNode*>(p)->m_filePath : "";
    se.setParseLocation(ts_node_start_point(node).row + 1, ts_node_start_point(node).column + 1, ts_node_start_byte(node), fileName);
    throw se;
}

TSNode BaseNode::nodeChildByFieldName(const TSNode &node, const std::string &name){
    return ts_node_child_by_field_name(node, name.c_str(), static_cast<uint32_t>(name.length()));
}

std::vector<IdentifierNode *> BaseNode::fromNestedIdentifier(BaseNode *parent, const TSNode &node){
    std::vector<IdentifierNode *> result;
    if ( strcmp(ts_node_type(node), "identifier") == 0 ){
        result.push_back(new IdentifierNode(node));
    } else if ( strcmp(ts_node_type(node), "nested_identifier") == 0 ){
        uint32_t count = ts_node_child_count(node);
        for ( uint32_t i = 0; i < count; ++i ){
            TSNode child = ts_node_child(node, i);
            assertError(parent, child, "Expected identifier.");
            if ( strcmp(ts_node_type(child), "identifier") == 0 ){
                result.push_back(new IdentifierNode(child));
            } else if ( strcmp(ts_node_type(child), "nested_identifier") == 0 ){
                auto nestedResult = fromNestedIdentifier(parent, child);
                result.insert( result.end(), nestedResult.begin(), nestedResult.end() );
            } else if ( (strcmp(ts_node_type(child), ".") != 0) ){
                throwError(parent, child, "Expected identifier.");
            }
        }
    }

    return result;
}

ParameterListNode *BaseNode::scanFormalParameters(BaseNode *parent, const TSNode& formalParameters){
    ParameterListNode* result = new ParameterListNode(formalParameters);

    uint32_t paramterCount = ts_node_child_count(formalParameters);

    for (uint32_t pc = 0; pc < paramterCount; ++pc){
        TSNode ftpc = ts_node_child(formalParameters, pc);
        assertError(parent, ftpc, "Function declaration not supported.");
        if (strcmp(ts_node_type(ftpc), "identifier") == 0){
            auto nameNode = new IdentifierNode(ftpc);
            auto paramNode = new ParameterNode(ftpc, nameNode);
            result->m_parameters.push_back(paramNode);
        } else if ( strcmp(ts_node_type(ftpc), "required_parameter") == 0 ){
            uint32_t paramterInteralCount = ts_node_child_count(ftpc);
            for (uint32_t pi = 0; pi < paramterInteralCount; ++pi){
                TSNode ftpci = ts_node_child(ftpc, pi);
                assertError(parent, ftpci, "Function declaration not supported.");
                if (strcmp(ts_node_type(ftpci), "identifier") == 0){
                    auto nameNode = new IdentifierNode(ftpci);
                    auto paramNode = new ParameterNode(ftpc, nameNode);
                    result->m_parameters.push_back(paramNode);
                }
            }
        } else if ( strcmp(ts_node_type(ftpc), "optional_parameter") == 0 ){
            uint32_t paramterInteralCount = ts_node_child_count(ftpc);
            for (uint32_t pi = 0; pi < paramterInteralCount; ++pi){
                TSNode ftpci = ts_node_child(ftpc, pi);
                assertError(parent, ftpci, "Function declaration not supported.");
                if (strcmp(ts_node_type(ftpci), "identifier") == 0){
                    auto nameNode = new IdentifierNode(ftpci);
                    auto paramNode = new ParameterNode(ftpc, nameNode);
                    result->m_parameters.push_back(paramNode);
                }
            }
        }
    }
    return result;
}

ParameterListNode *BaseNode::scanFormalTypeParameters(BaseNode *parent, const TSNode &parameters){
    auto result = new ParameterListNode(parameters);
    uint32_t parameterCount = ts_node_child_count(parameters);

    for (uint32_t pc = 0; pc < parameterCount; ++pc){
        TSNode ftpc = ts_node_child(parameters, pc);
        assertError(parent, ftpc, "Function parameter not declared property.");
        if (strcmp(ts_node_type(ftpc), "formal_type_parameter") == 0){

            if ( ts_node_child_count(ftpc) > 0 ){
                TSNode typeParameter = ts_node_child(ftpc, 0);

                std::string paramKey = "name";
                TSNode parameterName = ts_node_child_by_field_name(typeParameter, paramKey.c_str(), paramKey.length());
                assertValid(parent, parameterName, "Parameter name is null.");
                paramKey = "type";
                TSNode parameterType = ts_node_child_by_field_name(typeParameter, paramKey.c_str(), paramKey.length());
                assertValid(parent, parameterType, "Parameter type is null.");

                if ( strcmp(ts_node_type(typeParameter), "required_type_parameter") == 0 ){
                    result->m_parameters.push_back(
                        new ParameterNode(typeParameter, new IdentifierNode(parameterName), new TypeNode(parameterType), false)
                    );
                } else if ( strcmp(ts_node_type(typeParameter), "optional_type_parameter") == 0){
                    result->m_parameters.push_back(
                        new ParameterNode(typeParameter, new IdentifierNode(parameterName), new TypeNode(parameterType), true)
                    );
                }
            }
        }
    }
    return result;
}

void BaseNode::addChild(BaseNode *child){
    m_children.push_back(child);
    child->setParent(this);
}

std::string BaseNode::astString() const{
    char* str = ts_node_string(m_node);
    std::string result(str);
    free(str);
    return result;
}

std::string BaseNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += m_typeString + " [" +
            std::to_string(ts_node_start_byte(m_node)) + ", " +
            std::to_string(ts_node_end_byte(m_node)) + "]\n";

    for (BaseNode* child : m_children){
        result += child->toString(indent < 0 ? indent : indent + 1);
    }

    return result;
}

void BaseNode::visit(BaseNode *parent, const TSNode &node){
    if ( strcmp(ts_node_type(node), "import_statement") == 0 ){
        visitImport(parent, node);
    } else if ( strcmp(ts_node_type(node), "js_import_statement") == 0 ){
        visitJsImport(parent, node);
    } else if ( strcmp(ts_node_type(node), "identifier") == 0 ){
        visitIdentifier(parent, node);
    } else if ( strcmp(ts_node_type(node), "constructor_definition") == 0 ){
        visitConstructorDefinition(parent, node);
    } else if ( strcmp(ts_node_type(node), "property_identifier") == 0 ){
        visitPropertyIdentifier(parent, node);
    } else if ( strcmp(ts_node_type(node), "this") == 0 ){
        visitIdentifier(parent, node);
    } else if ( strcmp(ts_node_type(node), "import_path") == 0 ){
        visitImportPath(parent, node);
    } else if ( strcmp(ts_node_type(node), "component") == 0 ){
        visitComponentDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "component_declaration") == 0 ){
        visitComponentDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "component_instance_statement") == 0 ){
        visitComponentInstanceStatement(parent, node);
    } else if ( strcmp(ts_node_type(node), "new_component_expression") == 0 ){
        visitNewComponentExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "arrow_function") == 0 ){
        visitArrowFunction(parent, node);
    } else if ( strcmp(ts_node_type(node), "component_body") == 0 ){
        visitComponentBody(parent, node);
    } else if ( strcmp(ts_node_type(node), "new_component_body") == 0 ){
        visitComponentBody(parent, node);
    } else if ( strcmp(ts_node_type(node), "class_declaration") == 0 ){
        visitClassDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "property_declaration") == 0 ){
        visitPropertyDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "static_property_declaration") == 0 ){
        visitStaticPropertyDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "member_expression") == 0 ){
        visitMemberExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "subscript_expression") == 0 ){
        visitSubscriptExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "identifier_property_assignment") == 0 ){
        visitIdentifierAssignment(parent, node);
    } else if ( strcmp(ts_node_type(node), "property_assignment") == 0 ){
        visitPropertyAssignment(parent, node);
    } else if ( strcmp(ts_node_type(node), "event_declaration") == 0 ){
        visitEventDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "listener_declaration") == 0 ){
        visitListenerDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "method_definition") == 0 ){
        visitMethodDefinition(parent, node);
    } else if ( strcmp(ts_node_type(node), "typed_method_declaration") == 0 ){
        visitTypedMethodDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "property_accessor_declaration") == 0 ){
        visitPropertyAccessorDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "function_declaration") == 0 ){
        visitFunctionDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "function") == 0 ){
        visitFunction(parent, node);
    } else if ( strcmp(ts_node_type(node), "number") == 0 ){
        visitNumber(parent, node);
    } else if ( strcmp(ts_node_type(node), "expression_statement") == 0 ){
        visitExpressionStatement(parent, node);
    } else if ( strcmp(ts_node_type(node), "assignment_expression") == 0 ){
        visitAssignmentExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "call_expression") == 0 ){
        visitCallExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "new_tagged_component_expression") == 0 ){
        visitNewTaggedComponentExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "tagged_type_string") == 0 ){
        visitTaggedString(parent, node);
    } else if ( strcmp(ts_node_type(node), "new_tripple_tagged_component_expression") == 0 ){
        visitNewTrippleTaggedComponentExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "tripple_tagged_type_string") == 0 ){
        visitTrippleTaggedString(parent, node);
    } else if ( strcmp(ts_node_type(node), "variable_declaration") == 0 ){
        visitVariableDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "lexical_declaration") == 0 ){
        visitLexicalDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "array_pattern") == 0 ){
        visitDestructuringPattern(parent, node);
    } else if ( strcmp(ts_node_type(node), "object_pattern") == 0 ){
        visitDestructuringPattern(parent, node);
    } else if ( strcmp(ts_node_type(node), "new_expression") == 0 ){
        visitNewExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "return_statement") == 0 ){
        visitReturnStatement(parent, node);
    } else if ( strcmp(ts_node_type(node), "object") == 0 ){
        visitObject(parent, node);
    } else if ( strcmp(ts_node_type(node), "ERROR") == 0 ){
        SyntaxException se = CREATE_EXCEPTION(SyntaxException, "Syntax error.", Exception::toCode("~LanguageNodes"));
        BaseNode* p = parent;
        while (p && p->typeString() != "Program")
            p = p->parent();

        std::string fileName = p && p->typeString() == "Program" ? static_cast<ProgramNode*>(p)->m_filePath : "";
        se.setParseLocation(ts_node_start_point(node).row + 1, ts_node_start_point(node).column + 1, ts_node_start_byte(node), fileName);

        throw se;
    } else {
        visitChildren(parent, node);
    }
}

void BaseNode::visitChildren(BaseNode *parent, const TSNode &node){
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        visit(parent, child);
    }
}

void BaseNode::visitImport(BaseNode *parent, const TSNode &node){
    ImportNode* importNode = new ImportNode(node);
    parent->addChild(importNode);
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "import_as" ) == 0 ){
            TSNode aliasChild = ts_node_child(child, 1);
            IdentifierNode* in = new IdentifierNode(aliasChild);
            importNode->m_importAs = in;
            importNode->addChild(in);
        } else if ( strcmp(ts_node_type(child), "import_path") == 0 ){
            visitImportPath(importNode, child);
        }
    }
}

void BaseNode::visitJsImport(BaseNode *parent, const TSNode &node){
    JsImportNode* importNode = new JsImportNode(node);
    parent->addChild(importNode);
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "identifier" ) == 0 ){
            IdentifierNode* name = new IdentifierNode(child);
            importNode->m_importNames.push_back(name);
            importNode->addChild(name);
            addToDeclarations(parent, name);
        } else if ( strcmp(ts_node_type(child), "string" ) == 0 ){
            StringNode* path = new StringNode(child);
            importNode->m_importPath = path;
            importNode->addChild(path);
        }
    }
}

void BaseNode::visitIdentifier(BaseNode *parent, const TSNode &node){
    IdentifierNode* identifierNode = new IdentifierNode(node);
    parent->addChild(identifierNode);
    addUsedIdentifier(parent, identifierNode);
    visitChildren(identifierNode, node);
}

void BaseNode::visitPropertyIdentifier(BaseNode *parent, const TSNode &node){
    IdentifierNode* identifierNode = new IdentifierNode(node);
    parent->addChild(identifierNode);
    visitChildren(identifierNode, node);
}

void BaseNode::visitImportPath(BaseNode *parent, const TSNode &node){
    ImportPathNode* ipnode = new ImportPathNode(node);
    uint32_t count = ts_node_child_count(node);

    TSNode n = ts_node_child_by_field_name(node, "relative", 8);
    if ( !ts_node_is_null(n) )
        ipnode->m_isRelative = true;

    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "import_path_segment") == 0 ){
            ImportPathSegmentNode* in = new ImportPathSegmentNode(child);
            ipnode->m_segments.push_back(in);
            ipnode->addChild(in);
        }
    }
    parent->addChild(ipnode);
}

void BaseNode::visitComponentDeclaration(BaseNode *parent, const TSNode &node){
    ComponentDeclarationNode* enode = new ComponentDeclarationNode(node);
    parent->addChild(enode);


    TSNode name = nodeChildByFieldName(node, "name");
    if ( !ts_node_is_null(name) ){
        enode->m_name = new IdentifierNode(name);
        enode->addChild(enode->m_name);
        addToDeclarations(parent, enode->m_name);
    }

    TSNode heritage = nodeChildByFieldName(node, "heritage");
    if ( !ts_node_is_null(heritage) ){
        if ( strcmp(ts_node_type(heritage), "component_heritage") == 0 ){
            uint32_t heritageCount = ts_node_child_count(heritage);
            for ( uint32_t j = 0; j < heritageCount; ++j ){
                TSNode heritageSegment = ts_node_child(heritage, j);
                if ( strcmp(ts_node_type(heritageSegment), "identifier" ) == 0 ){
                    IdentifierNode* heritageSegmentNode = new IdentifierNode(heritageSegment);
                    enode->m_heritage.push_back(heritageSegmentNode);
                    enode->addChild(heritageSegmentNode);
                }
            }
        } else if ( strcmp(ts_node_type(heritage), "component_short_heritage") == 0 ){
            uint32_t heritageCount = ts_node_child_count(heritage);
            for ( uint32_t j = 0; j < heritageCount; ++j ){
                TSNode heritageSegment = ts_node_child(heritage, j);
                if ( strcmp(ts_node_type(heritageSegment), "identifier" ) == 0 ){
                    IdentifierNode* heritageSegmentNode = new IdentifierNode(heritageSegment);
                    enode->m_heritage.push_back(heritageSegmentNode);
                    enode->addChild(heritageSegmentNode);
                }
            }
        }

        if ( enode->m_heritage.size() > 0 ){
            addUsedIdentifier(enode, enode->m_heritage[0]);
        }
    }


    TSNode identifier = nodeChildByFieldName(node, "id");
    if ( !ts_node_is_null(identifier) ){
        if ( ts_node_named_child_count(identifier) > 0 ){
            TSNode idChild = ts_node_child(identifier, 1);
            enode->m_id = new IdentifierNode(idChild);
            enode->addChild(enode->m_id);
            addToDeclarations(parent, enode->m_id);
        }
    }

    TSNode body = nodeChildByFieldName(node, "body");
    assertValid(parent, body, "Component declaration body is null.");
    enode->m_body = new ComponentBodyNode(body);
    enode->addChild(enode->m_body);
    visitChildren(enode->m_body, body);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "ERROR") == 0 ){
            assertError(enode, child, "Unexpected component syntax.");
        }
    }
}

void BaseNode::visitComponentBody(BaseNode *parent, const TSNode &node){
    ComponentBodyNode* enode = new ComponentBodyNode(node);
    parent->addChild(enode);
    visitChildren(enode, node);
}

void BaseNode::visitNewComponentExpression(BaseNode *parent, const TSNode &node){
    NewComponentExpressionNode* enode = nullptr;

    if (parent->typeString() == "ComponentInstanceStatement"
        && parent->parent()
        && parent->parent()->typeString() == "Program")
    {
        enode = new RootNewComponentExpressionNode(node);
    } else {
        BaseNode* p = parent;
        while (p && dynamic_cast<JsBlockNode*>(p) == nullptr){
            p = p->parent();
        }
        if (p && p->typeString() == "JSScope"){
            enode = new RootNewComponentExpressionNode(node);
        } else {
            enode = new NewComponentExpressionNode(node);
        }
    }

    parent->addChild(enode);
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "identifier") == 0 ){
            auto iden = new IdentifierNode(child);
            enode->m_name.push_back(iden);
            enode->addChild(iden);
            if ( enode->m_name.size() == 1 )
                addUsedIdentifier(enode, iden);
        } else if ( strcmp(ts_node_type(child), "nested_identifier") == 0 ){
            enode->m_name = BaseNode::fromNestedIdentifier(enode, child);
            for( IdentifierNode* name : enode->m_name )
                enode->addChild(name);
            if ( enode->m_name.size() > 0 ){
                addUsedIdentifier(enode, enode->m_name.front());
            }
        } else if ( strcmp(ts_node_type(child), "new_component_body") == 0 ){
            enode->m_body = new ComponentBodyNode(child);
            enode->addChild(enode->m_body);
            visitChildren(enode->m_body, child);
        } else if ( strcmp(ts_node_type(child), "arguments") == 0 ){
            enode->m_arguments = new ArgumentsNode(child);
            enode->addChild(enode->m_arguments);
            visitChildren(enode->m_arguments, child);
            for ( auto child : enode->m_arguments->children() ){
                if (child->typeString() == "Identifier"){
                    addUsedIdentifier(enode->m_arguments, child->as<IdentifierNode>());
                }
            }

        } else if ( strcmp(ts_node_type(child), "component_identifier") == 0 ){
            TSNode id = ts_node_child(child, 1);
            if (strcmp(ts_node_type(id), "identifier") != 0)
                continue;
            enode->m_id = new IdentifierNode(id);
            enode->addChild(enode->m_id);
        }
    }

    // for id components
    BaseNode* p = parent;

    if ( enode->m_id ){
        while (p && enode->typeString() != "RootNewComponentExpression"){
            if (p->typeString() == "ComponentBody"){
                auto body = p->as<ComponentBodyNode>();
                if (body->parent()->typeString() == "ComponentDeclaration")
                {
                    auto decl = body->parent()->as<ComponentDeclarationNode>();
                    decl->pushToIdComponents(enode);
                    addToDeclarations(body, enode->m_id);
                    break;
                }
            } else if (p->typeString() == "RootNewComponentExpression"){
                auto rnce = p->as<RootNewComponentExpressionNode>();
                rnce->m_idComponents.push_back(enode);
                addToDeclarations(rnce, enode->m_id);
                break;
            }

            p = p->parent();
        }
    }


    // for default property components
    p = parent;
    if (p->typeString() == "ComponentBody")
    {
        auto body = p->as<ComponentBodyNode>();
        if (body->parent()->typeString() == "ComponentDeclaration")
        {
            auto decl = body->parent()->as<ComponentDeclarationNode>();
            decl->pushToDefault(enode);

        }
        else if (body->parent()->typeString() == "NewComponentExpression" || body->parent()->typeString() == "RootNewComponentExpression")
        {
            auto expr = body->parent()->as<NewComponentExpressionNode>();
            expr->pushToDefault(enode);
        }

    }

}

void BaseNode::visitComponentInstanceStatement(BaseNode *parent, const TSNode &node){
    ComponentInstanceStatementNode* enode = new ComponentInstanceStatementNode(node);
    parent->addChild(enode);
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "component_instance") == 0 ){
            TSNode id = ts_node_child(child, 1);
            if (strcmp(ts_node_type(id), "identifier") != 0)
                continue;
            enode->m_name = new IdentifierNode(id);
            addToDeclarations(parent, enode->m_name);
            enode->addChild(enode->m_name);
        } else if ( strcmp(ts_node_type(child), "new_component_expression") == 0 ){
            visitNewComponentExpression(enode, child);
        }
    }
}

void BaseNode::visitPropertyDeclaration(BaseNode *parent, const TSNode &node){
    PropertyDeclarationNode* enode = new PropertyDeclarationNode(node);
    parent->addChild(enode);

    std::string paramKey = "name";
    TSNode propName = ts_node_child_by_field_name(node, paramKey.c_str(), paramKey.length());
    assertValid(parent, propName, "Property name is null.");
    enode->m_name = new IdentifierNode(propName);
    enode->addChild(enode->m_name);
    paramKey = "type";
    TSNode propType = ts_node_child_by_field_name(node, paramKey.c_str(), paramKey.length());
    if ( !ts_node_is_null(propType) ){
        enode->m_type = new TypeNode(propType);
        enode->addChild(enode->m_type);
    }

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "property_assignment_expression") == 0 ){
            enode->m_expression = new BindableExpressionNode(child);
            enode->addChild(enode->m_expression);
            visitChildren(enode->m_expression, child);
        } else if (strcmp(ts_node_type(child), "statement_block") == 0) {
            enode->m_statementBlock = new JsBlockNode(child);
            enode->addChild(enode->m_statementBlock);
            visitChildren(enode->m_statementBlock, child);
        } else if ( strcmp(ts_node_type(child), "=") == 0 ){
            enode->m_isBindingAssignment = false;
        }
    }

    if (parent->parent() && parent->parent()->typeString() == "ComponentDeclaration"){
        parent->parent()->as<ComponentDeclarationNode>()->pushToProperties(enode);
    }

    if (parent->parent() && (parent->parent()->typeString() == "NewComponentExpression" ||
        parent->parent()->typeString() == "RootNewComponentExpression"))
    {
        parent->parent()->as<NewComponentExpressionNode>()->pushToProperties(enode);
    }
}

void BaseNode::visitStaticPropertyDeclaration(BaseNode *parent, const TSNode &node){
    StaticPropertyDeclarationNode* enode = new StaticPropertyDeclarationNode(node);
    parent->addChild(enode);

    TSNode propName = BaseNode::nodeChildByFieldName(node, "name");
    assertValid(parent, propName, "Property name is null.");
    enode->m_name = new IdentifierNode(propName);
    enode->addChild(enode->m_name);
    TSNode propType = BaseNode::nodeChildByFieldName(node, "type");
    if ( !ts_node_is_null(propType) ){
        enode->m_type = new TypeNode(propType);
        enode->addChild(enode->m_type);
    }

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "property_expression_initializer" ) == 0 ){
            uint32_t initCount = ts_node_child_count(child);
            if ( initCount == 2 ){
                TSNode initChild = ts_node_child(child, 1);
                enode->m_expression = new BindableExpressionNode(initChild);
                enode->addChild(enode->m_expression);
                visitChildren(enode->m_expression, initChild);
            }
        }
    }

    if (parent->parent() && parent->parent()->typeString() == "ComponentDeclaration"){
        auto componentDeclaration = parent->parent()->as<ComponentDeclarationNode>();
        if ( componentDeclaration->isAnonymous() ){
            throwError(enode, node, "Cannot declare static members for an anonymous component.");
        }

        componentDeclaration->pushToStaticProperties(enode);
    }

    if (parent->parent() && (parent->parent()->typeString() == "NewComponentExpression" ||
        parent->parent()->typeString() == "RootNewComponentExpression"))
    {
        SyntaxException se = CREATE_EXCEPTION(SyntaxException, "Syntax error: Cannot declare static members on a new expression.", Exception::toCode("~LanguageNodes"));
        BaseNode* p = parent;
        while (p && p->typeString() == "Program")
            p = p->parent();
        se.setParseLocation(ts_node_start_point(node).row + 1, ts_node_start_point(node).column + 1, ts_node_start_byte(node), p ? static_cast<ProgramNode*>(p)->m_filePath : "");
        throw se;
    }
}

void BaseNode::visitBindableExpression(BaseNode *parent, const TSNode &node){
    BindableExpressionNode* enode = new BindableExpressionNode(node);
    parent->addChild(enode);
    visitChildren(enode, node);
}

void BaseNode::visitMemberExpression(BaseNode *parent, const TSNode &node){
    MemberExpressionNode* enode = new MemberExpressionNode(node);
    parent->addChild(enode);
    visitChildren(enode, node);

    // import keyword fix
    uint32_t count = ts_node_child_count(node);
    if ( count > 0 ){
        TSNode child = ts_node_child(node, 0);
        if ( strcmp(ts_node_type(child), "import") == 0 ){
            enode->m_children.insert(enode->m_children.begin(), new IdentifierNode(child));
        }
    }

    if ( parent->typeString() == "CallExpression" ){
        if ( enode->children().size() > 0 && enode->children()[0]->typeString() == "MemberExpression" ){
            enode = enode->children()[0]->as<MemberExpressionNode>();
        } else {
            return;
        }
    }

    if ( enode->children().size() > 0 ){
        BaseNode* p = parent;
        BaseNode* prevP = enode;

        while (p){
            if (p->typeString() == "MemberExpression") break;
            if (p->typeString() == "FunctionDeclaration") break;
            if (p->typeString() == "ClassDeclaration") break;
            if (p->typeString() == "ArrowFunction") break;
            if (p->typeString() == "NewComponentExpression" ) break;
            if (p->typeString() == "ComponentDeclaration" ) break;
            if (p->typeString() == "ListenerDeclaration" ) break;

            if (p->typeString() == "AssignmentExpression" &&
                p->as<AssignmentExpressionNode>()->left() == prevP)
            {
                break;
            }

            if (p->typeString() == "PropertyDeclaration"){
                auto child = enode->children()[0];
                if (child->typeString() != "MemberExpression" && child->typeString() != "Identifier")
                    break;
                p->as<PropertyDeclarationNode>()->pushToBindings(enode);
                break;
            }
            if (p->typeString() == "PropertyAssignment"){
                auto child = enode->children()[0];
                if (child->typeString() != "MemberExpression" && child->typeString() != "Identifier")
                    break;
                p->as<PropertyAssignmentNode>()->pushToBindings(enode);
                break;
            }
            prevP = p;
            p = p->parent();
        }
    }


}

void BaseNode::visitSubscriptExpression(BaseNode *parent, const TSNode &node){
    SubscriptExpressionNode* enode = new SubscriptExpressionNode(node);
    parent->addChild(enode);
    visitChildren(enode, node);
}

void BaseNode::visitPropertyAssignment(BaseNode *parent, const TSNode &node){
    PropertyAssignmentNode* enode = new PropertyAssignmentNode(node);
    parent->addChild(enode);
    uint32_t count = ts_node_child_count(node);

    TSNode propertyName = BaseNode::nodeChildByFieldName(node, "name");
    assertValid(parent, propertyName, "Failed to find property name.");

    auto propertyIdentifiers = fromNestedIdentifier(enode, propertyName);
    for ( IdentifierNode* inode : propertyIdentifiers ){
        enode->addChild(inode);
    }
    enode->m_property = propertyIdentifiers;

    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "property_assignment_expression") == 0 ){
            enode->m_expression = new BindableExpressionNode(child);
            enode->addChild(enode->m_expression);
            visitChildren(enode->m_expression, child);
        } else if (strcmp(ts_node_type(child),"statement_block") == 0) {
            enode->m_statementBlock = new JsBlockNode(child);
            enode->addChild(enode->m_statementBlock);
            visitChildren(enode->m_statementBlock, child);
        } else if ( strcmp(ts_node_type(child), "=") == 0 ){
            enode->m_isBindingAssignment = false;
        }
    }

    if (parent->parent() && parent->parent()->typeString() == "ComponentDeclaration"){
        parent->parent()->as<ComponentDeclarationNode>()->pushToAssignments(enode);
    }

    if (parent->parent() && (parent->parent()->typeString() == "NewComponentExpression" || parent->parent()->typeString() == "RootNewComponentExpression")){
        parent->parent()->as<NewComponentExpressionNode>()->pushToAssignments(enode);
    }
}

void BaseNode::visitIdentifierAssignment(BaseNode *parent, const TSNode &node){
    BaseNode* componentParent = parent ? parent->parent() : nullptr;
    if ( !componentParent )
        return;

    TSNode idChild = ts_node_child(node, 2);

    if ( componentParent->typeString() == "ComponentDeclaration" ){
        ComponentDeclarationNode* cdn = componentParent->as<ComponentDeclarationNode>();
        cdn->m_id = new IdentifierNode(idChild);
        // provide id to outside scope
        addToDeclarations(componentParent, cdn->m_id);
    } else if ( componentParent->typeString() == "NewComponentExpression" || componentParent->typeString() == "RootNewComponentExpression"){
        NewComponentExpressionNode* ncen = componentParent->as<NewComponentExpressionNode>();
        ncen->m_id = new IdentifierNode(idChild);
        // provide id to outside scope
        addToDeclarations(componentParent, ncen->m_id);
    }
}

void BaseNode::visitEventDeclaration(BaseNode *parent, const TSNode &node){
    EventDeclarationNode* enode = new EventDeclarationNode(node);
    parent->addChild(enode);

    TSNode name = nodeChildByFieldName(node, "name");
    assertValid(parent, name, "Function name is null.");
    enode->m_name = new IdentifierNode(name);

    TSNode parameters = nodeChildByFieldName(node, "parameters");
    assertValid(parent, parameters, "Event parameters are null.");

    enode->m_parameters = BaseNode::scanFormalTypeParameters(parent, parameters);
    enode->addChild(enode->m_parameters);

    if (parent->parent() && parent->parent()->typeString() == "ComponentDeclaration"){
        parent->parent()->as<ComponentDeclarationNode>()->m_events.push_back(enode);
    }

    if (parent->parent() && parent->parent()->canCast<NewComponentExpressionNode>() ){
        parent->parent()->as<NewComponentExpressionNode>()->m_events.push_back(enode);
    }
}

void BaseNode::visitListenerDeclaration(BaseNode *parent, const TSNode &node){
    ListenerDeclarationNode* enode = new ListenerDeclarationNode(node);
    parent->addChild(enode);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "property_identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
        } else if ( strcmp(ts_node_type(child), "formal_parameters") == 0 ){
            enode->m_parameters = scanFormalParameters(parent, child);
        }
    }

    TSNode body = BaseNode::nodeChildByFieldName(node, "body");
    assertValid(parent, body, "Failed to find listener body.");

    if ( strcmp(ts_node_type(body), "statement_block") == 0 ){
        enode->m_body = new JsBlockNode(body);
        enode->addChild(enode->m_body);
        visitChildren(enode->m_body, body);
    } else {
        enode->m_bodyExpression = new ExpressionNode(body);
        enode->addChild(enode->m_bodyExpression);
        visitChildren(enode->m_bodyExpression, body);
    }

    if (parent->parent() && parent->parent()->typeString() == "ComponentDeclaration"){
        parent->parent()->as<ComponentDeclarationNode>()->m_listeners.push_back(enode);
    }
    if (parent->parent() && parent->parent()->canCast<NewComponentExpressionNode>()){
        parent->parent()->as<NewComponentExpressionNode>()->m_listeners.push_back(enode);
    }

    if ( enode->m_parameters ){
        if (enode->m_body ){
            for (size_t i = 0; i != enode->m_parameters->m_parameters.size(); ++i){
                addToDeclarations(enode->m_body, enode->m_parameters->m_parameters[i]->identifier());
            }
        } else {
            for (size_t i = 0; i != enode->m_parameters->m_parameters.size(); ++i){
                addToDeclarations(enode, enode->m_parameters->m_parameters[i]->identifier());
            }
        }
    }
}

void BaseNode::visitMethodDefinition(BaseNode *parent, const TSNode &node){
    MethodDefinitionNode* enode = new MethodDefinitionNode(node);
    parent->addChild(enode);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "property_identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
        } else if ( strcmp(ts_node_type(child), "formal_parameters") == 0 ){
            enode->m_parameters = BaseNode::scanFormalParameters(parent, child);
        } else if ( strcmp(ts_node_type(child), "statement_block") == 0 ){
            enode->m_body = new JsBlockNode(child);
            enode->addChild(enode->m_body);
            visitChildren(enode->m_body, child);
        }
    }
}

void BaseNode::visitTypedMethodDeclaration(BaseNode *parent, const TSNode &node){
    TypedMethodDeclarationNode* enode = new TypedMethodDeclarationNode(node);
    parent->addChild(enode);

    TSNode name = nodeChildByFieldName(node, "name");
    assertValid(parent, name, "Function name is null.");
    enode->m_name = new IdentifierNode(name);

    TSNode parameters = nodeChildByFieldName(node, "parameters");
    assertValid(parent, parameters, "Function parameters are null.");

    enode->m_parameters = BaseNode::scanFormalTypeParameters(parent, parameters);
    enode->addChild(enode->m_parameters);

    TSNode body = nodeChildByFieldName(node, "body");
    assertValid(parent, body, "Function body is null.");

    enode->m_body = new JsBlockNode(body);
    enode->addChild(enode->m_body);
    visitChildren(enode->m_body, body);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){

        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "static") == 0 ){
            enode->setStatic(true);
        } else if ( strcmp(ts_node_type(child), "async") == 0 ){
            enode->setAsync(true);
        }
    }

    if (parent->parent() && parent->parent()->typeString() == "ComponentDeclaration"){
        parent->parent()->as<ComponentDeclarationNode>()->m_methods.push_back(enode);
    }
    if (parent->parent() && parent->parent()->canCast<NewComponentExpressionNode>()){
        parent->parent()->as<NewComponentExpressionNode>()->m_methods.push_back(enode);
    }

    if (enode->m_body && enode->m_parameters){
        for (size_t i = 0; i != enode->m_parameters->m_parameters.size(); ++i){
            addToDeclarations(enode->m_body, enode->m_parameters->m_parameters[i]->identifier());
        }
    }
}

void BaseNode::visitPropertyAccessorDeclaration(BaseNode *parent, const TSNode &node){
    PropertyAccessorDeclarationNode* enode = new PropertyAccessorDeclarationNode(node);
    parent->addChild(enode);

    uint32_t nodeCount = ts_node_child_count(node);
    for (uint32_t i = 0; i < nodeCount; ++i){
        if (strcmp(ts_node_type(ts_node_child(node, i)), "get") == 0){
            enode->m_access = PropertyAccessorDeclarationNode::Getter;
        } else if (strcmp(ts_node_type(ts_node_child(node, i)), "set") == 0){
            enode->m_access = PropertyAccessorDeclarationNode::Setter;
        }
    }

    TSNode name = nodeChildByFieldName(node, "name");
    assertValid(parent, name, "Accessor name is null.");
    enode->m_name = new IdentifierNode(name);

    if ( enode->m_access == PropertyAccessorDeclarationNode::Setter ){
        TSNode parameters = nodeChildByFieldName(node, "parameters");
        assertValid(parent, parameters, "Set parameters are null.");
        enode->m_parameters = scanFormalTypeParameters(enode, parameters);
    }

    TSNode body = nodeChildByFieldName(node, "body");
    assertValid(parent, body, "Accessor body is null.");

    enode->m_body = new JsBlockNode(body);
    enode->addChild(enode->m_body);
    visitChildren(enode->m_body, body);

    if (enode->m_body && enode->m_parameters ){
        for (size_t i = 0; i != enode->m_parameters->m_parameters.size(); ++i){
            auto param = enode->m_parameters->m_parameters[i];
            addToDeclarations(enode->m_body, param->identifier());
        }
    }

    if (parent->parent() && parent->parent()->typeString() == "ComponentDeclaration"){
        parent->parent()->as<ComponentDeclarationNode>()->pushToPropertyAccessors(enode);
    }
}

void BaseNode::visitPublicFieldDeclaration(BaseNode *parent, const TSNode &node){
    PublicFieldDeclarationNode* enode = new PublicFieldDeclarationNode(node);
    parent->addChild(enode);
    visitChildren(enode, node);
}

void BaseNode::visitJsScope(BaseNode *parent, const TSNode &node){
    JsBlockNode* enode = new JsBlockNode(node);
    parent->addChild(enode);
    visitChildren(enode, node);
}

void BaseNode::visitNumber(BaseNode *parent, const TSNode &node)
{
    NumberNode* nnode = new NumberNode(node);
    parent->addChild(nnode);
}

void BaseNode::visitConstructorDefinition(BaseNode *parent, const TSNode &node)
{
    ConstructorDefinitionNode* cdnode = new ConstructorDefinitionNode(node);
    parent->addChild(cdnode);
    // visitChildren(cdnode, node);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "statement_block") == 0 ){
            cdnode->m_body = new JsBlockNode(child);
            cdnode->addChild(cdnode->m_body);
            visitChildren(cdnode->m_body, child);
        } else if ( strcmp(ts_node_type(child), "formal_type_parameters") == 0 ){
            TSNode& parameters = child;

            cdnode->m_parameters = new ParameterListNode(parameters);
            cdnode->addChild(cdnode->m_parameters);
            uint32_t parameterCount = ts_node_child_count(parameters);

            for (uint32_t pc = 0; pc < parameterCount; ++pc){
                TSNode ftpc = ts_node_child(parameters, pc);
                assertError(parent, ftpc, "Constructor parameter not declared properly.");
                if (strcmp(ts_node_type(ftpc), "formal_type_parameter") == 0){

                    if ( ts_node_child_count(ftpc) > 0 ){
                        TSNode typeParameter = ts_node_child(ftpc, 0);

                        std::string paramKey = "name";
                        TSNode parameterName = ts_node_child_by_field_name(typeParameter, paramKey.c_str(), paramKey.length());
                        assertValid(cdnode, parameterName, "Parameter name is null.");
                        paramKey = "type";
                        TSNode parameterType = ts_node_child_by_field_name(typeParameter, paramKey.c_str(), paramKey.length());
                        assertValid(cdnode, parameterType, "Parameter type is null.");

                        if ( strcmp(ts_node_type(typeParameter), "required_type_parameter") == 0 ){
                            cdnode->m_parameters->m_parameters.push_back(
                                new ParameterNode(typeParameter, new IdentifierNode(parameterName), new TypeNode(parameterType), false)
                            );
                        } else if ( strcmp(ts_node_type(typeParameter), "optional_type_parameter") == 0){
                            cdnode->m_parameters->m_parameters.push_back(
                                new ParameterNode(typeParameter, new IdentifierNode(parameterName), new TypeNode(parameterType), true)
                            );
                        }
                    }
                }
            }
        } else
            visit(cdnode, child);
    }

    if (parent->typeString() == "ComponentBody"){
        parent->as<ComponentBodyNode>()->setConstructor(cdnode);
    }

    if (cdnode->m_body){
        for (size_t i = 0; i != cdnode->m_parameters->parameters().size(); ++i){
            addToDeclarations(cdnode->m_body, cdnode->m_parameters->m_parameters[i]->identifier());
        }
    }
}

void BaseNode::visitExpressionStatement(BaseNode *parent, const TSNode &node)
{
    ExpressionStatementNode* esnode = new ExpressionStatementNode(node);
    parent->addChild(esnode);
    visitChildren(esnode, node);
}

void BaseNode::visitAssignmentExpression(BaseNode *parent, const TSNode &node){
    AssignmentExpressionNode* esnode = new AssignmentExpressionNode(node);
    parent->addChild(esnode);
    visitChildren(esnode, node);
}

void BaseNode::visitCallExpression(BaseNode *parent, const TSNode &node)
{
    CallExpressionNode* enode = new CallExpressionNode(node);
    parent->addChild(enode);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "arguments") == 0 ){
            enode->m_arguments = new ArgumentsNode(child);
            enode->addChild(enode->m_arguments);
            visitChildren(enode->m_arguments, child);

            for ( BaseNode* childNode : enode->m_arguments->children() ){
                if ( childNode->typeString() == "Identifier" ){
                    addUsedIdentifier(enode, childNode->as<IdentifierNode>());
                }
            }

        } else if ( strcmp(ts_node_type(child), "super") == 0 ) {
            enode->isSuper = true;
            if (parent->parent() && parent->parent()->parent()
                    && parent->parent()->parent()->typeString() == "ConstructorDefinition")
            {
                parent->parent()->parent()->as<ConstructorDefinitionNode>()->setSuperCall(enode);
            }
        } else {
            visit(enode, child);
        }
    }
}

void BaseNode::visitNewTaggedComponentExpression(BaseNode *parent, const TSNode &node)
{
    NewTaggedComponentExpressionNode* tagnode = new NewTaggedComponentExpressionNode(node);
    parent->addChild(tagnode);
    visitChildren(tagnode, node);

    // for default property components
    BaseNode* p = parent;
    if (p->typeString() == "ComponentBody")
    {
        auto body = p->as<ComponentBodyNode>();
        if (body->parent()->typeString() == "ComponentDeclaration")
        {
            auto decl = body->parent()->as<ComponentDeclarationNode>();
            decl->pushToDefault(tagnode);
        }
        else if (body->parent()->typeString() == "NewComponentExpression" || body->parent()->typeString() == "RootNewComponentExpression")
        {
            auto expr = body->parent()->as<NewComponentExpressionNode>();
            expr->pushToDefault(tagnode);
        }
    }


    for (auto child: tagnode->children()){
        if (child->typeString() == "Identifier") {
            addUsedIdentifier(tagnode, child->as<IdentifierNode>());
        }
    }
}

void BaseNode::visitNewTrippleTaggedComponentExpression(BaseNode *parent, const TSNode &node){
    NewTrippleTaggedComponentExpressionNode* tagnode = new NewTrippleTaggedComponentExpressionNode(node);
    parent->addChild(tagnode);
    visitChildren(tagnode, node);

    // for default property components
    BaseNode* p = parent;
    if (p->typeString() == "ComponentBody")
    {
        auto body = p->as<ComponentBodyNode>();
        if (body->parent()->typeString() == "ComponentDeclaration")
        {
            auto decl = body->parent()->as<ComponentDeclarationNode>();
            decl->pushToDefault(tagnode);
        }
        else if (body->parent()->typeString() == "NewComponentExpression" || body->parent()->typeString() == "RootNewComponentExpression")
        {
            auto expr = body->parent()->as<NewComponentExpressionNode>();
            expr->pushToDefault(tagnode);
        }
    }


    for (auto child: tagnode->children()){
        if (child->typeString() == "Identifier") {
            addUsedIdentifier(tagnode, child->as<IdentifierNode>());
        }
    }
}

void BaseNode::visitTaggedString(BaseNode *parent, const TSNode &node){
    TaggedStringNode* tsnode = new TaggedStringNode(node);
    parent->addChild(tsnode);
}

void BaseNode::visitTrippleTaggedString(BaseNode *parent, const TSNode &node){
    TrippleTaggedStringNode* tsnode = new TrippleTaggedStringNode(node);
    parent->addChild(tsnode);
}

void BaseNode::visitFunction(BaseNode *parent, const TSNode &node){
    FunctionNode* enode = new FunctionNode(node);
    parent->addChild(enode);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "formal_parameters") == 0 ){
            enode->m_parameters = BaseNode::scanFormalParameters(parent, child);
        } else if ( strcmp(ts_node_type(child), "statement_block") == 0 ){
            enode->m_body = new JsBlockNode(child);
            enode->addChild(enode->m_body);
            visitChildren(enode->m_body, child);
        }
    }

    if ( enode->m_parameters ){
        if (enode->m_body ){
            for (size_t i = 0; i != enode->m_parameters->m_parameters.size(); ++i){
                addToDeclarations(enode->m_body, enode->m_parameters->m_parameters[i]->identifier());
            }
        }
    }
}

void BaseNode::visitFunctionDeclaration(BaseNode *parent, const TSNode &node)
{
    FunctionDeclarationNode* enode = new FunctionDeclarationNode(node);
    parent->addChild(enode);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
            enode->addChild(enode->m_name);
            addToDeclarations(parent, enode->m_name);
        } else if ( strcmp(ts_node_type(child), "property_identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
            enode->addChild(enode->m_name);
        } else if ( strcmp(ts_node_type(child), "formal_parameters") == 0 ){
            enode->m_parameters = BaseNode::scanFormalParameters(parent, child);
        } else if ( strcmp(ts_node_type(child), "statement_block") == 0 ){
            enode->m_body = new JsBlockNode(child);
            enode->addChild(enode->m_body);
            visitChildren(enode->m_body, child);
        }
    }

    if ( enode->m_parameters ){
        if (enode->m_body ){
            for (size_t i = 0; i != enode->m_parameters->m_parameters.size(); ++i){
                addToDeclarations(enode->m_body, enode->m_parameters->m_parameters[i]->identifier());
            }
        }
    }
}

void BaseNode::visitClassDeclaration(BaseNode *parent, const TSNode &node)
{
    ClassDeclarationNode* fnode = new ClassDeclarationNode(node);
    parent->addChild(fnode);
    visitChildren(fnode, node);

    TSNode nameNode = ts_node_child_by_field_name(node, "name", 4);
    if ( !ts_node_is_null(nameNode) && strcmp(ts_node_type(nameNode), "identifier") != 0){
        fnode->addChild(new IdentifierNode(nameNode));
    }

    for (auto it = fnode->children().begin(); it != fnode->children().end(); ++it)
        if ((*it)->typeString() == "Identifier")
        {
            // this must be the id!
            IdentifierNode* id = static_cast<IdentifierNode*>(*it);
            addToDeclarations(parent, id);
        }
}

void BaseNode::visitVariableDeclaration(BaseNode *parent, const TSNode &node)
{
    VariableDeclarationNode* vdn = new VariableDeclarationNode(node);
    parent->addChild(vdn);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "variable_declarator") == 0 ){
            VariableDeclaratorNode* decl = new VariableDeclaratorNode(child);
            decl->setParent(vdn);
            vdn->addChild(decl);
            vdn->m_declarators.push_back(decl);

            uint32_t subcount = ts_node_child_count(child);
            bool equalsFound = false;

            for ( uint32_t k = 0; k < subcount; ++k ){
                TSNode smaller_child = ts_node_child(child, k);
                if ( !equalsFound && strcmp(ts_node_type(smaller_child), "identifier") == 0 ){
                    IdentifierNode* id = new IdentifierNode(smaller_child);
                    decl->addChild(id);
                    addToDeclarations(decl, id);
                } else if ( strcmp(ts_node_type(smaller_child), "=") == 0 ){
                    equalsFound = true;
                } else {
                    visit(decl, smaller_child);
                }
            }
        } else if ( strcmp(ts_node_type(child), ";") == 0 ) {
            vdn->m_hasSemicolon = true;
        } else {
            visit(vdn, child);
        }
    }

}

void BaseNode::visitLexicalDeclaration(BaseNode *parent, const TSNode &node){
    VariableDeclarationNode* vdn = new VariableDeclarationNode(node);
    parent->addChild(vdn);
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "variable_declarator") == 0 ){
            VariableDeclaratorNode* decl = new VariableDeclaratorNode(child);
            decl->setParent(vdn);
            vdn->addChild(decl);
            vdn->m_declarators.push_back(decl);

            uint32_t subcount = ts_node_child_count(child);
            bool equalsFound = false;

            for ( uint32_t k = 0; k < subcount; ++k ){
                TSNode smaller_child = ts_node_child(child, k);
                if ( !equalsFound && strcmp(ts_node_type(smaller_child), "identifier") == 0 ){
                    IdentifierNode* id = new IdentifierNode(smaller_child);
                    decl->addChild(id);
                    addToDeclarations(decl, id);
                } else if ( strcmp(ts_node_type(smaller_child), "=") == 0 ){
                    equalsFound = true;
                } else {
                    visit(decl, smaller_child);
                }
            }
        } else if ( strcmp(ts_node_type(child), ";") == 0 ) {
            vdn->m_hasSemicolon = true;
        } else {
            visit(vdn, child);
        }
    }
}

void BaseNode::visitDestructuringPattern(BaseNode *parent, const TSNode &node){
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "identifier") == 0 ){
            auto inode = new IdentifierNode(child);
            inode->setParent(parent);
            parent->addChild(inode);
            addToDeclarations(parent, inode);
        } else if ( strcmp(ts_node_type(child), "shorthand_property_identifier") == 0 ){
            auto inode = new IdentifierNode(child);
            inode->setParent(parent);
            parent->addChild(inode);
            addToDeclarations(parent, inode);
        } else {
            visitDestructuringPattern(parent, child);
        }
    }
}

void BaseNode::visitNewExpression(BaseNode *parent, const TSNode &node)
{
    NewExpressionNode* nenode = new NewExpressionNode(node);
    parent->addChild(nenode);
    nenode->setParent(parent);
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "identifier") == 0 ){
            auto inode = new IdentifierNode(child);
            inode->setParent(nenode);
            nenode->addChild(inode);
            addUsedIdentifier(nenode, inode);
        }
    }
    visitChildren(nenode, node);
}

void BaseNode::visitReturnStatement(BaseNode *parent, const TSNode &node)
{
    ReturnStatementNode* rsnode = new ReturnStatementNode(node);
    parent->addChild(rsnode);
    visitChildren(rsnode, node);
}

void BaseNode::visitArrowFunction(BaseNode *parent, const TSNode &node){
    ArrowFunctionNode* enode = new ArrowFunctionNode(node);
    parent->addChild(enode);

    bool arrowPassed = false;
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);

        if ( strcmp(ts_node_type(child), "property_identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
        } else if ( strcmp(ts_node_type(child), "identifier") == 0 ){
            auto nameNode = new IdentifierNode(child);
            enode->m_parameters.push_back(nameNode);
            enode->addChild(nameNode);
        } else if ( strcmp(ts_node_type(child), "formal_parameters") == 0 ){
            uint32_t paramterCount = ts_node_child_count(child);

            for (uint32_t pc = 0; pc < paramterCount; ++pc){
                TSNode ftpc = ts_node_child(child, pc);
                if (strcmp(ts_node_type(ftpc), "identifier") == 0){
                    auto nameNode = new IdentifierNode(ftpc);
                    enode->m_parameters.push_back(nameNode);
                    enode->addChild(nameNode);
                } else if ( strcmp(ts_node_type(ftpc), "required_parameter") == 0 ){
                    uint32_t paramterInteralCount = ts_node_child_count(ftpc);
                    for (uint32_t pi = 0; pi < paramterInteralCount; ++pi){
                        TSNode ftpci = ts_node_child(ftpc, pi);
                        if (strcmp(ts_node_type(ftpci), "identifier") == 0){
                            auto nameNode = new IdentifierNode(ftpci);
                            enode->m_parameters.push_back(nameNode);
                            enode->addChild(nameNode);
                        }
                    }
                } else if ( strcmp(ts_node_type(ftpc), "optional_parameter") == 0 ){
                    uint32_t paramterInteralCount = ts_node_child_count(ftpc);
                    for (uint32_t pi = 0; pi < paramterInteralCount; ++pi){
                        TSNode ftpci = ts_node_child(ftpc, pi);
                        if (strcmp(ts_node_type(ftpci), "identifier") == 0){
                            auto nameNode = new IdentifierNode(ftpci);
                            enode->m_parameters.push_back(nameNode);
                            enode->addChild(nameNode);
                        }
                    }
                }
            }
        } else if ( strcmp(ts_node_type(child), "=>") == 0 ){
            arrowPassed = true;
        } else if ( strcmp(ts_node_type(child), "statement_block") == 0 ){
            enode->m_body = new JsBlockNode(child);
            enode->addChild(enode->m_body);
            visitChildren(enode->m_body, child);
        } else {
            if ( arrowPassed ){
                visit(enode, child);
            }
        }
    }

    if (enode->m_body){
        for (size_t i = 0; i != enode->m_parameters.size(); ++i){
            addToDeclarations(enode->m_body, enode->m_parameters[i]);
        }
    } else {
        for (size_t i = 0; i != enode->m_parameters.size(); ++i){
            addToDeclarations(enode, enode->m_parameters[i]);
        }
    }
}

void BaseNode::visitObject(BaseNode *parent, const TSNode &node)
{
    ObjectNode* onode = new ObjectNode(node);
    parent->addChild(onode);
    visitChildren(onode, node);
}

std::string JsImportNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += std::string("JsImport (" + rangeString()+ ")") + "\n";

    return result;
}

std::string ImportNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    std::string importPath;
    if ( m_importPath ){
        for( ImportPathSegmentNode* node : m_importPath->segments() ){
            importPath += node->rangeString();
        }
    }

    std::string importAs = m_importAs ? "(as " + m_importAs->rangeString() + ")" : "";

    result += std::string("Import (" + importPath + ")") + importAs + rangeString() + "\n";

    return result;
}

std::string ImportNode::path(const std::string& source) const{
    std::string importSegments;

    if (isRelative())
        importSegments = ".";

    auto segments = m_importPath->segments();
    for (size_t i = 0; i < segments.size(); ++i){
        auto node = segments[i];
        if (i != 0)
            importSegments += ".";
        importSegments += slice(source, node);
    }

    return importSegments;
}

std::string ImportNode::as(const std::string &source) const{
    if ( m_importAs )
        return slice(source, m_importAs);
    return "";
}

void ImportNode::addChild(BaseNode *child)
{
    if (child->typeString() == "ImportPath"){
        m_importPath = child->as<ImportPathNode>();
        child->setParent(this);
        return;
    }
    BaseNode::addChild(child);
}

void ProgramNode::collectImportTypes(const std::string &source, ConversionContext *ctx){
    if ( m_importTypesCollected )
        return;

    std::vector<IdentifierNode*> identifiers;
    collectImports(source, identifiers, ctx);

    for ( auto identifier : identifiers ){

        std::string idName = slice(source, identifier);

        bool isNamespaceIdentifier = false;

        // check whether identifier is part of an import namespace
        for ( ImportNode* in : m_imports ){
            if ( in->hasNamespace() ){
                if ( in->as(source) == idName ){
                    isNamespaceIdentifier = true;
                    BaseNode* parent = identifier->parent();
                    if ( parent->typeString() == "ComponentDeclaration" ){
                        auto parentCast = parent->as<ComponentDeclarationNode>();
                        if ( parentCast->heritage().size() > 1 && parentCast->heritage()[0] == identifier ){
                            ProgramNode::ImportType impt;
                            impt.importNamespace = idName;
                            impt.name = slice(source, parentCast->heritage()[1]);
                            addImportType(impt);
                        }
                    } else if ( parent->typeString() == "MemberExpression" ){
                        auto parentCast = parent->as<MemberExpressionNode>();
                        if ( parentCast->children().size() > 1 && parentCast->children()[0] == identifier ){
                            ProgramNode::ImportType impt;
                            impt.importNamespace = idName;
                            impt.name = slice(source, parentCast->children()[1]);
                            addImportType(impt);
                        }
                    } else if ( parent->typeString() == "NewComponentExpression" || parent->typeString() == "RootNewComponentExpression" ){
                        auto parentCast = parent->as<NewComponentExpressionNode>();
                        if ( parentCast->name().size() > 1 && parentCast->name()[0] == identifier ){
                            ProgramNode::ImportType impt;
                            impt.importNamespace = idName;
                            impt.name = slice(source, parentCast->name()[1]);
                            addImportType(impt);
                        }
                    }
                }
            }
        }

        if ( !isNamespaceIdentifier ){
            ProgramNode::ImportType impt;
            impt.name = idName;
            addImportType(impt);
        }
    }

    m_importTypesCollected = true;
}

void ProgramNode::resolveImport(const std::string &as, const std::string &name, const std::string &path){
    auto asIt = m_importTypes.find(as);
    if ( asIt != m_importTypes.end() ){
        auto nameIt = asIt->second.find(name);
        if ( nameIt != asIt->second.end() ){
            nameIt->second.resolvedPath = path;
        }
    }
}

std::string ProgramNode::importTypesString() const{
    std::string result;
    for ( auto it = m_importTypes.begin(); it != m_importTypes.end(); ++it ){
        if ( !result.empty() )
            result += '\n';
        result += "Imports as '" + it->first + "':";

        for ( auto impIt = it->second.begin(); impIt != it->second.end(); ++ impIt ){
            result += " " + impIt->second.name;
        }
    }
    return result;
}

void ProgramNode::addChild(BaseNode *child){
    if ( child->typeString() == "Import" ){
        m_imports.push_back(child->as<ImportNode>());
        child->setParent(this);
        return;
    } else if ( child->typeString() == "JsImport" ){
        m_jsImports.push_back(child->as<JsImportNode>());
        BaseNode::addChild(child);
    } else if ( child->typeString() == "ComponentDeclaration" ){
        m_exports.push_back(child);
        BaseNode::addChild(child);
    } else if ( child->typeString() == "ComponentInstanceStatement" ){
        m_exports.push_back(child);
        BaseNode::addChild(child);
    } else {
        SyntaxException se = CREATE_EXCEPTION(SyntaxException, "Unexpected expression in file root of type '" + child->typeString() + "'", lv::Exception::toCode("~Parse"));
        se.setParseLocation(child->startPoint().first + 1, child->startPoint().second + 1, child->startByte(), filePath());
        throw se;
    }
}

std::string ProgramNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += std::string("Program [") +
            std::to_string(ts_node_start_byte(current())) + ", " +
            std::to_string(ts_node_end_byte(current())) + "]\n";

    std::string indentStr;
    if ( indent > 0 )
        indentStr.assign(indent * 2, ' ');

    result += indentStr + " /Imports\n";
    for (BaseNode* child : m_imports){
        result += child->toString(indent < 0 ? indent : indent + 1);
    }

    result += indentStr + " /Component Exports\n";
    for (BaseNode* child : m_exports){
        result += child->toString(indent < 0 ? indent : indent + 1);
    }

    result += indentStr + " /Children\n";
    for (BaseNode* child : children()){
        result += child->toString(indent < 0 ? indent : indent + 1);
    }

    return result;
}

void ProgramNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &fragments, int indentValue, ConversionContext* ctx){

    if ( ctx && !ctx->jsImportsEnabled && !m_jsImports.empty() ){
        THROW_EXCEPTION(lv::Exception, "Javascript imports are not enabled.", lv::Exception::toCode("~Enabled"));
    }

    ElementsInsertion* importsCompose = new ElementsInsertion;
    importsCompose->from = 0;
    int to = 0;
    for ( ImportNode* node: m_imports ){
        if ( node->endByte() > to )
            to = node->endByte();
    }
    for ( JsImportNode* node : m_jsImports ){
        if ( node->endByte() > to )
            to = node->endByte();
    }
    importsCompose->to = to;

    for ( JsImportNode* node : m_jsImports ){
        std::string importsNames;
        for ( IdentifierNode* nameNode : node->importNames() ){
            if ( !importsNames.empty() )
                importsNames += ",";
            importsNames += slice(source, nameNode);
        }

        std::string importPath = slice(source, node->importPath());
        if ( importPath.length() > 2 )
            importPath = importPath.substr(1, importPath.length() - 2);
        if ( ctx && importPath.length() > 2 ){
            if ( !ctx->relativePathFromBuild.empty() && importPath[0] == '.' && (importPath[1] == '.' || importPath[1] == '/') ){
                importPath = ctx->relativePathFromBuild + "/" + importPath;
            }
        }

        *importsCompose << "import {" << importsNames << "} from \'" << importPath << "\'\n";
    }

    if ( ctx && !ctx->baseComponentImportUri.empty() && !ctx->baseComponent.empty() ){
        ImportType it;
        it.name = ctx->baseComponent;
        it.resolvedPath = ctx->baseComponentImportUri;
        addImportType(it);
    }

    for ( auto it = m_importTypes.begin(); it != m_importTypes.end(); ++it ){
        if ( it->first.empty() ){
            for ( auto impIt = it->second.begin(); impIt != it->second.end(); ++ impIt ){
                if ( !ctx->allowUnresolved && impIt->second.resolvedPath.empty() ){
                    delete importsCompose;
                    THROW_EXCEPTION(lv::Exception, "Identifer not found in imports: \'" + impIt->second.name + "\' in " + filePath(), lv::Exception::toCode("~Identifier"));
                }
                std::string impPath = impIt->second.resolvedPath.empty() ? "__UNRESOLVED__" : impIt->second.resolvedPath;
                *importsCompose << ("import {" + impIt->second.name + "} from '" + impPath + "'\n");
            }
        } else {
            std::string moduleWrap = "let " + it->first + " = {";
            for ( auto impIt = it->second.begin(); impIt != it->second.end(); ++ impIt ){
                if ( !ctx->allowUnresolved && impIt->second.resolvedPath.empty() ){
                    delete importsCompose;
                    THROW_EXCEPTION(lv::Exception, "Identifer not found in imports: \'" + impIt->second.name + "\' in " + filePath(), lv::Exception::toCode("~Identifier"));
                }
                std::string impPath = impIt->second.resolvedPath.empty() ? "__UNRESOLVED__" : impIt->second.resolvedPath;
                std::string impKey = "__" + it->first + "__" + impIt->second.name;
                std::string impName = impIt->second.name;

                if ( impIt != it->second.begin() )
                    moduleWrap += ", ";
                moduleWrap += impIt->second.name + ":" + impKey;

                *importsCompose << ("import {" + impName + " as " + impKey + "} from '" + impPath + "'\n");
            }
            moduleWrap += "}\n";
            *importsCompose << moduleWrap;
        }
    }

    fragments.push_back(importsCompose);

    size_t offset = fragments.size();

    for ( BaseNode* child: m_exports )
        child->convertToJs(source, fragments, indentValue, ctx);

//    BaseNode::convertToJs(source, fragments, indentValue);

    if (m_idComponents.empty())
        return;
    
    auto iter = fragments.begin() + offset;
    ElementsInsertion* compose = new ElementsInsertion;
    if (offset == fragments.size()) {
        compose->from = compose->to = static_cast<int>(source.length());
    } else {
        compose->from = compose->to = (*iter)->from;
    }

    fragments.insert(iter, compose);
}

void ProgramNode::collectImports(const std::string &source, std::vector<IdentifierNode *> &identifiers, ConversionContext *ctx){
    for ( BaseNode* child: m_exports ){
        child->collectImports(source, identifiers, ctx);
    }
}

void ProgramNode::addImportType(const ProgramNode::ImportType &t){
    auto asIt = m_importTypes.find(t.importNamespace);
    if ( asIt == m_importTypes.end() ){
        std::map<std::string, ProgramNode::ImportType> imports;
        imports[t.name] = t;
        m_importTypes[t.importNamespace] = imports;
    } else {
        asIt->second[t.name] = t;
    }
}

ComponentDeclarationNode::ComponentDeclarationNode(const TSNode &node)
    : JsBlockNode(node, "ComponentDeclaration")
    , m_name(nullptr)
    , m_id(nullptr)
    , m_body(nullptr)
{
}

std::string ComponentDeclarationNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    std::string name = "(name " + (m_name ? m_name->rangeString() : "") + ")";

    std::string inheritance = "";
    if ( m_heritage.size() > 0 ){
        inheritance = "(inherits ";
        for( IdentifierNode* node :m_heritage ){
            inheritance += node->rangeString();
        }
        inheritance += ")";
    }

    std::string identifier = "";
    if ( m_id ){
        identifier = "(id " + m_id->rangeString() + ")";
    }

    result += "ComponentDeclaration " + name + identifier + inheritance + rangeString() + "\n";
    if ( m_body ){
        result += m_body->toString(indent >= 0 ? indent + 1 : indent);
    }

    return result;
}

void ComponentDeclarationNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &fragments, int indentValue, ConversionContext* ctx){
    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = startByte();
    compose->to   = endByte();

    std::string componentName = name(source);

    std::string heritage = "";
    if ( m_heritage.size() > 0 ){
        heritage = slice(source, m_heritage[0]);
        for ( size_t i = 1; i < m_heritage.size(); ++i ){
            heritage += "." + slice(source, m_heritage[i]);
        }
    }

    if ( heritage.empty() )
        heritage = ConversionContext::baseComponentName(ctx);

    if (parent() && parent()->typeString() == "Program"){
        *compose << "\n" << indent(indentValue) << "export class ";
    } else {
        *compose << "class ";
    }

    *compose << (isAnonymous() ? "" : componentName + " ") << "extends " + heritage + "{\n\n";

    // handle constructor
    if (m_body->constructor()){
        *compose << indent(indentValue + 1) <<  "constructor";
        if ( m_body->constructor()->m_parameters && m_body->constructor()->m_parameters->parameters().size() ){
            std::string params = "(";
            for (size_t i = 0; i != m_body->constructor()->m_parameters->parameters().size(); ++i){
                if ( i != 0 )
                    params += ",";
                ParameterNode* param = m_body->constructor()->m_parameters->parameters()[i];
                params += slice(source, param->identifier());
            }
            params += ")";
            *compose << params;
        } else {
            *compose << "()";
        }

        *compose << slice(source, m_body->constructor()->m_body->startByte(), m_body->constructor()->m_body->endByte()) << "\n";

    } else {
        *compose << indent(indentValue + 1) << "constructor(){\n"
                 << indent(indentValue + 2) << "super()\n"
                 << indent(indentValue + 2) << (isAnonymous() ? "new.target" : componentName) << ".prototype.__initialize.call(this)\n"
                 << indent(indentValue + 1) << "}\n";
    }

    *compose << indent(indentValue + 1) << "__initialize(){\n";

    if (m_id || !m_idComponents.empty())
        *compose << indent(indentValue + 2) << "this.ids = {}\n\n";

    if (m_id){
        *compose << indent(indentValue + 2) << "var " << slice(source, m_id) << " = this\n";
        *compose << indent(indentValue + 2) << "this.ids[\"" << slice(source, m_id) << "\"] = " << slice(source, m_id) << "\n\n";
    }

    for (size_t i = 0; i < m_idComponents.size();++i)
    {
        *compose << indent(indentValue + 2) << "var " << slice(source, m_idComponents[i]->id()) << " = new " << m_idComponents[i]->initializerName(source);
        if (m_idComponents[i]->arguments())
            *compose << slice(source, m_idComponents[i]->arguments()) << "\n";
        else
            *compose << "()\n";
        *compose << indent(indentValue + 2) << "this.ids[\"" << slice(source, m_idComponents[i]->id()) << "\"] = " << slice(source, m_idComponents[i]->id()) << "\n\n";
    }

    for (size_t i = 0; i < m_idComponents.size();++i)
    {
        std::string id = slice(source, m_idComponents[i]->id());
        auto properties = m_idComponents[i]->properties();

        for (uint32_t idx = 0; idx < properties.size(); ++idx){
            std::string propertyName = slice(source, properties[idx]->name());
            PropertyAccessorDeclarationNode::PropertyAccess accessPair;
            properties[idx]->convertToJs(source, id, indentValue + 2, ctx, accessPair, compose);
        }
    }

    for (size_t i = 0; i < m_properties.size(); ++i){
        std::string propertyName = slice(source, m_properties[i]->name());
        PropertyAccessorDeclarationNode::PropertyAccess accessPair = propertyAccessors(source, propertyName);
        m_properties[i]->convertToJs(source, "this", indentValue + 2, ctx, accessPair, compose);
    }

    for (size_t i = 0; i < m_events.size(); ++i){
        std::string paramList = "";
        if ( m_events[i]->parameterList() ){
            ParameterListNode* pdn = m_events[i]->parameterList()->as<ParameterListNode>();
            for ( auto it = pdn->parameters().begin(); it != pdn->parameters().end(); ++it ){
                if ( it != pdn->parameters().begin() )
                    paramList += ",";
                paramList += "[\'" + TypeNode::sliceWithoutAnnotation(source, (*it)->type()) + "\',\'" + slice(source, (*it)->identifier()) + "\']";
            }
        }

        *compose << indent(indentValue + 1) << (ConversionContext::baseComponentName(ctx) + ".addEvent(this, \'" + slice(source, m_events[i]->name()) + "\', [" + paramList + "])\n");
    }

    for (size_t i = 0; i < m_listeners.size(); ++i){

        std::string paramList = "";
        if ( m_listeners[i]->parameterList() ){
            ParameterListNode* pdn = m_listeners[i]->parameterList()->as<ParameterListNode>();
            for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                if ( pit != pdn->parameters().begin() )
                    paramList += ",";
                paramList += slice(source, (*pit)->identifier());
            }
        }

        *compose << indent(indentValue + 2) << "this.on(\'" << slice(source, m_listeners[i]->name()) << "\', function(" << paramList << ")";

        if ( m_listeners[i]->body() ){
            JSSection* jssection = new JSSection;
            jssection->from = m_listeners[i]->body()->startByte();
            jssection->to   = m_listeners[i]->body()->endByte();
            m_listeners[i]->body()->convertToJs(source, jssection->m_children, indentValue + 1, ctx);
            *compose << jssection << ".bind(this));\n";
        } else {
            JSSection* jssection = new JSSection;
            jssection->from = m_listeners[i]->bodyExpression()->startByte();
            jssection->to   = m_listeners[i]->bodyExpression()->endByte();
            m_listeners[i]->bodyExpression()->convertToJs(source, jssection->m_children, indentValue + 1, ctx);
            *compose << "{" << jssection << "}.bind(this));\n";
        }
    }

    for (size_t i = 0; i < m_properties.size(); ++i){

        std::string bindingsInJs = m_properties[i]->bindingIdentifiersToJs(source);

        if (bindingsInJs.size() > 0 && m_properties[i]->isBindingsAssignment() ){
            std::string comp = indent(indentValue + 1) + ConversionContext::baseComponentName(ctx) + ".assignPropertyExpression(this,\n"
                             + indent(indentValue + 1) + "'" + slice(source, m_properties[i]->name()) + "',\n";
            if (m_properties[i]->expression()){
                auto expr = m_properties[i]->expression();
                comp += indent(indentValue + 1) + "function(){ return ";
                el::JSSection* section = new el::JSSection;
                section->from = expr->startByte();
                section->to = expr->endByte();
                expr->convertToJs(source, section->m_children, indentValue + 1, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    comp += s;
                delete section;
                comp += "}.bind(this),\n";
            } else if (m_properties[i]->statementBlock()){
                auto block = m_properties[i]->statementBlock();
                comp += indent(indentValue + 1) + "(function()\n" ;
                el::JSSection* section = new el::JSSection;
                section->from = block->startByte();
                section->to = block->endByte();
                block->convertToJs(source, section->m_children, indentValue + 1, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    comp += s;
                delete section;
                comp += ".bind(this)\n),\n";
            }
            comp += indent(indentValue + 1) + bindingsInJs + "\n";
            comp += indent(indentValue + 1) + ")\n";
            *compose << comp;
        } else if ( m_properties[i]->hasAssignment() ){
            *compose << indent(indentValue + 2) << "this." << slice(source,m_properties[i]->name())
                     << " = ";
            if (m_properties[i]->expression()){
                auto expr = m_properties[i]->expression();
                // convert the subexpression
                JSSection* expressionSection = new JSSection;
                expressionSection->from = expr->startByte();
                expressionSection->to   = expr->endByte();
                expr->convertToJs(source, expressionSection->m_children, indentValue + 2, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;
                delete expressionSection;

                *compose << "\n";
            } else if (m_properties[i]->statementBlock()){
                auto block = m_properties[i]->statementBlock();
                *compose << "(function()" ;
                el::JSSection* section = new el::JSSection;
                section->from = block->startByte();
                section->to = block->endByte();
                block->convertToJs(source, section->m_children, indentValue + 1, ctx);
                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;
                delete section;
                *compose << ".bind(this))()\n";
            }
        }
    }

    for (size_t i = 0; i < m_assignments.size(); ++i){

        std::string bindingsInJs = m_assignments[i]->bindingIdentifiersToJs(source);

        if ( bindingsInJs.length() > 0 && m_assignments[i]->isBindingAssignment() ){
            if (m_assignments[i]->m_expression){
                auto& property = m_assignments[i]->m_property;
                std::string object = "this";
                for (size_t x = 0; x < property.size()-1; x++){
                    object += "." + slice(source, property[x]);
                }

                *compose << indent(indentValue + 2) << ConversionContext::baseComponentName(ctx) << ".assignPropertyExpression(" << object << ",\n"
                         << indent(indentValue + 3) << "'" << slice(source, property[property.size()-1])
                         << "',\n" << indent(indentValue + 3) << "function(){ return ";

                auto expr = m_assignments[i]->m_expression;
                JSSection* expressionSection = new JSSection;
                expressionSection->from = expr->startByte();
                expressionSection->to   = expr->endByte();
                expr->convertToJs(source, expressionSection->m_children, indentValue + 4, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;
                delete expressionSection;

                *compose << "}.bind(" << object << "),\n"
                         << indent(indentValue + 3) << bindingsInJs << "\n"
                         << indent(indentValue + 2) << ")\n";
            } else if ( m_assignments[i]->m_statementBlock ){
                auto& property = m_assignments[i]->m_property;
                std::string object = "this";
                for (size_t x = 0; x < property.size()-1; x++){
                    object += "." + slice(source, property[x]);
                }

                *compose << indent(indentValue + 2) << ConversionContext::baseComponentName(ctx) << ".assignPropertyExpression(" << object << ",\n"
                         << indent(indentValue + 3) << "'" << slice(source, property[property.size() - 1])
                         << "',\n" << indent(indentValue + 3) << "function()";

                auto expr = m_assignments[i]->m_statementBlock;
                JSSection* expressionSection = new JSSection;
                expressionSection->from = expr->startByte();
                expressionSection->to   = expr->endByte();
                expr->convertToJs(source, expressionSection->m_children, indentValue + 4, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;
                delete expressionSection;

                *compose << ".bind(" << object << "),\n"
                         << indent(indentValue + 2) << bindingsInJs << "\n"
                         << indent(indentValue + 1) << ")\n";
            }
        } else {
            if (m_assignments[i]->m_expression && !m_assignments[i]->m_property.empty()){
                *compose << indent(indentValue + 2) << "this";

                for (size_t prop = 0; prop < m_assignments[i]->m_property.size(); ++prop){
                    *compose << "." << slice(source, m_assignments[i]->m_property[prop]);
                }

                auto exp = m_assignments[i]->m_expression;
                std::string comp = "" ;
                el::JSSection* section = new el::JSSection;
                section->from = exp->startByte();
                section->to = exp->endByte();
                exp->convertToJs(source, section->m_children, indentValue + 1, ctx);
                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat){
                    comp += s;
                }
                delete section;

                *compose << " = " << comp << "\n"; //slice(source, m_assignments[i]->m_expression) << "\n\n";
            }
            else if (m_assignments[i]->m_statementBlock) {
                std::string propName = "this";

                for (size_t prop = 0; prop < m_assignments[i]->m_property.size(); ++prop){
                    propName += "." + slice(source, m_assignments[i]->m_property[prop]);
                }
                *compose << indent(indentValue + 2) << propName << " = " << "(function()";

                auto block = m_assignments[i]->m_statementBlock;

                el::JSSection* section = new el::JSSection;
                section->from = block->startByte();
                section->to = block->endByte();
                block->convertToJs(source, section->m_children, indentValue + 3, ctx);
                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat) {
                    *compose << s;
                }
                delete section;

                *compose << ".bind(this)())\n\n";
            }
        }
    }

    if (!m_nestedComponents.empty()){
        *compose << indent(indentValue + 2) << ConversionContext::baseComponentName(ctx) << ".assignChildren(this, [\n";
        for (size_t i = 0; i < m_nestedComponents.size(); ++i)
        {
            if (i != 0)
                *compose << indent(indentValue + 3) << ",\n";
            el::JSSection* section = new el::JSSection;
            section->from = m_nestedComponents[i]->startByte();
            section->to = m_nestedComponents[i]->endByte();
            m_nestedComponents[i]->convertToJs(source, section->m_children, indentValue + 3, ctx);
            std::vector<std::string> flat;
            section->flatten(source, flat);
            for (auto s: flat)
                *compose << s;
            delete section;
        }
        *compose << indent(indentValue + 2) << "])\n";
    }

    *compose << indent(indentValue + 1) << "}\n\n";

    for (size_t i = 0; i < m_propertyAccesors.size(); ++i){
        PropertyAccessorDeclarationNode* pa = m_propertyAccesors[i];
        if ( !pa->isPropertyAttached() ){
            std::string header;
            if ( pa->access() == PropertyAccessorDeclarationNode::Getter ){
                header += "get " + slice(source, pa->name()) + "()";
            } else if ( pa->access() == PropertyAccessorDeclarationNode::Setter ){
                std::string param;
                if ( pa->parameters() ){
                    ParameterListNode* pdn = pa->parameters()->as<ParameterListNode>();
                    if  ( pdn->parameters().size() > 0 ){
                        param += slice(source, pdn->parameters()[0]->identifier());
                    }
                }
                header += "set " + BaseNode::slice(source, pa->name()) + "(" + param + ")";
            }

            *compose << indent(indentValue + 1) << header;

            JSSection* jssection = new JSSection;
            jssection->from = pa->body()->startByte();
            jssection->to   = pa->body()->endByte();
            pa->body()->convertToJs(source, jssection->m_children, indentValue + 1, ctx);
            std::vector<std::string> flat;
            jssection->flatten(source, flat);
            for (auto s: flat){
                *compose << s << "\n";
            }
            delete jssection;
            *compose << "\n";
        }
    }

    for ( auto it = m_methods.begin(); it != m_methods.end(); ++it ){
        TypedMethodDeclarationNode* tfdn = *it;

        std::string paramList = "";
        if ( tfdn->parameters() ){
            ParameterListNode* pdn = tfdn->parameters()->as<ParameterListNode>();
            for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                if ( pit != pdn->parameters().begin() )
                    paramList += ",";
                paramList += slice(source, (*pit)->identifier());
            }
        }
        std::string annotations = "";
        if ( tfdn->isStatic() )
            annotations += "static ";
        if ( tfdn->isAsync() )
            annotations += "async ";
        *compose << indent(indentValue + 1) << annotations << slice(source, tfdn->name()) << "(" << paramList << ")";

        JSSection* jssection = new JSSection;
        jssection->from = tfdn->body()->startByte();
        jssection->to   = tfdn->body()->endByte();
        tfdn->body()->convertToJs(source, jssection->m_children, indentValue + 1, ctx);
        *compose << jssection << "\n";
    }

    *compose << indent(indentValue) << "}\n";

    for ( auto it = m_staticProperties.begin(); it != m_staticProperties.end(); ++it ){
        StaticPropertyDeclarationNode* spd = (*it)->as<StaticPropertyDeclarationNode>();
        *compose << indent(indentValue)  << componentName << "." << slice(source, spd->name());
        if ( spd->expression() ){
            *compose << " = ";
            JSSection* jssection = new JSSection;
            jssection->from = spd->expression()->startByte();
            jssection->to   = spd->expression()->endByte();
            spd->expression()->convertToJs(source, jssection->m_children, indentValue + 1, ctx);
            *compose << jssection << "\n";
        } else {
            *compose << "\n";
        }
    }

    fragments.push_back(compose);
}

PropertyAccessorDeclarationNode::PropertyAccess ComponentDeclarationNode::propertyAccessors(const std::string &source, const std::string &propertyName){
    PropertyAccessorDeclarationNode::PropertyAccess result;

    for ( size_t j = 0; j < m_propertyAccesors.size(); ++j ){
        PropertyAccessorDeclarationNode* pa = m_propertyAccesors[j];
        if ( slice(source, pa->name()) == propertyName ){
            if ( pa->access() == PropertyAccessorDeclarationNode::Getter ){
                result.getter = pa;
                pa->setIsPropertyAttached(true);
            } else if ( pa->access() == PropertyAccessorDeclarationNode::Setter ){
                result.setter = pa;
                pa->setIsPropertyAttached(true);
            }
        }
    }

    return result;
}

std::string ComponentDeclarationNode::name(const std::string& source) const{
    if ( !m_name )
        return "";

    std::string name = slice(source, m_name);

    if (name == "default"){
        const BaseNode* p = this;
        while (p && p->typeString() != "Program")
            p = p->parent();
        if (p) {
            const ProgramNode* program = dynamic_cast<const ProgramNode*>(p);
            if (program)
                name = program->fileName();
        }
    }

    return name;
}

bool ComponentDeclarationNode::isAnonymous() const{
    return !m_name;
}

void NewComponentExpressionNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &fragments, int indt, ConversionContext* ctx){
    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = startByte();
    compose->to   = endByte();

    if ( parent() && parent()->typeString() == "ComponentInstanceStatement" ){
        compose->from = parent()->startByte();
        std::string instanceName = parent()->as<ComponentInstanceStatementNode>()->name(source);
        *compose << "\nexport let " << instanceName << " = ";
    }
    *compose << indent(indt) << "(function(parent){\n" << indent(indt + 1) << "this.setParent(parent)\n";

    std::string id_root = "this";
    bool isRoot = (dynamic_cast<RootNewComponentExpressionNode*>(this) != nullptr);
    if (isRoot && (!m_idComponents.empty() || m_id ) ) {
        *compose << indent(indt + 1) << "this.ids = {}\n\n";
    }
    if (m_id) {
        *compose << indent(indt + 1) << ConversionContext::baseComponentName(ctx) << ".assignId(" << slice(source,m_id) << ", \"" << slice(source,m_id) << "\")\n";
        if (isRoot){
            id_root = slice(source,m_id);
            *compose << indent(indt + 1) << "var " << id_root << " = this\n";
            *compose << indent(indt + 1) << "this.ids[\"" << id_root << "\"] = " << id_root << "\n";
        }
    }

    if (isRoot && !m_idComponents.empty()){
        for (size_t i = 0; i < m_idComponents.size();++i){
            auto type = m_idComponents[i]->initializerName(source);
            *compose << indent(indt + 1) << "var " << slice(source, m_idComponents[i]->id()) << " = new " << type;
            if (m_idComponents[i]->arguments())
                *compose << slice(source, m_idComponents[i]->arguments()) << "\n";
            else
                *compose << "()\n";
            *compose << indent(indt + 1) << "this.ids[\"" << slice(source, m_idComponents[i]->id()) << "\"] = " << slice(source, m_idComponents[i]->id()) << "\n\n";
        }
    }

    if (isRoot || !m_id){
        for (size_t i = 0; i < m_properties.size(); ++i){
            std::string propertyName = slice(source, m_properties[i]->name());
            PropertyAccessorDeclarationNode::PropertyAccess accessPair;
            m_properties[i]->convertToJs(source, id_root, indt + 2, ctx, accessPair, compose);
        }
    }

    if (isRoot && !m_idComponents.empty()){
        for (size_t i = 0; i < m_idComponents.size();++i){
            std::string id = slice(source, m_idComponents[i]->id());
            auto properties = m_idComponents[i]->properties();

            for (size_t idx = 0; idx < properties.size(); ++idx){
                std::string propertyName = slice(source, properties[idx]->name());
                PropertyAccessorDeclarationNode::PropertyAccess accessPair;
                properties[idx]->convertToJs(source, id, indt + 1, ctx, accessPair, compose);
            }
        }
    }

    for (size_t i = 0; i < m_properties.size(); ++i){

        std::string bindingsInJs = m_properties[i]->bindingIdentifiersToJs(source);
        if (bindingsInJs.size() > 0 && m_properties[i]->isBindingsAssignment() ){
            std::string comp = "";
            if (m_properties[i]->expression()){
                comp += indent(indt + 1) + ConversionContext::baseComponentName(ctx) + ".assignPropertyExpression(this,\n"
                      + indent(indt + 2) + "'" + slice(source, m_properties[i]->name()) + "',\n"
                      + indent(indt + 2) + "function(){ return ";

                JSSection* expressionSection = new JSSection;
                expressionSection->from = m_properties[i]->expression()->startByte();
                expressionSection->to   = m_properties[i]->expression()->endByte();
                m_properties[i]->expression()->convertToJs(source, expressionSection->m_children, indt + 1, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    comp += s;

                comp += "}.bind(this),\n"
                      + indent(indt + 2) + bindingsInJs + "\n";
                comp += indent(indt + 1) +  + ")\n";
            } else {
                comp += indent(indt + 1) + ConversionContext::baseComponentName(ctx) + ".assignPropertyExpression(this,\n"
                      + indent(indt + 2) + "'" + slice(source, m_properties[i]->name()) + "',\n" + indent(indt + 2) + "function()";
                el::JSSection* section = new el::JSSection;
                auto block = m_properties[i]->statementBlock();
                section->from = block->startByte();
                section->to = block->endByte();
                block->convertToJs(source, section->m_children, indt + 1, ctx);
                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    comp += s;
                delete section;
                comp += ".bind(this),\n"
                      + indent(indt + 1) + bindingsInJs + "\n";
                comp += indent(indt + 1) + ")\n";
            }

            *compose << comp;
        } else if ( m_properties[i]->hasAssignment() ){
            if (m_properties[i]->expression()){
                auto expr = m_properties[i]->expression();
                *compose << indent(indt + 1) << "this." << slice(source,m_properties[i]->name()) << " = ";

                // convert the subexpression
                JSSection* expressionSection = new JSSection;
                expressionSection->from = expr->startByte();
                expressionSection->to   = expr->endByte();
                expr->convertToJs(source, expressionSection->m_children, indt + 1, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;

                *compose << "\n";
            } else if (m_properties[i]->statementBlock()) {
                *compose << indent(indt + 1) << "this." << slice(source,m_properties[i]->name()) << " = " << "(function()";
                el::JSSection* section = new el::JSSection;
                auto block = m_properties[i]->statementBlock();
                section->from = block->startByte();
                section->to = block->endByte();
                block->convertToJs(source, section->m_children, indt + 2, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    *compose <<  s;
                delete section;

                *compose << ".bind(this)())\n\n";
            }
        }
    }

    for (size_t i = 0; i < m_assignments.size(); ++i){
        if ( m_assignments[i]->m_property.size() == 0 )
            continue;

        std::string bindingsInJs = m_assignments[i]->bindingIdentifiersToJs(source);

        if ( bindingsInJs.length() > 0 ){ // has bindings
            if (m_assignments[i]->m_expression ){
                auto& property = m_assignments[i]->m_property;
                std::string object = "this";
                for (size_t x = 0; x < property.size() - 1; x++){
                    object += "." + slice(source, property[x]);
                }
                *compose << indent(indt + 1) << ConversionContext::baseComponentName(ctx) << ".assignPropertyExpression(" << object << ",\n"
                         << indent(indt + 2) << "'" << slice(source, property[property.size()-1]) << "',\n"
                         << indent(indt + 2) << "function(){ return ";

                el::JSSection* section = new el::JSSection;
                auto expr = m_assignments[i]->m_expression;
                section->from = expr->startByte();
                section->to = expr->endByte();
                expr->convertToJs(source, section->m_children, indt + 2, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    *compose <<  s;
                delete section;

                *compose << "}.bind(" << object << "),\n"
                         << indent(indt + 2) << bindingsInJs << "\n"
                         << indent(indt + 1) << ")\n";
            } else if ( m_assignments[i]->m_statementBlock ){
                auto& property = m_assignments[i]->m_property;
                std::string object = "this";
                for (size_t x = 0; x < property.size() - 1; x++){
                    object += "." + slice(source, property[x]);
                }
                *compose << indent(indt + 1) << ConversionContext::baseComponentName(ctx) << ".assignPropertyExpression(" << object << ",\n"
                         << indent(indt + 2) << "'" << slice(source, property[property.size()-1]) << "',\n"
                         << indent(indt + 2) << "function()";

                el::JSSection* section = new el::JSSection;
                auto block = m_assignments[i]->m_statementBlock;
                section->from = block->startByte();
                section->to = block->endByte();
                block->convertToJs(source, section->m_children, indt + 2, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    *compose <<  s;
                delete section;

                *compose << ".bind(" << object << "),\n"
                         << indent(indt + 2) << bindingsInJs << "\n"
                         << indent(indt + 1) << ")\n";
            }
        } else {
            if (m_assignments[i]->m_expression){
                *compose << indent(indt + 1) << "this";
                auto expr = m_assignments[i]->m_expression;
                for (size_t prop = 0; prop < m_assignments[i]->m_property.size(); ++prop){
                    *compose << "." << slice(source, m_assignments[i]->m_property[prop]);
                }
                *compose << " = ";

                // convert the subexpression
                JSSection* expressionSection = new JSSection;
                expressionSection->from = expr->startByte();
                expressionSection->to   = expr->endByte();
                expr->convertToJs(source, expressionSection->m_children, indt + 1, ctx);
                std::vector<std::string> flat;
                expressionSection->flatten(source, flat);
                for (auto s: flat)
                    *compose << s;

                *compose << "\n";

            } else if (m_assignments[i]->m_statementBlock) {
                std::string propName = "this";
                for (size_t prop = 0; prop < m_assignments[i]->m_property.size(); ++prop){
                    propName += "." + slice(source, m_assignments[i]->m_property[prop]);
                }
                *compose << indent(indt + 1) << propName << " = " << "(function()";

                el::JSSection* section = new el::JSSection;
                auto block = m_assignments[i]->m_statementBlock;
                section->from = block->startByte();
                section->to = block->endByte();
                block->convertToJs(source, section->m_children, indt + 2, ctx);

                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat)
                    *compose <<  s;
                delete section;

                *compose << ".bind(this)())\n";
            }
        }
    }

    for ( auto it = m_events.begin(); it != m_events.end(); ++it ){
        EventDeclarationNode* edn = *it;

        std::string paramList = "";
        if ( edn->parameterList() ){
            ParameterListNode* pdn = edn->parameterList()->as<ParameterListNode>();
            for ( auto it = pdn->parameters().begin(); it != pdn->parameters().end(); ++it ){
                if ( it != pdn->parameters().begin() )
                    paramList += ",";
                paramList += "[\'" + TypeNode::sliceWithoutAnnotation(source, (*it)->type()) + "\',\'" + slice(source, (*it)->identifier()) + "\']";
            }
        }
        *compose << indent(indt + 1) << (ConversionContext::baseComponentName(ctx) + ".addEvent(this, \'" + slice(source, edn->name()) + "\', [" + paramList + "])\n");
    }

    for ( auto it = m_listeners.begin(); it != m_listeners.end(); ++it ){
        ListenerDeclarationNode* ldn = *it;

        std::string paramList = "";
        if ( ldn->parameterList() ){
            ParameterListNode* pdn = ldn->parameterList()->as<ParameterListNode>();
            for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                if ( pit != pdn->parameters().begin() )
                    paramList += ",";
                paramList += slice(source, (*pit)->identifier());
            }
        }

        *compose << indent(indt + 1) << "this.on(\'" << slice(source, ldn->name()) << "\', function(" << paramList << ")";

        if ( ldn->body() ){
            JSSection* jssection = new JSSection;
            jssection->from = ldn->body()->startByte();
            jssection->to   = ldn->body()->endByte();
            ldn->body()->convertToJs(source, jssection->m_children, indt + 1, ctx);
            *compose << jssection << ".bind(this));\n";
        } else {
            JSSection* jssection = new JSSection;
            jssection->from = ldn->bodyExpression()->startByte();
            jssection->to   = ldn->bodyExpression()->endByte();
            ldn->bodyExpression()->convertToJs(source, jssection->m_children, indt + 1, ctx);
            *compose << "{" << jssection << "}.bind(this));\n";
        }
    }

    for ( auto it = m_methods.begin(); it != m_methods.end(); ++it ){
        TypedMethodDeclarationNode* tfdn = *it;

        std::string paramList = "";
        if ( tfdn->parameters() ){
            ParameterListNode* pdn = tfdn->parameters()->as<ParameterListNode>();
            for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                if ( pit != pdn->parameters().begin() )
                    paramList += ",";
                paramList += slice(source, (*pit)->identifier());
            }
        }
        JSSection* jssection = new JSSection;
        jssection->from = tfdn->body()->startByte();
        jssection->to   = tfdn->body()->endByte();
        tfdn->convertToJs(source, jssection->m_children, indt + 2, ctx);
        *compose << indent(indt + 1) << "this." << slice(source, tfdn->name()) << " = function(" << paramList << ")" << jssection << "\n";
    }

    if (!m_nestedComponents.empty()){
        *compose << indent(indt + 1) << ConversionContext::baseComponentName(ctx) << ".assignChildrenAndComplete(this, [\n";
        for (unsigned i = 0; i < m_nestedComponents.size(); ++i){
            if (i != 0) *compose << ",\n";
            el::JSSection* section = new el::JSSection;
            section->from = m_nestedComponents[i]->startByte();
            section->to = m_nestedComponents[i]->endByte();
            m_nestedComponents[i]->convertToJs(source, section->m_children, indt + 2, ctx);
            std::vector<std::string> flat;
            section->flatten(source, flat);
            for (auto s: flat)
                *compose << s << "\n";
            delete section;
        }
        *compose << indent(indt + 1) << "])\n";
    } else {
        *compose << indent(indt + 1) << ConversionContext::baseComponentName(ctx) << ".complete(this)\n";
    }

    *compose << indent(indt + 1) << "return this\n" << indent(indt) << "}.bind(";

    if (!m_id || isRoot)
    {
        *compose << "new ";
        std::string name;
        for ( auto nameIden : m_name ){
            if ( !name.empty() )
                name += ".";
            name += slice(source, nameIden);
        }
        *compose << name;

        if (!m_arguments)
            *compose << "()";
        else
            *compose << slice(source, m_arguments);
    } else {
        *compose << slice(source, m_id);
    }

    bool isThis = parent() && parent()->typeString() == "ComponentBody";
    isThis = isThis || (parent()
                        && parent()->parent()
                        && parent()->parent()->typeString() == "PropertyDeclaration"
                        && parent()->parent()->parent()
                        && parent()->parent()->parent()->typeString() == "ComponentBody" );

    if (isThis){
        *compose << ")(this)";
    } else {
        *compose << ")(null)";
    }

    *compose << ")\n";

    fragments.push_back(compose);
}

NewComponentExpressionNode::NewComponentExpressionNode(const TSNode &node, const std::string& typeString)
    : JsBlockNode(node, typeString)
    , m_id(nullptr)
    , m_body(nullptr)
    , m_arguments(nullptr)
{

}

NewComponentExpressionNode::NewComponentExpressionNode(const TSNode &node):
    NewComponentExpressionNode(node, "NewComponentExpression")
{

}


std::string NewComponentExpressionNode::initializerName(const std::string &source){
    std::string name;
    for ( auto nameIden : m_name ){
        if ( !name.empty() )
            name += ".";
        name += slice(source, nameIden);
    }
    return name;
}

std::string NewComponentExpressionNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    std::string name =
        "(name " +
        (!m_name.empty()
            ? std::string("[") + std::to_string(m_name.front()->startByte()) + ", " +
              std::to_string(m_name.back()->endByte()) + "]" : "") + ")";

    std::string identifier = "";
    if ( m_id )
        identifier = "(id " + m_id->rangeString() + ")";


    std::string args = "";
    if (m_arguments)
        args = "(args " + m_arguments->rangeString() + ")";

    result += typeString() + " " + name + identifier +args + rangeString() + "\n";
    if ( m_body ){
        result += m_body->toString(indent >= 0 ? indent + 1 : indent);
    }

    if (m_arguments)
        result += m_arguments->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

std::string ComponentBodyNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += "ComponentBody " + rangeString() + "\n";

    const std::vector<BaseNode*>& c = children();
    for ( BaseNode* node : c ){
        result += node->toString(indent >= 0 ? indent + 1 : indent);
    }

    return result;
}

PropertyDeclarationNode::PropertyDeclarationNode(const TSNode &node)
    : JsBlockNode(node, "PropertyDeclaration")
    , m_name(nullptr)
    , m_type(nullptr)
    , m_isBindingAssignment(true)
    , m_expression(nullptr)
    , m_statementBlock(nullptr)
    , m_bindingContainer(new PropertyBindingContainer)
{
    m_bindingContainer->setDeclarationCheck([this](const std::string& source, const std::string& name, BaseNode* m){
        BaseNode* parent = m->parent();
        while ( parent ){
            if ( parent == this ){
                break;
            }
            JsBlockNode* hasIds = dynamic_cast<JsBlockNode*>(parent);
            if (hasIds){
                for (auto it = hasIds->identifiers().begin(); it != hasIds->identifiers().end(); ++it){
                    std::string declaredId = slice(source, *it);
                    if (declaredId == name){
                        return true;
                    }
                }
            }
            parent = parent->parent();
        }
        return false;
    });
}

PropertyDeclarationNode::~PropertyDeclarationNode(){
    delete m_bindingContainer;
}

std::string PropertyDeclarationNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    std::string name = "";
    if ( m_name )
        name = "(name " + m_name->rangeString() + ")";
    std::string type = "";
    if ( m_type )
        type = "(type " + m_type->rangeString() + ")";
    result += "PropertyDeclaration " + rangeString() + name + type + "\n";

    if ( m_expression )
        result += m_expression->toString(indent >= 0 ? indent + 1 : indent);

    if ( m_statementBlock )
        result += m_statementBlock->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

void PropertyDeclarationNode::pushToBindings(BaseNode *bn){
    m_bindingContainer->addBinding(bn);
}

std::string PropertyDeclarationNode::bindingIdentifiersToString(const std::string &source) const{
    return m_bindingContainer->bindingIdentifiersToString(source);
}

std::string PropertyDeclarationNode::bindingIdentifiersToJs(const std::string &source) const{
    return m_bindingContainer->bindingIdentifiersToJs(source);
}

void PropertyDeclarationNode::convertToJs(
        const std::string &source,
        const std::string &componentReference,
        int indt,
        BaseNode::ConversionContext *ctx,
        const PropertyAccessorDeclarationNode::PropertyAccess &propertyAccess,
        ElementsInsertion* compose)
{
    *compose << indent(indt) << ConversionContext::baseComponentName(ctx) << ".addProperty(" + componentReference + ", '" << slice(source, name())
             << "', { type: '" << (type() ? slice(source, type()) : "") << "', notify: '"
             << slice(source, name()) << "Changed'";


    if ( propertyAccess.getter ){
        *compose << ", get: function()";
        JSSection* jssection = new JSSection;
        jssection->from = propertyAccess.getter->body()->startByte();
        jssection->to   = propertyAccess.getter->body()->endByte();
        propertyAccess.getter->body()->convertToJs(source, jssection->m_children, indt + 1, ctx);
        std::vector<std::string> flat;
        jssection->flatten(source, flat);
        for (auto s: flat){
            *compose << s << "\n";
        }
        delete jssection;
    }
    if ( propertyAccess.setter ){
        *compose << ", set: function(" << (propertyAccess.setter->firstParameterName() ? slice(source, propertyAccess.setter->firstParameterName()) : "") << ")";
        JSSection* jssection = new JSSection;
        jssection->from = propertyAccess.setter->body()->startByte();
        jssection->to   = propertyAccess.setter->body()->endByte();
        propertyAccess.setter->body()->convertToJs(source, jssection->m_children, indt + 1, ctx);
        std::vector<std::string> flat;
        jssection->flatten(source, flat);
        for (auto s: flat){
            *compose << s << "\n";
        }
        delete jssection;
    }


    *compose << "})\n";
}

StaticPropertyDeclarationNode::StaticPropertyDeclarationNode(const TSNode &node)
    : BaseNode(node, "StaticPropertyDeclaration")
    , m_name(nullptr)
    , m_type(nullptr)
    , m_expression(nullptr)
{
}

std::string StaticPropertyDeclarationNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    std::string name = "";
    if ( m_name )
        name = "(name " + m_name->rangeString() + ")";
    std::string type = "";
    if ( m_type )
        type = "(type " + m_type->rangeString() + ")";
    result += "StaticPropertyDeclaration " + rangeString() + name + type + "\n";

    if ( m_expression )
        result += m_expression->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}


PropertyAssignmentNode::PropertyAssignmentNode(const TSNode &node)
    : JsBlockNode(node, "PropertyAssignment")
    , m_isBindingAssignment(true)
    , m_expression(nullptr)
    , m_statementBlock(nullptr)
    , m_bindingContainer(new PropertyBindingContainer)
{
    m_bindingContainer->setDeclarationCheck([this](const std::string& source, const std::string& name, BaseNode* m){
        BaseNode* parent = m->parent();
        while ( parent ){
            if ( parent == this ){
                break;
            }
            JsBlockNode* hasIds = dynamic_cast<JsBlockNode*>(parent);
            if (hasIds){
                for (auto it = hasIds->identifiers().begin(); it != hasIds->identifiers().end(); ++it){
                    std::string declaredId = slice(source, *it);
                    if (declaredId == name){
                        return true;
                    }
                }
            }
            parent = parent->parent();
        }
        return false;
    });
}

PropertyAssignmentNode::~PropertyAssignmentNode(){
    delete m_bindingContainer;
}

std::string PropertyAssignmentNode::toString(int indent) const{
    std::string result;
   if ( indent > 0 )
       result.assign(indent * 2, ' ');
    std::string name = "";
    result += "PropertyAssignment " + rangeString() + name + "\n";
    if ( m_expression )
        result += m_expression->toString(indent >= 0 ? indent + 1 : indent);
    return result;
}

void PropertyAssignmentNode::pushToBindings(BaseNode *bn){
    m_bindingContainer->addBinding(bn);
}

std::string PropertyAssignmentNode::bindingIdentifiersToString(const std::string &source) const{
    return m_bindingContainer->bindingIdentifiersToString(source);
}

std::string PropertyAssignmentNode::bindingIdentifiersToJs(const std::string &source) const{
    return m_bindingContainer->bindingIdentifiersToJs(source);
}

EventDeclarationNode::EventDeclarationNode(const TSNode &node)
    : BaseNode(node, "EventDeclaration")
    , m_name(nullptr)
    , m_parameters(nullptr)
{}

std::string EventDeclarationNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');
    std::string name = "";
    if ( m_name )
        name = "(name " + m_name->rangeString() + ")";
    result += "EventDeclaration " + rangeString() + name + "\n";
    if ( m_parameters )
        result += m_parameters->toString(indent >= 0 ? indent + 1 : indent);
    return result;
}

std::vector<std::string> MemberExpressionNode::identifierChain(const std::string &source) const{
    std::vector<std::string> result;
    for ( auto child : children() ){
        if ( child->typeString() == "Identifier" ){
            result.push_back(BaseNode::slice(source, child));
        } else if ( child->canCast<MemberExpressionNode>() ){
            auto nestedChain = child->as<MemberExpressionNode>()->identifierChain(source);
            result.insert( result.end(), nestedChain.begin(), nestedChain.end() );
        }
    }
    return result;
}

std::string ParameterListNode::toString(int indent) const{
    std::string result;
   if ( indent > 0 )
       result.assign(indent * 2, ' ');

    std::string parameters = "(no parameters: " + std::to_string(m_parameters.size()) + ")";

    result += "ParameterList " + rangeString() + parameters + "\n";
    return result;
}

ListenerDeclarationNode::ListenerDeclarationNode(const TSNode &node)
    : BaseNode(node, "ListenerDeclaration")
    , m_name(nullptr)
    , m_parameters(nullptr)
    , m_body(nullptr)
    , m_bodyExpression(nullptr)
{}

std::string ListenerDeclarationNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');
    std::string name = "";
    if ( m_name )
        name = "(name " + m_name->rangeString() + ")";

    result += "ListenerDeclaration " + rangeString() + name + "\n";
    if ( m_parameters )
        result += m_parameters->toString(indent >= 0 ? indent + 1 : indent);
    if ( m_body )
        result += m_body->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

MethodDefinitionNode::MethodDefinitionNode(const TSNode &node)
    : BaseNode(node, "MethodDefinition")
    , m_name(nullptr)
    , m_parameters(nullptr)
    , m_body(nullptr)
{
}

std::string MethodDefinitionNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');
    std::string name = "";
    if ( m_name )
        name = "(name " + m_name->rangeString() + ")";

    result += "MethodDefinition " + rangeString() + name + "\n";
    if ( m_parameters )
        result += m_parameters->toString(indent >= 0 ? indent + 1 : indent);
    if ( m_body )
        result += m_body->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}


ArrowFunctionNode::ArrowFunctionNode(const TSNode &node)
    : JsBlockNode(node, "ArrowFunction")
    , m_name(nullptr)
    , m_body(nullptr)
{
}

std::string ArrowFunctionNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');
    std::string name = "";
    if ( m_name )
        name = "(name " + m_name->rangeString() + ")";

    result += "ArrowFunction " + rangeString() + name + "\n";

    if ( m_parameters.size() > 0 ){
        std::string paramsResult;
        if ( indent > 0 )
            paramsResult.assign(indent * 2, ' ');
        std::string parameters = "(no parameters: " + std::to_string(m_parameters.size()) + ")";
        result += paramsResult + "ParameterList " + rangeString() + parameters + "\n";
    }

    if ( m_body )
        result += m_body->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}


FunctionNode::FunctionNode(const TSNode &node)
    : BaseNode(node, "FunctionDeclaration")
    , m_parameters(nullptr)
    , m_body(nullptr)
{
}

std::string FunctionNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += "Function " + rangeString() + "\n";

    if ( m_parameters->parameters().size() > 0 ){
        std::string paramsResult;
        if ( indent > 0 )
            paramsResult.assign(indent * 2, ' ');
        std::string parameters = "(no parameters: " + std::to_string(m_parameters->parameters().size()) + ")";
        result += paramsResult + "ParameterList " + rangeString() + parameters + "\n";
    }

    if ( m_body )
        result += m_body->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

FunctionNode::FunctionNode(const TSNode &node, const std::string &nodeType)
    : BaseNode(node, nodeType)
    , m_body(nullptr)
{
}


FunctionDeclarationNode::FunctionDeclarationNode(const TSNode &node)
    : FunctionNode(node, "FunctionDeclaration")
    , m_name(nullptr)
{
}

std::string FunctionDeclarationNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');
    std::string name = "";
    if ( m_name )
        name = "(name " + m_name->rangeString() + ")";

    result += "FunctionDeclaration " + rangeString() + name + "\n";

    if ( parameters() && parameters()->parameters().size() > 0 ){
        std::string paramsResult;
        if ( indent > 0 )
            paramsResult.assign(indent * 2, ' ');
        std::string params = "(no parameters: " + std::to_string(parameters()->parameters().size()) + ")";
        result += paramsResult + "ParameterList " + rangeString() + params + "\n";
    }

    if ( body() )
        result += body()->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}


TypedMethodDeclarationNode::TypedMethodDeclarationNode(const TSNode &node)
    : BaseNode(node, "TypedMethodDeclaration")
    , m_name(nullptr)
    , m_parameters(nullptr)
    , m_body(nullptr)
    , m_async(false)
    , m_static(false)
{
}

TypedMethodDeclarationNode::TypedMethodDeclarationNode(const TSNode &node, const std::string &typeString)
    : BaseNode(node, typeString)
    , m_name(nullptr)
    , m_parameters(nullptr)
    , m_body(nullptr)
{
}

std::string TypedMethodDeclarationNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');
    std::string name = "";
    if ( m_name )
        name = "(name " + m_name->rangeString() + ")";

    result += "TypedMethodDeclaration " + rangeString() + name + "\n";
    if ( m_parameters )
        result += m_parameters->toString(indent >= 0 ? indent + 1 : indent);
    if ( m_body )
        result += m_body->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

std::string ConstructorDefinitionNode::toString(int indent) const
{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += "ConstructorDefinition " + rangeString() + "\n";

    if ( m_parameters && m_parameters->parameters().size() > 0 ){
        std::string paramsResult;
        if ( indent > 0 )
            paramsResult.assign(indent * 2, ' ');
        std::string params = "(no parameters: " + std::to_string(m_parameters->parameters().size()) + ")";
        result += paramsResult + "ParameterList " + rangeString() + params + "\n";
    }

    if (m_body)
        result += m_body->toString(indent >= 0 ? indent + 1 : indent);

    for (auto it = children().begin(); it != children().end(); ++it)
        result += (*it)->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

std::string ExpressionStatementNode::toString(int indent) const
{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += "ExpressionStatement " + rangeString() + "\n";
    for (auto it = children().begin(); it != children().end(); ++it)
        result += (*it)->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

std::string AssignmentExpressionNode::toString(int indent) const{

    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += "AssignmentExpression" + rangeString() + "\n";
    for (auto it = children().begin(); it != children().end(); ++it)
        result += (*it)->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

void AssignmentExpressionNode::addChild(BaseNode *child){
    TSNode s = current();
    TSNode left = ts_node_child_by_field_name(s, "left", 4);
    TSNode right = ts_node_child_by_field_name(s, "right", 5);
    if ( !ts_node_is_null(left) && !ts_node_is_null(right) ){
        TSNode childNode = child->current();
        if ( ts_node_eq(left, childNode) ){
            m_left = child;
        } else if ( ts_node_eq(right, childNode ) ){
            m_right = child;
        }
    }

    BaseNode::addChild(child);
}

std::string CallExpressionNode::toString(int indent) const
{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += "CallExpression " + rangeString() + (isSuper ? " SUPER" : "") + "\n";
    if (m_arguments)
        result += m_arguments->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

std::string NewTaggedComponentExpressionNode::toString(int indent) const
{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += "NewTaggedComponentExpression " + rangeString() + "\n";

    for (auto it = children().begin(); it != children().end(); ++it)
        result += (*it)->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

void NewTaggedComponentExpressionNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &fragments, int indentValue, ConversionContext* ctx)
{
    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = startByte();
    compose->to = endByte();
    std::string name, value;
    for (auto child: children())
    {
        if (child->typeString() == "Identifier") {
            name = slice(source, child);
        } else {
            std::string sourceVal = slice(source, child);
            sourceVal = sourceVal.substr(1, sourceVal.length() - 2);

            enum State{
                Default,
                Space,
                Escape
            };
            value = "";

            if ( !sourceVal.empty() ){

                State state = Space;
                size_t i = 0;

                if ( sourceVal[0] == ' ' ){
                    value += ' ';
                }

                while (i < sourceVal.length()){
                    if ( sourceVal[i] == '\n' || sourceVal[i] == '\t' ){
                        if ( state == Default ){
                            value += ' ';
                            state = Space;
                        } else if ( state == Escape ){
                            value += "\\\\";
                            state = Space;
                        } // do nothing for space state
                    } else if ( sourceVal[i] == '\r' ){
                        if ( state == Escape ){
                            value += '\\';
                            state = Default;
                        }
                    } else if ( sourceVal[i] == ' ' ){
                        if ( state == Default ){
                            value += ' ';
                            state = Space;
                        } else if ( state == Escape ){
                            value += '\\';
                            state = Space;
                        } // do nothing for space state
                    } else if ( sourceVal[i] == '\\' ){
                        if ( state == Escape ){
                            value += "\\\\";
                            state = Default;
                        } else {
                            state = Escape;
                        }
                    } else if ( sourceVal[i] == 's' ){
                        if ( state == Escape ){
                            value += ' ';
                            state = Default;
                        } else {
                            value += 's';
                        }
                    } else {
                        if ( state == Escape ){
                            value += '\\';
                        }
                        value += sourceVal[i];
                        state = Default;
                    }
                    ++i;
                }
            }
        }
    }

    *compose << indent(indentValue + 0) << "(function(parent){\n";
    *compose << indent(indentValue + 1) << "this.setParent(parent)\n";
    *compose << indent(indentValue + 1) << ConversionContext::baseComponentName(ctx) << ".complete(this)\n";
    *compose << indent(indentValue + 1) << "return this\n";
    *compose << indent(indentValue + 0) << "}.bind(new " << name << "(\"" << value << "\"))(this))\n";

    fragments.push_back(compose);
}

std::string NewTrippleTaggedComponentExpressionNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += "NewTrippleTaggedComponentExpression " + rangeString() + "\n";

    for (auto it = children().begin(); it != children().end(); ++it)
        result += (*it)->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

void NewTrippleTaggedComponentExpressionNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &fragments, int indentValue, ConversionContext* ctx)
{
    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = startByte();
    compose->to = endByte();
    std::string name, value;

    for (auto child: children())
    {
        if (child->typeString() == "Identifier") {
            name = slice(source, child);
        } else {
            value = slice(source, child);
            value = value.substr(3, value.length() - 6);

            size_t startAt = value.find('\n');
            size_t endAt   = value.rfind('\n');

            if ( startAt != std::string::npos ){
                startAt += 1;
                size_t len = endAt - startAt;
                if ( len > 0 ){
                    value = value.substr(startAt, len);
                } else {
                    value = "";
                }
            }

            Utf8::replaceAll(value, "\r", "");
            Utf8::replaceAll(value, "\n", "\\n");
            Utf8::replaceAll(value, "\t", "\\t");
        }
    }

    *compose << indent(indentValue + 0) << "(function(parent){\n";
    *compose << indent(indentValue + 1) << "this.setParent(parent)\n";
    *compose << indent(indentValue + 1) << ConversionContext::baseComponentName(ctx) << ".complete(this)\n";
    *compose << indent(indentValue + 1) << "return this\n";
    *compose << indent(indentValue + 0) << "}.bind(new " << name << "(\"" << value << "\"))(this))\n";

    fragments.push_back(compose);
}


std::string ArgumentsNode::toString(int indent) const
{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += "Arguments " + rangeString() + "\n";

    for (auto it = children().begin(); it != children().end(); ++it)
        result += (*it)->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

std::string VariableDeclarationNode::toString(int indent) const
{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += "VariableDeclaration " + rangeString() + "\n";

    for (size_t i = 0; i < m_declarators.size(); i++)
    {
        result += m_declarators[i]->toString(indent >= 0 ? indent + 1 : indent);
    }

    for (auto it = children().begin(); it != children().end(); ++it)
        result += (*it)->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

void JsBlockNode::collectImports(const std::string &source, std::vector<IdentifierNode *> &identifiers, ConversionContext *ctx){
    BaseNode::collectImports(source, identifiers, ctx);
    collectBlockImports(source, identifiers, ctx);
}

void JsBlockNode::collectBlockImports(const std::string &source, std::vector<IdentifierNode *> &identifiers, ConversionContext* ctx){
    for ( auto identifier : m_usedIdentifiers ){
        if ( !checkIdentifierDeclared(source, this, slice(source, identifier), ctx) ){
            identifiers.push_back(identifier);
        }
    }
}

std::string ComponentInstanceStatementNode::name(const std::string &source) const{
    if ( !m_name )
        return "";

    std::string instance_name = slice(source, m_name);

    if (instance_name == "default"){
        const BaseNode* p = this;
        while (p && p->typeString() != "Program")
            p = p->parent();
        if (p) {
            const ProgramNode* program = dynamic_cast<const ProgramNode*>(p);
            if (program)
                instance_name = program->fileName();
        }
    }

    return instance_name;
}

IdentifierNode *PropertyAccessorDeclarationNode::firstParameterName() const{
    if ( parameters() ){
        ParameterListNode* pdn = parameters()->as<ParameterListNode>();
        if  ( pdn->parameters().size() > 0 ){
            return pdn->parameters()[0]->identifier();
        }
    }
    return nullptr;
}

std::string TypeNode::sliceWithoutAnnotation(const std::string& source, TypeNode *tn){
    if ( !tn ){
        return "";
    }

    if ( strcmp(ts_node_type(tn->current()), "type_annotation") == 0 ){
        auto count = ts_node_child_count(tn->current());
        if ( count > 1 ){
            uint32_t start = ts_node_start_byte(ts_node_child(tn->current(), 1));
            uint32_t end   = ts_node_end_byte(ts_node_child(tn->current(), count - 1));
            return slice(source, start, end);
        }
        return slice(source, tn);
    }
    return slice(source, tn);
}

}} // namespace lv, el

