#include "languagenodes_p.h"

#include <QDebug>
#include <queue>
#include <map>
#include <vector>
#include <algorithm>

namespace lv{ namespace el{

std::string indent(int i)
{
    std::string res;
    res.append(4*i, ' ');
    return res;
}

BaseNode::BaseNode(const TSNode &node, const std::string &typeString)
    : m_node(node)
    , m_typeString(typeString)
    , m_parent(nullptr)
{}

BaseNode::~BaseNode(){
    for ( auto it = m_children.begin(); it != m_children.end(); ++it ){
        delete *it;
    }
}

BaseNode *BaseNode::visit(LanguageParser::AST *ast){
    TSTree* tree = reinterpret_cast<TSTree*>(ast);
    TSNode root_node = ts_tree_root_node(tree);

    ProgramNode* node = new ProgramNode(root_node);

    uint32_t count = ts_node_child_count(root_node);

    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(root_node, i);
        visit(node, child);
    }

    return node;
}

void BaseNode::checkIdentifierDeclared(const std::string& source, BaseNode *node, std::string id)
{
    if (id == "this" || id == "parent" || id == "Element" || id == "Container") return;
    BaseNode* prog = nullptr;
    while (node)
    {
        JsBlockNode* hasIds = dynamic_cast<JsBlockNode*>(node);
        if (!node->parent())
        {
            prog = node;
        }
        if (hasIds)
        {
            for (auto it = hasIds->m_declarations.begin(); it != hasIds->m_declarations.end(); ++it)
            {
                std::string declaredId = slice(source, *it);
                if (declaredId == id) return;
            }
        }

        node = node->parent();
    }

    // at this point, we should've reached the Program node
    ProgramNode* program = dynamic_cast<ProgramNode*>(prog);
    program->undeclared().insert(id);
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

std::string BaseNode::rangeString() const{
    return std::string("[") + std::to_string(startByte()) + ", " + std::to_string(endByte()) + "]";
}

void BaseNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &sections, int indentValue){
    for ( BaseNode* node : m_children ){
        node->convertToJs(source, sections, indentValue);
    }
}

std::string BaseNode::slice(const std::string &source, uint32_t start, uint32_t end){
    return source.substr(start, end - start);
}

std::string BaseNode::slice(const std::string &source, BaseNode *node){
    return slice(source, node->startByte(), node->endByte());
}

void BaseNode::moveToDeclarations(BaseNode *parent, IdentifierNode *idNode)
{
    BaseNode* p = parent;
    while (p)
    {
        JsBlockNode* jsbn = dynamic_cast<JsBlockNode*>(p);
        if (jsbn)
        {
            jsbn->m_declarations.push_back(idNode);
            break;
        }
        p = p->parent();
    }
}

void BaseNode::insert(BaseNode *child){
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
    } else if ( strcmp(ts_node_type(node), "identifier") == 0 ){
        visitIdentifier(parent, node);
    } else if ( strcmp(ts_node_type(node), "constructor_definition") == 0 ){
        visitConstructorDefinition(parent, node);
    } else if ( strcmp(ts_node_type(node), "property_identifier") == 0 ){
        visitIdentifier(parent, node);
    } else if ( strcmp(ts_node_type(node), "this") == 0 ){
        visitIdentifier(parent, node);
    } else if ( strcmp(ts_node_type(node), "import_path") == 0 ){
        visitImportPath(parent, node);
    } else if ( strcmp(ts_node_type(node), "component_declaration") == 0 ){
        visitComponentDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "new_component_expression") == 0 ){
        visitNewComponentExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "component_body") == 0 ){
        visitComponentBody(parent, node);
    } else if ( strcmp(ts_node_type(node), "class_declaration") == 0 ){
        visitClassDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "property_declaration") == 0 ){
        visitPropertyDeclaration(parent, node);
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
    } else if ( strcmp(ts_node_type(node), "typed_function_declaration") == 0 ){
        visitTypedFunctionDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "function_declaration") == 0 ){
        visitFunctionDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "number") == 0 ){
        visitNumber(parent, node);
    } else if ( strcmp(ts_node_type(node), "expression_statement") == 0 ){
        visitExpressionStatement(parent, node);
    } else if ( strcmp(ts_node_type(node), "call_expression") == 0 ){
        visitCallExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "new_tagged_component_expression") == 0 ){
        visitNewTaggedComponentExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "tagged_type_string") == 0 ){
        visitTaggedString(parent, node);
    } else if ( strcmp(ts_node_type(node), "variable_declaration") == 0 ){
        visitVariableDeclaration(parent, node);
    } else if ( strcmp(ts_node_type(node), "new_expression") == 0 ){
        visitNewExpression(parent, node);
    } else if ( strcmp(ts_node_type(node), "return_statement") == 0 ){
        visitReturnStatement(parent, node);
    }else {
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
    parent->insert(importNode);
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "import_as" ) == 0 ){
            TSNode aliasChild = ts_node_child(child, 1);
            IdentifierNode* in = new IdentifierNode(aliasChild);
            importNode->m_importAs = in;
            in->setParent(importNode);
            moveToDeclarations(parent, in);
        } else if ( strcmp(ts_node_type(child), ".") == 0){
            importNode->m_isRelative = true;
        } else if ( strcmp(ts_node_type(child), "import_path") == 0 ){
            visitImportPath(importNode, child);
        }
    }
    visitChildren(importNode, node);
}

void BaseNode::visitIdentifier(BaseNode *parent, const TSNode &node){
    IdentifierNode* identifierNode = new IdentifierNode(node);
    parent->insert(identifierNode);
    visitChildren(identifierNode, node);
}

void BaseNode::visitImportPath(BaseNode *parent, const TSNode &node){
    ImportPathNode* ipnode = new ImportPathNode(node);
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "identifier") == 0 ){
            IdentifierNode* in = new IdentifierNode(child);
            ipnode->m_segments.push_back(in);
            in->setParent(ipnode);
        }
    }
    parent->insert(ipnode);
}

