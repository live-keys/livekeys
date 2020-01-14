#ifndef LVELEMENTSNODES_H
#define LVELEMENTSNODES_H

#include <vector>
#include <set>

#include "live/utf8.h"

#include "tree_sitter/api.h"
#include "tree_sitter/parser.h"
#include "elementssections_p.h"
#include "languageparser.h"

namespace lv{ namespace el{

class BaseNode;
class IdentifierNode;
class JsBlockNode;
class ImportPathNode;
class ImportNode;
class PropertyDeclarationNode;
class PropertyAssignmentNode;
class ParameterListNode;
class EventDeclarationNode;
class ListenerDeclarationNode;
class TypedFunctionDeclarationNode;
class MethodDefinitionNode;
class PublicFieldDeclarationNode;
class ComponentDeclarationNode;
class NewComponentExpressionNode;
class BindableExpressionNode;
class MemberExpressionNode;
class SubscriptExpressionNode;
class StatementBlockNode;
class ArgumentsNode;

class BaseNode{
    friend class JsBlockNode;
public:
    BaseNode(const TSNode& node, const std::string& typeString = "Elements");
    virtual ~BaseNode();

    const TSNode& current() const{ return m_node; }
    std::string astString() const;
    virtual std::string toString(int indent = 0) const;

    static BaseNode* visit(LanguageParser::AST* ast);
    static void checkIdentifierDeclared(const std::string& source, BaseNode* node, std::string id);
    const std::string& typeString() const{ return m_typeString; }

    template <typename T> T* as(){ return static_cast<T*>(this); }
    const std::vector<BaseNode *> &children() const;

    int startByte() const;
    int endByte() const;
    std::string rangeString() const;

    virtual void convertToJs(const std::string& source, std::vector<ElementsInsertion*>& sections, int indent = 0);

    static std::string slice(const std::string& source, uint32_t start, uint32_t end);
    static std::string slice(const std::string& source, BaseNode* node);

    static void moveToDeclarations(BaseNode* parent, IdentifierNode* idNode);

    BaseNode* parent() const{ return m_parent; }
    void setParent(BaseNode* parent){ m_parent = parent; }

protected:
    virtual void insert(BaseNode *child);

private:
    static void visit(BaseNode* parent, const TSNode& node);
    static void visitChildren(BaseNode* parent, const TSNode& node);
    static void visitImport(BaseNode* parent, const TSNode& node);
    static void visitIdentifier(BaseNode* parent, const TSNode& node);
    static void visitImportPath(BaseNode* parent, const TSNode& node);
    static void visitComponentDeclaration(BaseNode* parent, const TSNode& node);
    static void visitComponentBody(BaseNode* parent, const TSNode& node);
    static void visitNewComponentExpression(BaseNode* parent, const TSNode& node);
    static void visitPropertyDeclaration(BaseNode* parent, const TSNode& node);
    static void visitBindableExpression(BaseNode* parent, const TSNode& node);
    static void visitMemberExpression(BaseNode* parent, const TSNode& node);
    static void visitSubscriptExpression(BaseNode* parent, const TSNode& node);
    static void visitPropertyAssignment(BaseNode* parent, const TSNode& node);
    static void visitIdentifierAssignment(BaseNode* parent, const TSNode& node);
    static void visitPublicFieldDeclaration(BaseNode* parent, const TSNode& node);
    static void visitEventDeclaration(BaseNode* parent, const TSNode& node);
    static void visitListenerDeclaration(BaseNode* parent, const TSNode& node);
    static void visitMethodDefinition(BaseNode* parent, const TSNode& node);
    static void visitTypedFunctionDeclaration(BaseNode* parent, const TSNode& node);
    static void visitJsScope(BaseNode* parent, const TSNode& node);
    static void visitNumber(BaseNode* parent, const TSNode& node);
    static void visitConstructorDefinition(BaseNode* parent, const TSNode& node);
    static void visitExpressionStatement(BaseNode* parent, const TSNode& node);
    static void visitCallExpression(BaseNode* parent, const TSNode& node);
    static void visitNewTaggedComponentExpression(BaseNode* parent, const TSNode& node);
    static void visitTaggedString(BaseNode* parent, const TSNode& node);
    static void visitFunctionDeclaration(BaseNode* parent, const TSNode& node);
    static void visitClassDeclaration(BaseNode* parent, const TSNode& node);
    static void visitVariableDeclaration(BaseNode* parent, const TSNode& node);
    static void visitNewExpression(BaseNode* parent, const TSNode& node);