void BaseNode::visitComponentDeclaration(BaseNode *parent, const TSNode &node){
    ComponentDeclarationNode* enode = new ComponentDeclarationNode(node);
    parent->insert(enode);
    if (parent->typeString() == "PropertyDeclaration")
    {
        static_cast<PropertyDeclarationNode*>(parent)->m_componentDeclaration = enode;
    }
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
            enode->m_name->setParent(enode);
            moveToDeclarations(parent, enode->m_name);
        } else if ( strcmp(ts_node_type(child), "component_heritage") == 0 ){
            uint32_t heritageCount = ts_node_child_count(child);
            for ( uint32_t j = 0; j < heritageCount; ++j ){
                TSNode heritageSegment = ts_node_child(child, j);
                if ( strcmp(ts_node_type(heritageSegment), "identifier" ) == 0 ){
                    IdentifierNode* heritageSegmentNode = new IdentifierNode(heritageSegment);
                    enode->m_heritage.push_back(heritageSegmentNode);
                    heritageSegmentNode->setParent(enode);
                }
            }
        } else if ( strcmp(ts_node_type(child), "component_body") == 0 ){
            enode->m_body = new ComponentBodyNode(child);
            enode->m_body->setParent(enode);
            visitChildren(enode->m_body, child);
        } else if ( strcmp(ts_node_type(child), "component_identifier") == 0 ){
            if ( ts_node_named_child_count(child) > 0 ){
                TSNode idChild = ts_node_child(child, 1);
                enode->m_id = new IdentifierNode(idChild);
                enode->m_id->setParent(enode);
                moveToDeclarations(parent, enode->m_id);
            }
        }
    }
}

void BaseNode::visitComponentBody(BaseNode *parent, const TSNode &node){
    ComponentBodyNode* enode = new ComponentBodyNode(node);
    parent->insert(enode);
    visitChildren(enode, node);
}

void BaseNode::visitNewComponentExpression(BaseNode *parent, const TSNode &node){
    NewComponentExpressionNode* enode = new NewComponentExpressionNode(node);
    parent->insert(enode);
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
            enode->m_name->setParent(enode);
        } else if ( strcmp(ts_node_type(child), "component_body") == 0 ){
            enode->m_body = new ComponentBodyNode(child);
            enode->m_body->setParent(enode);
            visitChildren(enode->m_body, child);
        } else if ( strcmp(ts_node_type(child), "arguments") == 0 ){
            enode->m_arguments = new ArgumentsNode(child);
            enode->m_arguments->setParent(enode);
            visitChildren(enode->m_arguments, child);
        } else if ( strcmp(ts_node_type(child), "component_identifier") == 0 ){
            TSNode id = ts_node_child(child, 1);
            if (strcmp(ts_node_type(id), "identifier") != 0) continue;
            enode->m_id = new IdentifierNode(id);
            enode->m_id->setParent(enode);
        } else if ( strcmp(ts_node_type(child), "component_instance") == 0 ){
            TSNode id = ts_node_child(child, 1);
            if (strcmp(ts_node_type(id), "identifier") != 0) continue;
            enode->m_instance = new IdentifierNode(id);
            enode->m_instance->setParent(enode);
        }
    }

    // for id components
    BaseNode* p = parent;
    while (enode->m_id && p)
    {
        if (p->typeString() == "ComponentBody")
        {
            auto body = p->as<ComponentBodyNode>();
            if (body->parent()->typeString() == "ComponentDeclaration")
            {
                auto decl = body->parent()->as<ComponentDeclarationNode>();
                decl->pushToIdComponents(enode);
                break;
            }
        } else if (p->typeString() == "Program"){
            auto prog = p->as<ProgramNode>();
            prog->m_idComponents.push_back(enode);
        }

        p = p->parent();
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
        else if (body->parent()->typeString() == "NewComponentExpression")
        {
            auto expr = body->parent()->as<NewComponentExpressionNode>();
            expr->pushToDefault(enode);
        }

    }

}

void BaseNode::visitPropertyDeclaration(BaseNode *parent, const TSNode &node){
    PropertyDeclarationNode* enode = new PropertyDeclarationNode(node);
    parent->insert(enode);
    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "identifier") == 0 ){
            enode->m_type = new IdentifierNode(child);
            enode->m_type->setParent(enode);
        } else if ( strcmp(ts_node_type(child), "property_identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
            enode->m_name->setParent(enode);
            // moveToDeclarations(parent, enode->m_name);
        } else if ( strcmp(ts_node_type(child), "expression_statement") == 0 ){
            enode->m_expression = new BindableExpressionNode(child);
            enode->m_expression->setParent(enode);
            visitChildren(enode->m_expression, child);
        } else if (strcmp(ts_node_type(child),"statement_block") == 0) {
            enode->m_statementBlock = new StatementBlockNode(child);
            enode->m_statementBlock->setParent(enode);
            visitChildren(enode->m_statementBlock, child);
        } else if (strcmp(ts_node_type(child),"component_declaration") == 0) {
            visitComponentDeclaration(enode, child);

        }
    }

    if (parent->parent() && parent->parent()->typeString() == "ComponentDeclaration")
    {
        parent->parent()->as<ComponentDeclarationNode>()->pushToProperties(enode);
    }

    if (parent->parent() && parent->parent()->typeString() == "NewComponentExpression")
    {
        parent->parent()->as<NewComponentExpressionNode>()->pushToProperties(enode);
    }
}

void BaseNode::visitBindableExpression(BaseNode *parent, const TSNode &node){
    BindableExpressionNode* enode = new BindableExpressionNode(node);
    parent->insert(enode);
    visitChildren(enode, node);
}

void BaseNode::visitMemberExpression(BaseNode *parent, const TSNode &node){
    MemberExpressionNode* enode = new MemberExpressionNode(node);
    parent->insert(enode);
    visitChildren(enode, node);

    BaseNode* p = parent;
    while (p)
    {
        if (p->typeString() == "MemberExpression") break;
        if (p->typeString() == "FunctionDeclaration") break;
        if (p->typeString() == "ClassDeclaration") break;
        if (p->typeString() == "PropertyDeclaration")
        {
            auto child = enode->children()[0];
            if (child->typeString() != "MemberExpression" && child->typeString() != "Identifier") break;
            p->as<PropertyDeclarationNode>()->pushToBindings(enode);
            break;
        }
        if (p->typeString() == "PropertyAssignment")
        {
            auto child = enode->children()[0];
            if (child->typeString() != "MemberExpression" && child->typeString() != "Identifier") break;
            p->as<PropertyAssignmentNode>()->m_bindings.push_back(enode);
            break;
        }
        p = p->parent();
    }

}

void BaseNode::visitSubscriptExpression(BaseNode *parent, const TSNode &node){
    SubscriptExpressionNode* enode = new SubscriptExpressionNode(node);
    parent->insert(enode);
    visitChildren(enode, node);
}