    BaseNode*              m_parent;
    TSNode                 m_node;
    std::string            m_typeString;
    std::vector<BaseNode*> m_children;
};

class NumberNode: public BaseNode {
    friend class BaseNode;
public:
    NumberNode(const TSNode& node, const std::string& typeString = "Number") : BaseNode(node, typeString){}
};

class FunctionDeclarationNode: public BaseNode {
    friend class BaseNode;
public:
    FunctionDeclarationNode(const TSNode& node, const std::string& typeString = "FunctionDeclaration") : BaseNode(node, typeString){}
};

class NewExpressionNode: public BaseNode {
    friend class BaseNode;
public:
    NewExpressionNode(const TSNode& node) : BaseNode(node, "NewExpression"){}
};

class JsBlockNode : public BaseNode{
    friend class BaseNode;
public:
    JsBlockNode(const TSNode& node, const std::string& typeString = "JSScope") : BaseNode(node, typeString){}
    const std::vector<IdentifierNode*>& identifiers() const { return m_declarations; }
private:
    std::vector<IdentifierNode*> m_declarations;
};

class ProgramNode : public JsBlockNode {
    friend class BaseNode;
public:
    ProgramNode(const TSNode& node) : JsBlockNode(node, "Program") {}
    std::set<std::string>& undeclared() { return m_undeclared; }
    void setFilename(std::string fn){ m_fileName = fn; }
    std::string filename() const { return m_fileName; }
protected:
    virtual void insert(BaseNode *child);
    virtual std::string toString(int indent = 0) const;
    virtual void convertToJs(const std::string& source, std::vector<ElementsInsertion*>& fragments, int indent = 0);

private:
    std::vector<ImportNode*>    m_imports;
    std::vector<ComponentDeclarationNode*> m_exports;
    std::set<std::string> m_undeclared;
    std::string m_fileName;

    std::vector<NewComponentExpressionNode*> m_idComponents;

};

class IdentifierNode : public BaseNode{
    friend class BaseNode;
public:
    IdentifierNode(const TSNode& node) : BaseNode(node, "Identifier"){}
};

class VariableDeclaratorNode : public BaseNode{
    friend class BaseNode;
public:
    VariableDeclaratorNode(const TSNode& node) : BaseNode(node, "VariableDeclarator"){}
};

class VariableDeclarationNode : public BaseNode{
    friend class BaseNode;
public:
    VariableDeclarationNode(const TSNode& node) : BaseNode(node, "VariableDeclaration"){}
    virtual std::string toString(int indent = 0) const;
private:
    std::vector<VariableDeclaratorNode*> m_declarators;
};


class CallExpressionNode : public BaseNode{
    friend class BaseNode;
public:
    CallExpressionNode(const TSNode& node) : BaseNode(node, "CallExpression"), m_arguments(nullptr), isSuper(false){}
    virtual std::string toString(int indent = 0) const;
private:
    ArgumentsNode* m_arguments;
    bool isSuper;

    friend class ComponentDeclarationNode;

};

class ImportPathNode : public BaseNode{
    friend class BaseNode;
public:
    ImportPathNode(const TSNode& node) : BaseNode(node, "ImportPath"){}

    const std::vector<IdentifierNode*>& segments() const{ return m_segments; }

private:
    std::vector<IdentifierNode*> m_segments;
};

class NewTaggedComponentExpressionNode : public BaseNode{
    friend class BaseNode;
public:
    NewTaggedComponentExpressionNode(const TSNode& node) : BaseNode(node, "NewTaggedComponentExpression"){}
    virtual std::string toString(int indent = 0) const;
    virtual void convertToJs(const std::string& source, std::vector<ElementsInsertion*>& fragments, int indent = 0);
};

class TaggedStringNode : public BaseNode{
    friend class BaseNode;
public:
    TaggedStringNode(const TSNode& node) : BaseNode(node, "TaggedString"){}
};

class ImportNode : public BaseNode{
    friend class BaseNode;
public:
    ImportNode(const TSNode& node) : BaseNode(node, "Import"), m_importPath(nullptr), m_importAs(nullptr){}
    virtual std::string toString(int indent = 0) const;