void BaseNode::visitPropertyAssignment(BaseNode *parent, const TSNode &node){
    PropertyAssignmentNode* enode = new PropertyAssignmentNode(node);
    parent->insert(enode);
    uint32_t count = ts_node_child_count(node);
    if (count >= 1)
    {
        TSNode lhs = ts_node_child(node, 0);
        if (strcmp(ts_node_type(lhs), "property_assignment_lhs") == 0)
        {
            for (int i = 0; i < ts_node_child_count(lhs); i+=2)
            {
                auto id = new IdentifierNode(ts_node_child(lhs, i));
                enode->m_property.push_back(id);
                id->setParent(enode);
            }
        }
    }
    if (count >= 3)
    {
        TSNode rhs = ts_node_child(node, 2);
        if ( strcmp(ts_node_type(rhs), "expression_statement") == 0 ){
            enode->m_expression = new BindableExpressionNode(rhs);
            enode->m_expression->setParent(enode);
            visitChildren(enode->m_expression, rhs);
        }
    }

    if (parent->parent() && parent->parent()->typeString() == "ComponentDeclaration")
    {
        parent->parent()->as<ComponentDeclarationNode>()->pushToAssignments(enode);
    }

    if (parent->parent() && parent->parent()->typeString() == "NewComponentExpression")
    {
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
        moveToDeclarations(componentParent, cdn->m_id);
    } else if ( componentParent->typeString() == "NewComponentExpression" ){
        NewComponentExpressionNode* ncen = componentParent->as<NewComponentExpressionNode>();
        ncen->m_id = new IdentifierNode(idChild);
        // provide id to outside scope
        moveToDeclarations(componentParent, ncen->m_id);
    }
}

void BaseNode::visitEventDeclaration(BaseNode *parent, const TSNode &node){
    EventDeclarationNode* enode = new EventDeclarationNode(node);
    parent->insert(enode);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "property_identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
        } else if ( strcmp(ts_node_type(child), "formal_type_parameters") == 0 ){
            enode->m_parameters = new ParameterListNode(child);

            uint32_t paramterCount = ts_node_named_child_count(child);

            if ( paramterCount % 2 == 0 ){
                for ( uint32_t j = 0; j < paramterCount; j += 2 ){
                    TSNode paramType = ts_node_named_child(child, j);
                    TSNode paramName = ts_node_named_child(child, j + 1);
                    enode->m_parameters->m_parameters.push_back(
                        std::make_pair(new IdentifierNode(paramType), new IdentifierNode(paramName))
                    );
                }
            }
        }
    }

    if (parent->parent() && parent->parent()->typeString() == "ComponentDeclaration")
    {
        parent->parent()->as<ComponentDeclarationNode>()->m_events.push_back(enode);
    }
}

void BaseNode::visitListenerDeclaration(BaseNode *parent, const TSNode &node){
    ListenerDeclarationNode* enode = new ListenerDeclarationNode(node);
    parent->insert(enode);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "property_identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
        } else if ( strcmp(ts_node_type(child), "formal_parameters") == 0 ){
            enode->m_parameters = new ParameterListNode(child);
            uint32_t paramterCount = ts_node_named_child_count(child);

            for ( uint32_t j = 0; j < paramterCount; ++j ){
                TSNode paramName = ts_node_named_child(child, j);
                IdentifierNode* paramNameId = new IdentifierNode(paramName);
                enode->m_parameters->m_parameters.push_back(std::make_pair(nullptr, paramNameId));
            }
        } else if ( strcmp(ts_node_type(child), "statement_block") == 0 ){
            enode->m_body = new JsBlockNode(child);
            enode->m_body->setParent(enode);
            visitChildren(enode->m_body, child);
        }
    }

    if (parent->parent() && parent->parent()->typeString() == "ComponentDeclaration")
    {
        parent->parent()->as<ComponentDeclarationNode>()->m_listeners.push_back(enode);
    }
}

void BaseNode::visitMethodDefinition(BaseNode *parent, const TSNode &node){
    MethodDefinitionNode* enode = new MethodDefinitionNode(node);
    parent->insert(enode);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "property_identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
        } else if ( strcmp(ts_node_type(child), "formal_parameters") == 0 ){
            enode->m_parameters = new ParameterListNode(child);
            uint32_t paramterCount = ts_node_named_child_count(child);

            for ( uint32_t j = 0; j < paramterCount; ++j ){
                TSNode paramName = ts_node_named_child(child, j);
                enode->m_parameters->m_parameters.push_back(
                    std::make_pair(nullptr, new IdentifierNode(paramName))
                );
            }
        } else if ( strcmp(ts_node_type(child), "statement_block") == 0 ){
            enode->m_body = new JsBlockNode(child);
            enode->m_body->setParent(enode);
            visitChildren(enode->m_body, child);
        }
    }
}

void BaseNode::visitTypedFunctionDeclaration(BaseNode *parent, const TSNode &node){
    TypedFunctionDeclarationNode* enode = new TypedFunctionDeclarationNode(node);
    parent->insert(enode);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "property_identifier") == 0 ){
            enode->m_name = new IdentifierNode(child);
        } else if ( strcmp(ts_node_type(child), "formal_type_parameters") == 0 ){
            enode->m_parameters = new ParameterListNode(child);
            uint32_t paramterCount = ts_node_child_count(child);

            for (int pc = 0; pc < paramterCount; ++pc)
            {
                TSNode ftpc = ts_node_child(child, pc);
                if (strcmp(ts_node_type(ftpc), "formal_type_parameter") == 0)
                {
                    TSNode paramType = ts_node_child(ftpc, 0);
                    TSNode paramName = ts_node_child(ftpc, 1);

                    auto typeNode = new IdentifierNode(paramType);
                    auto nameNode = new IdentifierNode(paramName);

                    enode->m_parameters->m_parameters.push_back(
                        std::make_pair(typeNode, nameNode)
                    );
                }
            }
        } else if ( strcmp(ts_node_type(child), "statement_block") == 0 ){
            enode->m_body = new JsBlockNode(child);
            enode->m_body->setParent(enode);
            visitChildren(enode->m_body, child);
        }
    }

    if (enode->m_body)
    for (unsigned i = 0; i != enode->m_parameters->m_parameters.size(); ++i)
    {
        auto pair = enode->m_parameters->m_parameters[i];

        moveToDeclarations(enode->m_body, pair.second);
    }
}

void BaseNode::visitPublicFieldDeclaration(BaseNode *parent, const TSNode &node){
    PublicFieldDeclarationNode* enode = new PublicFieldDeclarationNode(node);
    parent->insert(enode);
    visitChildren(enode, node);
}

void BaseNode::visitJsScope(BaseNode *parent, const TSNode &node){
    JsBlockNode* enode = new JsBlockNode(node);
    parent->insert(enode);
    visitChildren(enode, node);
}

void BaseNode::visitNumber(BaseNode *parent, const TSNode &node)
{
    NumberNode* nnode = new NumberNode(node);
    parent->insert(nnode);
}

void BaseNode::visitConstructorDefinition(BaseNode *parent, const TSNode &node)
{
    ConstructorDefinitionNode* cdnode = new ConstructorDefinitionNode(node);
    parent->insert(cdnode);
    // visitChildren(cdnode, node);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "statement_block") == 0 ){
            cdnode->m_block = new StatementBlockNode(child);
            cdnode->m_block->setParent(cdnode);
            visitChildren(cdnode->m_block, child);
            // enode->m_name = new IdentifierNode(child);
        } else if ( strcmp(ts_node_type(child), "formal_parameters") == 0 ){
            cdnode->m_formalParameters = new FormalParametersNode(child);
            cdnode->m_formalParameters->setParent(cdnode);
            visitChildren(cdnode->m_formalParameters,child);
        }
        else visit(cdnode, child);
    }

    if (parent->typeString() == "ComponentBody")
    {
        parent->as<ComponentBodyNode>()->setConstructor(cdnode);
    }
}

void BaseNode::visitExpressionStatement(BaseNode *parent, const TSNode &node)
{
    ExpressionStatementNode* esnode = new ExpressionStatementNode(node);
    parent->insert(esnode);
    visitChildren(esnode, node);
}

void BaseNode::visitCallExpression(BaseNode *parent, const TSNode &node)
{
    CallExpressionNode* enode = new CallExpressionNode(node);
    parent->insert(enode);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "arguments") == 0 ){
            enode->m_arguments = new ArgumentsNode(child);
            enode->insert(enode->m_arguments);
            visitChildren(enode->m_arguments, child);
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
    parent->insert(tagnode);
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
        else if (body->parent()->typeString() == "NewComponentExpression")
        {
            auto expr = body->parent()->as<NewComponentExpressionNode>();
            expr->pushToDefault(tagnode);
        }

    }
}

void BaseNode::visitTaggedString(BaseNode *parent, const TSNode &node)
{
    TaggedStringNode* tsnode = new TaggedStringNode(node);
    parent->insert(tsnode);
}

void BaseNode::visitFunctionDeclaration(BaseNode *parent, const TSNode &node)
{
    FunctionDeclarationNode* fnode = new FunctionDeclarationNode(node);
    parent->insert(fnode);
    visitChildren(fnode, node);

    for (auto it = fnode->children().begin(); it != fnode->children().end(); ++it)
        if ((*it)->typeString() == "Identifier")
        {
            // this must be the id!
            IdentifierNode* id = static_cast<IdentifierNode*>(*it);
            moveToDeclarations(parent, id);
        }
}

void BaseNode::visitClassDeclaration(BaseNode *parent, const TSNode &node)
{
    ClassDeclarationNode* fnode = new ClassDeclarationNode(node);
    parent->insert(fnode);
    visitChildren(fnode, node);

    for (auto it = fnode->children().begin(); it != fnode->children().end(); ++it)
        if ((*it)->typeString() == "Identifier")
        {
            // this must be the id!
            IdentifierNode* id = static_cast<IdentifierNode*>(*it);
            moveToDeclarations(parent, id);
        }
}

void BaseNode::visitVariableDeclaration(BaseNode *parent, const TSNode &node)
{
    VariableDeclarationNode* vdn = new VariableDeclarationNode(node);
    parent->insert(vdn);

    uint32_t count = ts_node_child_count(node);
    for ( uint32_t i = 0; i < count; ++i ){
        TSNode child = ts_node_child(node, i);
        if ( strcmp(ts_node_type(child), "variable_declarator") == 0 ){
            VariableDeclaratorNode* decl = new VariableDeclaratorNode(child);
            decl->setParent(vdn);
            vdn->m_declarators.push_back(decl);



            uint32_t subcount = ts_node_child_count(child);

            for ( uint32_t k = 0; k < subcount; ++k ){
                TSNode smaller_child = ts_node_child(child, k);

                if ( strcmp(ts_node_type(smaller_child), "identifier") == 0 ){
                    IdentifierNode* id = new IdentifierNode(smaller_child);
                    decl->insert(id);
                    moveToDeclarations(decl, id);
                }
            }
        } else {
            visit(vdn, child);
        }
    }

}

void BaseNode::visitNewExpression(BaseNode *parent, const TSNode &node)
{
    NewExpressionNode* nenode = new NewExpressionNode(node);
    parent->insert(nenode);
    visitChildren(nenode, node);
}

void BaseNode::visitReturnStatement(BaseNode *parent, const TSNode &node)
{
    ReturnStatementNode* rsnode = new ReturnStatementNode(node);
    parent->insert(rsnode);
    visitChildren(rsnode, node);
}

std::string ImportNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    std::string importPath;
    if ( m_importPath ){
        for( IdentifierNode* node : m_importPath->segments() ){
            importPath += node->rangeString();
        }
    }

    std::string importAs = m_importAs ? "(as " + m_importAs->rangeString() + ")" : "";

    result += std::string("Import (" + importPath + ")") + importAs + rangeString() + "\n";

    return result;
}

void ImportNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &fragments, int /*indentValue*/){
    if ( !m_importPath )
        return;

    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = startByte();
    compose->to   = endByte();

    std::string importSegments;

    if (m_isRelative) importSegments = ".";
    auto segments = m_importPath->segments();
    for (int i=0; i<segments.size(); ++i){
        auto node = segments[i];
        if (i!=0) importSegments += ".";
        importSegments += slice(source, node);
    }

    if ( m_importAs ){
        *compose << ("var " + slice(source, m_importAs) + " = imports.requireAs(\'" + importSegments + "\')\n");
    } else {
        *compose << ("imports.require(\'" + importSegments + "\')\n");
    }

    fragments.push_back(compose);
}

void ImportNode::insert(BaseNode *child)
{
    if (child->typeString() == "ImportPath")
    {
        m_importPath = child->as<ImportPathNode>();
        child->setParent(this);
        return;
    }
    BaseNode::insert(child);
}