    virtual void convertToJs(const std::string& source, std::vector<ElementsInsertion*>& fragments, int indent = 0);
protected:
    virtual void insert(BaseNode *child);
private:
    ImportPathNode* m_importPath;
    IdentifierNode* m_importAs;
};

class FormalParametersNode : public BaseNode{
    friend class BaseNode;
public:
    FormalParametersNode(const TSNode& node) : BaseNode(node, "FormalParameters"){}
    // virtual std::string toString(int indent = 0) const;
    // virtual void convertToJs(const std::string& source, std::vector<ElementsInsertion*>& fragments);
};

class ConstructorDefinitionNode : public BaseNode{
    friend class BaseNode;
public:
    ConstructorDefinitionNode(const TSNode& node)
        : BaseNode(node, "ConstructorDefinition")
        , m_block(nullptr)
        , m_superCall(nullptr)
        , m_formalParameters(nullptr){}
    virtual std::string toString(int indent = 0) const;
    void setSuperCall(CallExpressionNode* super) { m_superCall = super; }
    // virtual void convertToJs(const std::string& source, std::vector<ElementsInsertion*>& fragments);
private:
    StatementBlockNode* m_block;
    CallExpressionNode* m_superCall;
    FormalParametersNode* m_formalParameters;

    friend class ComponentDeclarationNode;

};



class PropertyDeclarationNode : public JsBlockNode{
    friend class BaseNode;
public:
    PropertyDeclarationNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const;
    IdentifierNode* type() const{ return m_type; }
    IdentifierNode* name() const{ return m_name; }
    BindableExpressionNode* expression() const{ return m_expression; }
    StatementBlockNode* statementBlock() const {return m_statementBlock; }
    ComponentDeclarationNode* componentDeclaration() const { return m_componentDeclaration; }

    void pushToBindings(BaseNode* bn) { m_bindings.push_back(bn); }
    bool hasBindings() { return m_bindings.size() > 0; }
    std::vector<BaseNode*>& bindings() { return m_bindings; }
private:
    IdentifierNode* m_type;
    IdentifierNode* m_name;
    BindableExpressionNode* m_expression;
    StatementBlockNode* m_statementBlock;
    ComponentDeclarationNode* m_componentDeclaration;
    std::vector<BaseNode*> m_bindings;
};

class ArgumentsNode : public BaseNode{
    friend class BaseNode;
public:
    ArgumentsNode(const TSNode& node) : BaseNode(node, "Arguments"){}
    virtual std::string toString(int indent = 0) const override;
    virtual void convertToJs(const std::string &source, std::vector<ElementsInsertion*> &fragments, int indent = 0) override;

};

class ClassDeclarationNode : public BaseNode{
public:
    ClassDeclarationNode(const TSNode& node) : BaseNode(node, "ClassDeclaration"){}
};

class PropertyAssignmentNode : public JsBlockNode{
    friend class BaseNode;
public:
    PropertyAssignmentNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const;
    // virtual void convertToJs(const std::string &source, std::vector<ElementsInsertion*> &fragments, int indent = 0) override;

private:
    BaseNode* m_property;
    BindableExpressionNode* m_expression;
    std::vector<BaseNode*> m_bindings;

    friend class BaseNode;
    friend class NewComponentExpressionNode;
};

class PublicFieldDeclarationNode : public BaseNode{
    friend class BaseNode;
public:
    PublicFieldDeclarationNode(const TSNode& node) : BaseNode(node, "PublicFieldDeclaration"){}
};

class ComponentBodyNode : public JsBlockNode{
    friend class BaseNode;
public:
    ComponentBodyNode(const TSNode& node) : JsBlockNode(node, "ComponentBody"), m_constructor(nullptr){}
    virtual std::string toString(int indent = 0) const;
    void setConstructor(ConstructorDefinitionNode* cons) { m_constructor = cons; }
    ConstructorDefinitionNode* constructor() { return m_constructor; }

private:
    ConstructorDefinitionNode* m_constructor;
    friend class ComponentDeclarationNode;

};

class ComponentDeclarationNode : public JsBlockNode{
    friend class BaseNode;
public:
    ComponentDeclarationNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const override;
    virtual void convertToJs(const std::string &source, std::vector<ElementsInsertion*> &fragments, int indent = 0) override;

    void pushToProperties(PropertyDeclarationNode* prop){ m_properties.push_back(prop); }
    void pushToIdComponents(NewComponentExpressionNode* nce){ m_idComponents.push_back(nce); }
    void pushToDefault(BaseNode* nce){ m_default.push_back(nce); }

    void pushToAssignments(PropertyAssignmentNode* ass) { m_assignments.push_back(ass); }
    std::vector<PropertyAssignmentNode*>& assignments() { return m_assignments; }
private:
    IdentifierNode* m_name;
    IdentifierNode* m_id;
    std::vector<IdentifierNode*> m_heritage;
    ComponentBodyNode* m_body;

    std::vector<PropertyDeclarationNode*> m_properties;
    std::vector<EventDeclarationNode*> m_events;
    std::vector<ListenerDeclarationNode*> m_listeners;
    std::vector<BaseNode*> m_default;
    std::vector<NewComponentExpressionNode*> m_idComponents;
    std::vector<PropertyAssignmentNode*> m_assignments;
};

class NewComponentExpressionNode : public JsBlockNode{
    friend class ComponentBodyNode;
public:
    NewComponentExpressionNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const override;
    virtual void convertToJs(const std::string &source, std::vector<ElementsInsertion*> &fragments, int indent = 0) override;

    IdentifierNode* id() { return m_id; }
    IdentifierNode* name() { return m_name; }
    ArgumentsNode* arguments() { return m_arguments; }
    void pushToProperties(PropertyDeclarationNode* prop){ m_properties.push_back(prop); }
    std::vector<PropertyDeclarationNode*>& properties() { return m_properties; }
    void pushToAssignments(PropertyAssignmentNode* ass) { m_assignments.push_back(ass); }
    std::vector<PropertyAssignmentNode*>& assignments() { return m_assignments; }

    void pushToDefault(BaseNode* nce){ m_default.push_back(nce); }
private:
    IdentifierNode* m_name;
    IdentifierNode* m_id;
    ComponentBodyNode* m_body;
    ArgumentsNode* m_arguments;
    IdentifierNode* m_instance;

    std::vector<BaseNode*> m_default;
    std::vector<PropertyDeclarationNode*> m_properties;
    std::vector<PropertyAssignmentNode*> m_assignments;

    friend class BaseNode;

};

class ExpressionStatementNode : public BaseNode{
public:
    ExpressionStatementNode(const TSNode& node) : BaseNode(node, "ExpressionStatement"){}
    virtual std::string toString(int indent = 0) const;
};

class BindableExpressionNode : public BaseNode{
public:
    BindableExpressionNode(const TSNode& node) : BaseNode(node, "BindableExpression"){}
};

class StatementBlockNode : public BaseNode{
public:
    StatementBlockNode(const TSNode& node) : BaseNode(node, "StatementBlock"){}
};

class MemberExpressionNode : public BaseNode{
public:
    MemberExpressionNode(const TSNode& node) : BaseNode(node, "MemberExpression"){}
};

class SubscriptExpressionNode : public BaseNode{
public:
    SubscriptExpressionNode(const TSNode& node) : BaseNode(node, "SubscriptExpression"){}
};

class ParameterListNode : public BaseNode{
    friend class BaseNode;
public:
    ParameterListNode(const TSNode& node) : BaseNode(node, "ParameterList"){}
    virtual std::string toString(int indent = 0) const;
    const std::vector<std::pair<IdentifierNode*, IdentifierNode*> >& parameters() const{ return m_parameters; }
private:
    std::vector<std::pair<IdentifierNode*, IdentifierNode*> > m_parameters;
};

class EventDeclarationNode : public BaseNode{
    friend class BaseNode;
public:
    EventDeclarationNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const;
    IdentifierNode* name() const{ return m_name; }
    ParameterListNode* parameterList() const{ return m_parameters; }
private:
    IdentifierNode* m_name;
    ParameterListNode* m_parameters;
};

class ListenerDeclarationNode : public BaseNode{
    friend class BaseNode;
public:
    ListenerDeclarationNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const;
    IdentifierNode* name() const{ return m_name; }
    ParameterListNode* parameterList() const{ return m_parameters; }
    JsBlockNode* body() const{ return m_body; }
private:
    IdentifierNode*    m_name;
    ParameterListNode* m_parameters;
    JsBlockNode*       m_body;
};

class MethodDefinitionNode : public BaseNode{
    friend class BaseNode;
public:
    MethodDefinitionNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const;

    IdentifierNode* name() const{ return m_name; }
    ParameterListNode* parameters() const{ return m_parameters; }
    JsBlockNode* body() const{ return m_body; }
private:
    IdentifierNode*    m_name;
    ParameterListNode* m_parameters;
    JsBlockNode*       m_body;
};

class TypedFunctionDeclarationNode : public BaseNode{
    friend class BaseNode;
public:
    TypedFunctionDeclarationNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const;

    IdentifierNode* name() const{ return m_name; }
    ParameterListNode* parameters() const{ return m_parameters; }
    JsBlockNode* body() const{ return m_body; }
private:
    IdentifierNode*    m_name;
    ParameterListNode* m_parameters;
    JsBlockNode*       m_body;
};

}} // namespace lv, el


#endif // ELEMENTSSTATEMENTS_H