void ProgramNode::insert(BaseNode *child){
    if ( child->typeString() == "Import" ){
        m_imports.push_back(child->as<ImportNode>());
        child->setParent(this);
    } else if ( child->typeString() == "ComponentDeclaration" ){
        m_exports.push_back(child->as<ComponentDeclarationNode>());
        child->setParent(this);
    } else
        BaseNode::insert(child);
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

void ProgramNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &fragments, int indentValue){

    for ( BaseNode* child: m_imports )
        child->convertToJs(source, fragments, indentValue);

    size_t offset = fragments.size();

    for ( BaseNode* child: m_exports )
        child->convertToJs(source, fragments, indentValue);

    BaseNode::convertToJs(source, fragments, indentValue);

    if (m_undeclared.empty() && m_idComponents.empty()) return;
    
    auto iter = fragments.begin() + offset;
    ElementsInsertion* compose = new ElementsInsertion;
    if (offset == fragments.size()) {
        compose->from = compose->to = static_cast<int>(source.length());
    } else {
        compose->from = compose->to = (*iter)->from;
    }
    for (auto it = m_undeclared.begin(); it != m_undeclared.end(); ++it)
        *compose << ("var " + *it + " = imports.get('" + *it + "')\n");

    if (!m_idComponents.empty())
    {
        for (int i = 0; i < m_idComponents.size();++i)
        {
            *compose << "var " << slice(source, m_idComponents[i]->id()) << " = new " << slice(source, m_idComponents[i]->name());
            checkIdentifierDeclared(source, m_idComponents[i]->name(), slice(source, m_idComponents[i]->name()));
            if (m_idComponents[i]->arguments()) *compose << slice(source, m_idComponents[i]->arguments()) << "\n";
            else *compose << "()\n";
        }

        for (int i = 0; i < m_idComponents.size();++i)
        {
            std::string id = slice(source, m_idComponents[i]->id());
            auto properties = m_idComponents[i]->properties();

            for (int idx = 0; idx<properties.size(); ++idx)
            {
                *compose << "Element.addProperty(" << id << ", '" << slice(source, properties[idx]->name())
                         << "', { type: '" << slice(source, properties[idx]->type()) << "', notify: '"
                         << slice(source, properties[idx]->name()) << "Changed' })\n";
            }

        }
    }

    fragments.insert(iter, compose);

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

void ComponentDeclarationNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &fragments, int indentValue){
    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = startByte();
    compose->to   = endByte();

    std::string name = slice(source, m_name);

    if (name == "default")
    {
        BaseNode* p = this;
        while (p && p->typeString() != "Program") p = p->parent();
        if (p) {
            ProgramNode* program = dynamic_cast<ProgramNode*>(p);
            if (program) name = program->filename();
        }
    }

    *compose << indent(indentValue);

    if (parent() && parent()->typeString() == "Program")
    {
        *compose << "\nmodule.exports[\"" << name << "\"] = ";
    }

    std::string heritage = "";
    if ( m_heritage.size() > 0 ){
        heritage = slice(source, m_heritage[0]);
        checkIdentifierDeclared(source, this, heritage);
        for ( size_t i = 1; i < m_heritage.size(); ++i ){
            heritage += "." + slice(source, m_heritage[i]);
        }
    }

    *compose << ("class " + name + " extends " + (heritage.empty() ? "Element" : heritage) + "{\n\n");

    // handle constructor
    if (m_body->constructor()){
        *compose << indent(indentValue + 1) <<  "constructor";
        if (m_body->constructor()->m_formalParameters)
        {
            *compose << slice(source, m_body->constructor()->m_formalParameters);
        } else *compose << "()";
        *compose << "{\n";
        *compose << indent(indentValue + 2);
        std::string block;
        if (!m_body->m_constructor->m_superCall)
        {
            *compose << "super()\n";
            block = slice(source, m_body->constructor()->m_block);
            block = block.substr(1, block.length() - 2);
        } else {
            *compose << slice(source, m_body->m_constructor->m_superCall);
            block = slice(source, m_body->m_constructor->m_superCall->endByte(), m_body->constructor()->m_block->endByte() - 1);
        }

        *compose << "\n" << indent(indentValue + 2) << "this.__initialize()\n";
        *compose << block;


        *compose << "\n" << indent(indentValue + 1) << "}\n\n";

    } else {
        *compose << indent(indentValue + 1) << "constructor(){\n"
                 << indent(indentValue + 2) << "super()\n"
                 << indent(indentValue + 2) << "this.__initialize()\n"
                 << indent(indentValue + 1) << "}\n\n";
    }

    *compose << indent(indentValue + 1) << "__initialize(){\n";

    if (m_id || !m_idComponents.empty())
        *compose << indent(indentValue + 1) << "this.ids = {}\n\n";

    if (m_id){
        *compose << indent(indentValue + 1) << "var " << slice(source, m_id) << " = this\n";
        *compose << indent(indentValue + 1) << "this.ids[\"" << slice(source, m_id) << "\"] = " << slice(source, m_id) << "\n\n";
    }

    for (int i = 0; i < m_idComponents.size();++i)
    {
        *compose << indent(indentValue + 1) << "var " << slice(source, m_idComponents[i]->id()) << " = new " << slice(source, m_idComponents[i]->name());
        if (m_idComponents[i]->arguments()) *compose << slice(source, m_idComponents[i]->arguments()) << "\n";
        else *compose << "()\n";
        *compose << indent(indentValue + 1) << "this.ids[\"" << slice(source, m_idComponents[i]->id()) << "\"] = " << slice(source, m_idComponents[i]->id()) << "\n\n";
    }

    for (int i = 0; i < m_idComponents.size();++i)
    {
        std::string id = slice(source, m_idComponents[i]->id());
        auto properties = m_idComponents[i]->properties();

        for (int idx = 0; idx<properties.size(); ++idx)
        {
            *compose << indent(indentValue + 1) << "Element.addProperty(" << id << ", '" << slice(source, properties[idx]->name())
                     << "', { type: \"" << slice(source, properties[idx]->type()) << "\", notify: \""
                     << slice(source, properties[idx]->name()) << "Changed\" })\n";
        }
    }

    for (int i=0; i < m_properties.size(); ++i)
    {
        *compose << indent(indentValue + 1) << "Element.addProperty(this, '" << slice(source, m_properties[i]->name())
                 << "', { type: \"" << slice(source, m_properties[i]->type()) << "\", notify: \""
                 << slice(source, m_properties[i]->name()) << "Changed\" })\n";
    }

    for (int i = 0; i < m_events.size(); ++i)
    {
        std::string paramList = "";
        if ( m_events[i]->parameterList() ){
            ParameterListNode* pdn = m_events[i]->parameterList()->as<ParameterListNode>();
            for ( auto it = pdn->parameters().begin(); it != pdn->parameters().end(); ++it ){
                if ( it != pdn->parameters().begin() )
                    paramList += ",";
                paramList += "[" + slice(source, it->first) + "," + slice(source, it->second) + "]";
            }
        }

        *compose << indent(indentValue + 1) << ("Element.addEvent(this, \'" + slice(source, m_events[i]->name()) + "\', [" + paramList + "])\n");
    }

    for (int i = 0; i < m_listeners.size(); ++i)
    {
        std::string paramList = "";
        if ( m_listeners[i]->parameterList() ){
            ParameterListNode* pdn = m_listeners[i]->parameterList()->as<ParameterListNode>();
            for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                if ( pit != pdn->parameters().begin() )
                    paramList += ",";
                paramList += slice(source, pit->second);
            }
        }

        *compose << "this.on(\'" << slice(source, m_listeners[i]->name()) << "\', function(" << paramList << ")";
        JSSection* jssection = new JSSection;
        jssection->from = m_listeners[i]->body()->startByte();
        jssection->to   = m_listeners[i]->body()->endByte();
        m_listeners[i]->convertToJs(source, jssection->m_children);
        std::vector<std::string> flat;
        jssection->flatten(source, flat);
        for (auto s: flat) *compose << s;
        *compose << ".bind(this));\n";

    }

    for (int i=0; i<m_properties.size(); ++i)
    {
        if (m_properties[i]->hasBindings())
        {
            *compose << indent(indentValue + 1) << "Element.assignPropertyExpression(this,\n '"
                     << slice(source, m_properties[i]->name())
                     << "',\n function(){ return " << slice(source, m_properties[i]->expression()) << "}.bind(this),\n"
                     << "[\n";
            std::set<std::pair<std::string, std::string>> bindingPairs;
            for (auto idx = m_properties[i]->bindings().begin(); idx != m_properties[i]->bindings().end(); ++idx)
            {
                BaseNode* node = *idx;
                if (node->typeString() == "MemberExpression")
                {
                    MemberExpressionNode* men = node->as<MemberExpressionNode>();

                    std::pair<std::string, std::string> pair = std::make_pair(
                        slice(source, men->children()[0]),
                        slice(source, men->children()[1])
                    );

                    if (bindingPairs.find(pair) == bindingPairs.end())
                    {
                        if (!bindingPairs.empty()) *compose << ",\n";
                        *compose << "[ " << pair.first << ", '" <<  pair.second << "Changed' ]";
                        bindingPairs.insert(pair);
                    }

                    BaseNode* left = men->children()[0];
                    while (left->typeString() != "Identifier")
                        left = left->children()[0];

                    checkIdentifierDeclared(source, left, slice(source, left));

                }
            }
            *compose << "]\n)\n";
        }
        else
        {
            *compose << "this." << slice(source,m_properties[i]->name())
                     << " = ";
            if (m_properties[i]->expression() && m_properties[i]->expression()->children().size() == 1
                    && m_properties[i]->expression()->children()[0]->typeString() == "NewComponentExpression")
            {
                auto nce = m_properties[i]->expression()->children()[0]->as<NewComponentExpressionNode>();

                el::JSSection* section = new el::JSSection;
                section->from = nce->startByte();
                section->to = nce->endByte();
                nce->convertToJs(source, section->m_children);
                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat) *compose << s;
                delete section;
            }
            else if (m_properties[i]->expression()){
                *compose << slice(source, m_properties[i]->expression()) << "\n";
            }
            else if (m_properties[i]->componentDeclaration())
            {
                auto cd = m_properties[i]->componentDeclaration();
                el::JSSection* section = new el::JSSection;
                section->from = cd->startByte();
                section->to = cd->endByte();
                cd->convertToJs(source, section->m_children);
                std::vector<std::string> flat;
                section->flatten(source, flat);
                for (auto s: flat) *compose << s;
                delete section;
            }
            else if (m_properties[i]->statementBlock())
            {
                *compose << "(function()" << slice(source, m_properties[i]->statementBlock())<< "())\n\n";

                std::queue<BaseNode*> q;
                q.push(m_properties[i]->statementBlock());
                while (!q.empty())
                {
                    BaseNode* b = q.front(); q.pop();
                    if (b->typeString() == "NewExpression" /*|| other stuff*/)
                    {
                        for (auto child: b->children())
                        {
                            if (child->typeString() == "Identifier")
                                checkIdentifierDeclared(source, child, slice(source, child));
                        }
                    } else {
                        for (auto child: b->children())
                        {
                            q.push(child);
                        }
                    }
                }
            }

            *compose << "\n\n";
        }
    }

    if (!m_default.empty())
    {
        *compose << "Element.assignDefaultProperty(this, [\n";
        for (int i = 0; i < m_default.size(); ++i)
        {
            if (i != 0) *compose << ",\n";
            el::JSSection* section = new el::JSSection;
            section->from = m_default[i]->startByte();
            section->to = m_default[i]->endByte();
            m_default[i]->convertToJs(source, section->m_children);
            std::vector<std::string> flat;
            section->flatten(source, flat);
            for (auto s: flat) *compose << s;
            delete section;
        }
        *compose << "\n]\n)\n";
    }/* else {
        *compose << "Element.assignDefaultProperty(null)\n\n";
    }*/

    *compose << "}\n\n";

    // *compose << m_body->convertToJs(source);
    for ( auto it = m_body->children().begin(); it != m_body->children().end(); ++it ){

        BaseNode* c = *it;

        if ( c->typeString() == "MethodDefinition"){
            MethodDefinitionNode* mdn = c->as<MethodDefinitionNode>();

            JSSection* jssection = new JSSection;
            jssection->from = mdn->startByte();
            jssection->to   = mdn->endByte();
            *compose << jssection << "\n";
            mdn->body()->convertToJs(source, jssection->m_children);
        } else if ( c->typeString() == "TypedFunctionDeclaration" ){
            TypedFunctionDeclarationNode* tfdn = c->as<TypedFunctionDeclarationNode>();

            std::string paramList = "";
            if ( tfdn->parameters() ){
                ParameterListNode* pdn = tfdn->parameters()->as<ParameterListNode>();
                for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                    if ( pit != pdn->parameters().begin() )
                        paramList += ",";
                    paramList += slice(source, pit->second);
                }
            }
            *compose << slice(source, tfdn->name()) << "(" << paramList << ")\n";

            JSSection* jssection = new JSSection;
            jssection->from = tfdn->body()->startByte();
            jssection->to   = tfdn->body()->endByte();
            tfdn->body()->convertToJs(source, jssection->m_children);
            std::vector<std::string> flat;
            jssection->flatten(source, flat);
            for (auto s: flat)
            {
                *compose << s << "\n";
            }
            delete jssection;

            *compose << "\n\n";
        }
    }

    *compose << "}\n";
    fragments.push_back(compose);
}


void NewComponentExpressionNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &fragments, int indentValue){

    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = startByte();
    compose->to   = endByte();

    std::string name = slice(source, m_name);

    if (m_instance)
    {
        std::string instance_name = slice(source, m_instance);

        if (instance_name == "default")
        {
            BaseNode* p = this;
            while (p && p->typeString() != "Program") p = p->parent();
            if (p) {
                ProgramNode* program = dynamic_cast<ProgramNode*>(p);
                if (program) instance_name = program->filename();
            }
        }

        *compose << "module.exports[\"" << instance_name << "\"] = ";
    }
    *compose << ("(function(parent){\n    this.setParent(parent)\n");

    if (!m_id)
    for (int i = 0; i<m_properties.size(); ++i)
    {
        *compose << "Element.addProperty(this, '" << slice(source, m_properties[i]->name())
                 << "', { type: '" << slice(source, m_properties[i]->type()) << "', notify: '"
                 << slice(source, m_properties[i]->name()) << "Changed' })\n";
    }
    else {
        *compose << "Element.assignId(" << slice(source,m_id) << ", \"" << slice(source,m_id) << "\")\n";
    }

    for (int i=0; i<m_properties.size(); ++i)
    {
        if (m_properties[i]->hasBindings())
        {
            if (m_properties[i]->expression())
            {
                *compose << "Element.assignPropertyExpression(this,\n '"
                         << slice(source, m_properties[i]->name())
                         << "',\n function(){ return " << slice(source, m_properties[i]->expression()) << "}.bind(this),\n"
                         << "[\n";
                std::set<std::pair<std::string, std::string>> bindingPairs;
                for (auto idx = m_properties[i]->bindings().begin(); idx != m_properties[i]->bindings().end(); ++idx)
                {
                    BaseNode* node = *idx;
                    if (node->typeString() == "MemberExpression")
                    {
                        MemberExpressionNode* men = node->as<MemberExpressionNode>();

                        std::pair<std::string, std::string> pair = std::make_pair(
                            slice(source, men->children()[0]),
                            slice(source, men->children()[1])
                        );

                        if (bindingPairs.find(pair) == bindingPairs.end())
                        {
                            if (!bindingPairs.empty()) *compose << ",\n";
                            *compose << "[ " << pair.first << ", '" <<  pair.second << "Changed' ]";
                            bindingPairs.insert(pair);
                        }

                    }
                }
                *compose << "]\n)\n";
            } else {
                *compose << "Element.assignPropertyExpression(this,\n '"
                         << slice(source, m_properties[i]->name())
                         << "',\n function()" << slice(source, m_properties[i]->statementBlock()) << ".bind(this),\n"
                         << "[\n";
                std::set<std::pair<std::string, std::string>> bindingPairs;
                for (auto idx = m_properties[i]->bindings().begin(); idx != m_properties[i]->bindings().end(); ++idx)
                {
                    BaseNode* node = *idx;
                    if (node->typeString() == "MemberExpression")
                    {
                        MemberExpressionNode* men = node->as<MemberExpressionNode>();

                        std::pair<std::string, std::string> pair = std::make_pair(
                            slice(source, men->children()[0]),
                            slice(source, men->children()[1])
                        );

                        if (bindingPairs.find(pair) == bindingPairs.end())
                        {
                            if (!bindingPairs.empty()) *compose << ",\n";
                            *compose << "[ " << pair.first << ", '" <<  pair.second << "Changed' ]";
                            bindingPairs.insert(pair);
                        }

                    }
                }
                *compose << "]\n)\n";
            }

        }
        else
        {
            if (m_properties[i]->expression())
            {
                *compose << "this." << slice(source,m_properties[i]->name())
                         << " = " << slice(source, m_properties[i]->expression()) << "\n";
            }
            else if (m_properties[i]->statementBlock()) {
                *compose << "this." << slice(source,m_properties[i]->name()) << " = "
                         << "(function()" <<  slice(source, m_properties[i]->statementBlock()) << "())\n\n";
            }
        }
    }

    for (unsigned i=0; i<m_assignments.size(); ++i){

        if (m_assignments[i]->m_bindings.size() > 0)
        {
            if (m_assignments[i]->m_expression)
            {
                *compose << "Element.assignPropertyExpression(this,\n '"
                         << slice(source, m_properties[i]->name())
                         << "',\n function(){ return " << slice(source, m_assignments[i]->m_expression) << "}.bind(this),\n"
                         << "[\n";
                std::set<std::pair<std::string, std::string>> bindingPairs;
                for (auto idx = m_assignments[i]->m_bindings.begin(); idx != m_assignments[i]->m_bindings.end(); ++idx)
                {
                    BaseNode* node = *idx;
                    if (node->typeString() == "MemberExpression")
                    {
                        MemberExpressionNode* men = node->as<MemberExpressionNode>();

                        std::pair<std::string, std::string> pair = std::make_pair(
                            slice(source, men->children()[0]),
                            slice(source, men->children()[1])
                        );

                        if (bindingPairs.find(pair) == bindingPairs.end())
                        {
                            if (!bindingPairs.empty()) *compose << ",\n";
                            *compose << "[ " << pair.first << ", '" <<  pair.second << "Changed' ]";
                            bindingPairs.insert(pair);
                        }

                    }
                }
                *compose << "]\n)\n";
            }
        }
        else {
            if (m_assignments[i]->m_expression && !m_assignments[i]->m_property.empty())
            {
                *compose << "this";

                for (int prop=0; prop<m_assignments[i]->m_property.size(); ++prop)
                {
                    *compose << "." << slice(source, m_assignments[i]->m_property[prop]);
                }

                *compose << " = " << slice(source, m_assignments[i]->m_expression) << "\n\n";
            }
        }

    }

    if (!m_default.empty())
    {
        *compose << "Element.assignDefaultProperty(this, [\n";
        for (unsigned i = 0; i < m_default.size(); ++i)
        {
            if (i != 0) *compose << ",\n";
            el::JSSection* section = new el::JSSection;
            section->from = m_default[i]->startByte();
            section->to = m_default[i]->endByte();
            m_default[i]->convertToJs(source, section->m_children);
            std::vector<std::string> flat;
            section->flatten(source, flat);
            for (auto s: flat) *compose << s << "\n";
            delete section;
        }
        *compose << "\n]\n)\n";
    }/* else {
        *compose << "Element.assignDefaultProperty(null)\n\n";
    }*/


    for ( auto it = m_body->children().begin(); it != m_body->children().end(); ++it ){
        BaseNode* c = *it;

        if ( c->typeString() == "EventDeclaration"){
            EventDeclarationNode* edn = c->as<EventDeclarationNode>();

            std::string paramList = "";
            if ( edn->parameterList() ){
                ParameterListNode* pdn = edn->parameterList()->as<ParameterListNode>();
                for ( auto it = pdn->parameters().begin(); it != pdn->parameters().end(); ++it ){
                    if ( it != pdn->parameters().begin() )
                        paramList += ",";
                    paramList += "[" + slice(source, it->first) + "," + slice(source, it->second) + "]";
                }
            }

            *compose << ("Element.addEvent(__current, \'" + slice(source, edn->name()) + "\', [" + paramList + "])\n");
        } else if ( c->typeString() == "ListenerDeclaration" ){
            ListenerDeclarationNode* ldn = c->as<ListenerDeclarationNode>();

            std::string paramList = "";
            if ( ldn->parameterList() ){
                ParameterListNode* pdn = ldn->parameterList()->as<ParameterListNode>();
                for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                    if ( pit != pdn->parameters().begin() )
                        paramList += ",";
                    paramList += slice(source, pit->second);
                }
            }
            JSSection* jssection = new JSSection;
            jssection->from = ldn->body()->startByte();
            jssection->to   = ldn->body()->endByte();
            *compose << "__current.on(\'" << slice(source, ldn->name()) << "\', function(" << paramList << ")" << jssection << "\n";

            ldn->body()->convertToJs(source, jssection->m_children);
        } else if ( c->typeString() == "MethodDefinition"){
            MethodDefinitionNode* mdn = c->as<MethodDefinitionNode>();

            JSSection* jssection = new JSSection;
            jssection->from = mdn->startByte();
            jssection->to   = mdn->endByte();
            *compose << jssection;
            mdn->body()->convertToJs(source, jssection->m_children);
        } else if ( c->typeString() == "TypedFunctionDeclaration" ){
            TypedFunctionDeclarationNode* tfdn = c->as<TypedFunctionDeclarationNode>();

            std::string paramList = "";
            if ( tfdn->parameters() ){
                ParameterListNode* pdn = tfdn->parameters()->as<ParameterListNode>();
                for ( auto pit = pdn->parameters().begin(); pit != pdn->parameters().end(); ++pit ){
                    if ( pit != pdn->parameters().begin() )
                        paramList += ",";
                    paramList += slice(source, pit->second);
                }
            }
            JSSection* jssection = new JSSection;
            jssection->from = tfdn->body()->startByte();
            jssection->to   = tfdn->body()->endByte();
            *compose << "__current." << slice(source, tfdn->name()) << " = function(" << paramList << ")" << jssection << "\n";

            tfdn->body()->convertToJs(source, jssection->m_children);
        }
    }

    *compose << "return this\n}.bind(";

    if (!m_id)
    {
        *compose << "new " << slice(source, m_name);
        checkIdentifierDeclared(source, m_name, slice(source, m_name));
        if (!m_arguments) *compose << "()";
        else *compose << slice(source, m_arguments);
    } else {
        *compose << slice(source, m_id);
    }

    bool isThis = parent() && parent()->typeString() == "ComponentBody";
    isThis = isThis || (parent()
                        && parent()->parent()
                        && parent()->parent()->typeString() == "PropertyDeclaration"
                        && parent()->parent()->parent()
                        && parent()->parent()->parent()->typeString() == "ComponentBody" );

    if (isThis)
    {
        *compose << ")(this)";
    } else {
        *compose << ")(null)";
    }

    *compose << ")";

    fragments.push_back(compose);
}


NewComponentExpressionNode::NewComponentExpressionNode(const TSNode &node)
    : JsBlockNode(node, "NewComponentExpression")
    , m_name(nullptr)
    , m_id(nullptr)
    , m_body(nullptr)
    , m_arguments(nullptr)
    , m_instance(nullptr)
{

}

std::string NewComponentExpressionNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    std::string name = "(name " + (m_name ? m_name->rangeString() : "") + ")";

    std::string identifier = "";
    if ( m_id )
        identifier = "(id " + m_id->rangeString() + ")";


    std::string args = "";
    if (m_arguments)
        args = "(args " + m_arguments->rangeString() + ")";

    result += "NewComponentExpression " + name + identifier +args + rangeString() + "\n";
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
    , m_expression(nullptr)
    , m_statementBlock(nullptr)
    , m_componentDeclaration(nullptr)
{
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

PropertyAssignmentNode::PropertyAssignmentNode(const TSNode &node)
    : JsBlockNode(node, "PropertyAssignment")
    , m_expression(nullptr)
{
}

std::string PropertyAssignmentNode::toString(int indent) const{
    std::string result;
   if ( indent > 0 )
       result.assign(indent * 2, ' ');
    std::string name = "";
//    if ( m_property )
//        name = "(property " + m_property->rangeString() + ")";
    result += "PropertyAssignment " + rangeString() + name + "\n";
    if ( m_expression )
        result += m_expression->toString(indent >= 0 ? indent + 1 : indent);
    return result;
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

TypedFunctionDeclarationNode::TypedFunctionDeclarationNode(const TSNode &node)
    : BaseNode(node, "TypedFunctionDeclaration")
    , m_name(nullptr)
    , m_parameters(nullptr)
    , m_body(nullptr)
{
}

std::string TypedFunctionDeclarationNode::toString(int indent) const{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');
    std::string name = "";
    if ( m_name )
        name = "(name " + m_name->rangeString() + ")";

    result += "TypedFunctionDeclaration " + rangeString() + name + "\n";
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


    if (m_formalParameters)
        result += m_formalParameters->toString(indent >= 0 ? indent + 1 : indent);

    if (m_block)
        result += m_block->toString(indent >= 0 ? indent + 1 : indent);

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

void NewTaggedComponentExpressionNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &fragments, int indentValue)
{
    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = startByte();
    compose->to = endByte();
    std::string name, value;
    for (auto child: children())
    {
        if (child->typeString() == "Identifier") {
            name = slice(source, child);
            checkIdentifierDeclared(source, child, name);
        }
        else
        {
            value = slice(source, child);
            value = value.substr(1, value.length() - 2);

            std::replace(value.begin(), value.end(), '\r', ' ');
            std::replace(value.begin(), value.end(), '\n', ' ');
            std::replace(value.begin(), value.end(), '\t', ' ');

            Utf8::trimRight(value);

            std::string result = "";
            unsigned i = 0;
            while (i < value.length())
            {
                if (value[i] == ' ')
                {
                    while (value[i] == ' ') ++i;
                    result += " ";
                } else if (value[i] == '\\') {
                    if (i == value.length() -1) break;
                    ++i;
                    if (value[i] == 'n') result += "\\n";
                    if (value[i] == 's') result += ' ';
                    if (value[i] == '\\') result += "\\\\";
                    ++i;
                } else {
                    result += value[i];
                    ++i;
                }

            }

            value = result;
        }
    }



    *compose << indent(indentValue + 1) << "(function(parent){\n";
    *compose << indent(indentValue + 2) << "this.setParent(parent)\n";
    *compose << indent(indentValue + 2) << "return this\n";
    *compose << indent(indentValue + 1) << "}.bind(new " << name << "(\"" << value << "\"))(this))\n";

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

void ArgumentsNode::convertToJs(const std::string &source, std::vector<ElementsInsertion *> &fragments, int indent)
{
    ElementsInsertion* compose = new ElementsInsertion;
    compose->from = startByte();
    compose->to = endByte();

    for (auto child: children())
    {
        if (child->typeString() == "Identifier")
        {
            checkIdentifierDeclared(source, this, slice(source, child));
        }
    }

    *compose << slice(source, this);

    fragments.push_back(compose);
}

std::string VariableDeclarationNode::toString(int indent) const
{
    std::string result;
    if ( indent > 0 )
        result.assign(indent * 2, ' ');

    result += "VariableDeclaration " + rangeString() + "\n";

    for (int i = 0; i < m_declarators.size(); i++)
    {
        result += m_declarators[i]->toString(indent >= 0 ? indent + 1 : indent);
    }

    for (auto it = children().begin(); it != children().end(); ++it)
        result += (*it)->toString(indent >= 0 ? indent + 1 : indent);

    return result;
}

}} // namespace lv, el

