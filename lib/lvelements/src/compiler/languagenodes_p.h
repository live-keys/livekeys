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

#ifndef LVELEMENTSNODES_H
#define LVELEMENTSNODES_H

#include <vector>
#include <map>

#include "live/utf8.h"
#include "live/mlnode.h"

#include "tree_sitter/api.h"
#include "tree_sitter/parser.h"
#include "elementssections_p.h"
#include "languageparser.h"

namespace lv{ namespace el{

class PropertyBindingContainer;

class BaseNode;
class IdentifierNode;
class TypeNode;
class JsBlockNode;
class ImportPathNode;
class ImportNode;
class JsImportNode;
class PropertyDeclarationNode;
class PropertyAssignmentNode;
class ParameterNode;
class ParameterListNode;
class EventDeclarationNode;
class ListenerDeclarationNode;
class TypedMethodDeclarationNode;
class MethodDefinitionNode;
class PublicFieldDeclarationNode;
class ComponentDeclarationNode;
class NewComponentExpressionNode;
class BindableExpressionNode;
class MemberExpressionNode;
class SubscriptExpressionNode;
class ArgumentsNode;
class ObjectNode;
class RootNewComponentExpressionNode;
class PropertyAccessorDeclarationNode;

class BaseNode{
    friend class JsBlockNode;

public:
    class ConversionContext{
    public:
        ConversionContext() : jsImportsEnabled(true), allowUnresolved(true){}

        std::string baseComponent;
        std::string baseComponentImportUri;
        bool        jsImportsEnabled;
        bool        allowUnresolved;
        std::list<std::string> implicitTypes;
        std::string componentPath;
        std::string relativePathFromBuild;

        static std::string baseComponentName(ConversionContext* ctx);
        static std::string baseComponentImport(ConversionContext* ctx);
        static bool isImplicitType(ConversionContext* ctx, const std::string& type);
    };

public:
    BaseNode(const TSNode& node, const std::string& typeString = "Elements");
    virtual ~BaseNode();

    const TSNode& current() const{ return m_node; }
    std::string astString() const;
    virtual std::string toString(int indent = 0) const;

    static BaseNode* visit(const std::string& filePath, const std::string& fileName, LanguageParser::AST* ast);
    static bool checkIdentifierDeclared(const std::string& source, BaseNode* node, std::string id, ConversionContext* ctx);
    const std::string& typeString() const{ return m_typeString; }

    template <typename T> T* as(){ return static_cast<T*>(this); }
    template <typename T> bool canCast(){ return dynamic_cast<T*>(this) != nullptr; }
    const std::vector<BaseNode *> &children() const;

    int startByte() const;
    int endByte() const;
    std::pair<int, int> startPoint() const;
    std::pair<int, int> endPoint() const;
    std::string rangeString() const;
    std::string pointRangeString() const;

    virtual void collectImports(const std::string& source, std::vector<IdentifierNode *> &identifiers, ConversionContext* ctx = nullptr);
    virtual void convertToJs(const std::string& source, std::vector<ElementsInsertion*>& sections, int indent = 0, ConversionContext* ctx = nullptr);

    static std::string slice(const std::string& source, uint32_t start, uint32_t end);
    static std::string slice(const std::string& source, BaseNode* node);

    static JsBlockNode* addToDeclarations(BaseNode* parent, IdentifierNode* idNode);
    static JsBlockNode* addUsedIdentifier(BaseNode* parent, IdentifierNode* idNode);

    BaseNode* parent() const{ return m_parent; }
    void setParent(BaseNode* parent){ m_parent = parent; }

    static TSNode nodeChildByFieldName(const TSNode& node, const std::string& name);
    static std::vector<IdentifierNode*> fromNestedIdentifier(BaseNode* parent, const TSNode& node);
    static ParameterListNode* scanFormalParameters(BaseNode* parent, const TSNode& formalParameters);
    static ParameterListNode* scanFormalTypeParameters(BaseNode* parent, const TSNode& formalParameters);

    static void assertValid(BaseNode* from, const TSNode& node, const std::string& message);
    static void assertError(BaseNode* from, const TSNode& node, const std::string& message);
    static void throwError(BaseNode* from, const TSNode& node, const std::string& message);


protected:
    virtual void addChild(BaseNode *child);

private:
    static void visit(BaseNode* parent, const TSNode& node);
    static void visitChildren(BaseNode* parent, const TSNode& node);
    static void visitImport(BaseNode* parent, const TSNode& node);
    static void visitJsImport(BaseNode* parent, const TSNode& node);
    static void visitIdentifier(BaseNode* parent, const TSNode& node);
    static void visitPropertyIdentifier(BaseNode* parent, const TSNode& node);
    static void visitImportPath(BaseNode* parent, const TSNode& node);
    static void visitComponentDeclaration(BaseNode* parent, const TSNode& node);
    static void visitComponentBody(BaseNode* parent, const TSNode& node);
    static void visitNewComponentExpression(BaseNode* parent, const TSNode& node);
    static void visitComponentInstanceStatement(BaseNode* parent, const TSNode& node);
    static void visitPropertyDeclaration(BaseNode* parent, const TSNode& node);
    static void visitStaticPropertyDeclaration(BaseNode* parent, const TSNode& node);
    static void visitBindableExpression(BaseNode* parent, const TSNode& node);
    static void visitMemberExpression(BaseNode* parent, const TSNode& node);
    static void visitSubscriptExpression(BaseNode* parent, const TSNode& node);
    static void visitPropertyAssignment(BaseNode* parent, const TSNode& node);
    static void visitIdentifierAssignment(BaseNode* parent, const TSNode& node);
    static void visitPublicFieldDeclaration(BaseNode* parent, const TSNode& node);
    static void visitEventDeclaration(BaseNode* parent, const TSNode& node);
    static void visitListenerDeclaration(BaseNode* parent, const TSNode& node);
    static void visitMethodDefinition(BaseNode* parent, const TSNode& node);
    static void visitTypedMethodDeclaration(BaseNode* parent, const TSNode& node);
    static void visitPropertyAccessorDeclaration(BaseNode* parent, const TSNode& node);
    static void visitJsScope(BaseNode* parent, const TSNode& node);
    static void visitNumber(BaseNode* parent, const TSNode& node);
    static void visitConstructorDefinition(BaseNode* parent, const TSNode& node);
    static void visitExpressionStatement(BaseNode* parent, const TSNode& node);
    static void visitAssignmentExpression(BaseNode* parent, const TSNode& node);
    static void visitCallExpression(BaseNode* parent, const TSNode& node);
    static void visitNewTaggedComponentExpression(BaseNode* parent, const TSNode& node);
    static void visitNewTrippleTaggedComponentExpression(BaseNode* parent, const TSNode& node);
    static void visitTaggedString(BaseNode* parent, const TSNode& node);
    static void visitTrippleTaggedString(BaseNode* parent, const TSNode& node);
    static void visitFunctionDeclaration(BaseNode* parent, const TSNode& node);
    static void visitFunction(BaseNode* parent, const TSNode& node);
    static void visitClassDeclaration(BaseNode* parent, const TSNode& node);
    static void visitVariableDeclaration(BaseNode* parent, const TSNode& node);
    static void visitLexicalDeclaration(BaseNode* parent, const TSNode& node);
    static void visitDestructuringPattern(BaseNode* parent, const TSNode& node);
    static void visitNewExpression(BaseNode* parent, const TSNode& node);
    static void visitReturnStatement(BaseNode* parent, const TSNode& node);
    static void visitArrowFunction(BaseNode* parent, const TSNode& node);
    static void visitObject(BaseNode* parent, const TSNode& node);

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

class ObjectNode: public BaseNode {
    friend class BaseNode;
public:
    ObjectNode(const TSNode& node, const std::string& typeString = "Object") : BaseNode(node, typeString){}
};


class ExpressionNode: public BaseNode {
    friend class BaseNode;
public:
    ExpressionNode(const TSNode& node, const std::string& typeString = "Expression") : BaseNode(node, typeString){}
};

class ReturnStatementNode: public BaseNode {
    friend class BaseNode;
public:
    ReturnStatementNode(const TSNode& node, const std::string& typeString = "ReturnStatement") : BaseNode(node, typeString){}
};

class NewExpressionNode: public BaseNode {
    friend class BaseNode;
public:
    NewExpressionNode(const TSNode& node) : BaseNode(node, "NewExpression"){}
};

class JsBlockNode : public BaseNode{

    friend class BaseNode;
    friend class ComponentDeclarationNode;
    friend class NewComponentExpressionNode;

public:
    JsBlockNode(const TSNode& node, const std::string& typeString = "JSScope") : BaseNode(node, typeString){}
    const std::vector<IdentifierNode*>& identifiers() const { return m_declarations; }
    const std::vector<IdentifierNode*>& usedIdentifiers() const{ return m_usedIdentifiers; }

    virtual void collectImports(const std::string& source, std::vector<IdentifierNode *> &identifiers, ConversionContext* ctx = nullptr);

protected:
    void collectBlockImports(const std::string& source, std::vector<IdentifierNode *> &identifiers, ConversionContext* ctx = nullptr);

    std::vector<IdentifierNode*> m_declarations;
    std::vector<IdentifierNode*> m_usedIdentifiers;
};

class ProgramNode : public JsBlockNode {

    friend class BaseNode;

public:
    class ImportType{
    public:
        std::string name;
        std::string importNamespace;
        std::string resolvedPath;
    };

public:
    ProgramNode(const TSNode& node) : JsBlockNode(node, "Program"), m_importTypesCollected(false) {}
    void setFileName(std::string fn){ m_fileName = fn; }
    std::string fileName() const { return m_fileName; }
    void setFilePath(const std::string& fp){ m_filePath = fp; }
    const std::string& filePath() const{ return m_filePath; }

    const std::vector<BaseNode*> exports(){ return m_exports; }
    const std::vector<ImportNode*> imports(){ return m_imports; }
    const std::vector<JsImportNode*> jsImports(){ return m_jsImports; }
    std::map<std::string, std::map<std::string, ImportType> >& importTypes() { return m_importTypes; }

    void collectImportTypes(const std::string& source, ConversionContext* ctx = nullptr);

    void resolveImport(const std::string& as, const std::string& name, const std::string& path);

    std::string importTypesString() const;

protected:
    virtual void addChild(BaseNode *child);
    virtual std::string toString(int indent = 0) const;
    virtual void convertToJs(const std::string& source, std::vector<ElementsInsertion*>& fragments, int indent = 0, ConversionContext* ctx = nullptr);
    virtual void collectImports(const std::string& source, std::vector<IdentifierNode *> &identifiers, ConversionContext* ctx = nullptr);

private:
    void addImportType(const ImportType& t);

private:
    std::string m_fileName;
    std::string m_filePath;

    std::vector<ImportNode*>   m_imports;
    std::vector<BaseNode*>     m_exports; // Exports are not owned
    std::vector<JsImportNode*> m_jsImports;
    bool                       m_importTypesCollected;
    std::map<std::string, std::map<std::string, ImportType> > m_importTypes;
    std::vector<NewComponentExpressionNode*> m_idComponents;
};

class IdentifierNode : public BaseNode{
    friend class BaseNode;
public:
    IdentifierNode(const TSNode& node) : BaseNode(node, "Identifier"){}
};

class TypeNode : public BaseNode{
    friend class BaseNode;
public:
    TypeNode(const TSNode& node) : BaseNode(node, "Type"){}

    static std::string sliceWithoutAnnotation(const std::string &source, TypeNode* tn);
};

class VariableDeclaratorNode : public BaseNode{
    friend class BaseNode;
public:
    VariableDeclaratorNode(const TSNode& node) : BaseNode(node, "VariableDeclarator"){}
};

class VariableDeclarationNode : public BaseNode{
    friend class BaseNode;
public:
    VariableDeclarationNode(const TSNode& node) : BaseNode(node, "VariableDeclaration"), m_hasSemicolon(false){}
    virtual std::string toString(int indent = 0) const;

private:
    std::vector<VariableDeclaratorNode*> m_declarators;
    bool m_hasSemicolon;
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

class ImportPathSegmentNode : public BaseNode{
    friend class BaseNode;
public:
    ImportPathSegmentNode(const TSNode& node) : BaseNode(node, "ImportPathSegment"){}
};

class ImportPathNode : public BaseNode{
    friend class BaseNode;
public:
    ImportPathNode(const TSNode& node) : BaseNode(node, "ImportPath"), m_isRelative(false){}

    bool isRelative(){ return m_isRelative; }
    const std::vector<ImportPathSegmentNode*>& segments() const{ return m_segments; }

private:
    bool m_isRelative;
    std::vector<ImportPathSegmentNode*> m_segments;
};

class NewTaggedComponentExpressionNode : public BaseNode{
    friend class BaseNode;
public:
    NewTaggedComponentExpressionNode(const TSNode& node) : BaseNode(node, "NewTaggedComponentExpression"){}
    virtual std::string toString(int indent = 0) const override;
    virtual void convertToJs(const std::string& source, std::vector<ElementsInsertion*>& fragments, int indent = 0, ConversionContext* ctx = nullptr) override;
};

class NewTrippleTaggedComponentExpressionNode : public BaseNode{
    friend class BaseNode;
public:
    NewTrippleTaggedComponentExpressionNode(const TSNode& node) : BaseNode(node, "NewTrippleTaggedComponentExpression"){}
    virtual std::string toString(int indent = 0) const override;
    virtual void convertToJs(const std::string& source, std::vector<ElementsInsertion*>& fragments, int indent = 0, ConversionContext* ctx = nullptr) override;
};

class TrippleTaggedStringNode : public BaseNode{
    friend class BaseNode;
public:
    TrippleTaggedStringNode(const TSNode& node) : BaseNode(node, "TrippleTaggedString"){}
};

class TaggedStringNode : public BaseNode{
    friend class BaseNode;
public:
    TaggedStringNode(const TSNode& node) : BaseNode(node, "TaggedString"){}
};

class StringNode : public BaseNode{
    friend class BaseNode;
public:
    StringNode(const TSNode& node) : BaseNode(node, "String"){}
};

class JsImportNode : public BaseNode{
    friend class BaseNode;
public:
    JsImportNode(const TSNode& node)
        : BaseNode(node, "JsImport")
        , m_importPath(nullptr)
    {}
    virtual std::string toString(int indent = 0) const;

    const std::vector<IdentifierNode*>& importNames() const{ return m_importNames; }
    BaseNode* importPath() const{ return m_importPath; }

private:
    std::vector<IdentifierNode*> m_importNames;
    BaseNode*                    m_importPath;
};

class ImportNode : public BaseNode{
    friend class BaseNode;
public:
    ImportNode(const TSNode& node)
        : BaseNode(node, "Import")
        , m_importPath(nullptr)
        , m_importAs(nullptr){}
    virtual std::string toString(int indent = 0) const;

    bool isRelative() const{ return m_importPath && m_importPath->isRelative(); }
    std::string path(const std::string& source) const;
    std::string as(const std::string& source) const;
    bool hasNamespace() const{ return m_importAs; }

protected:
    virtual void addChild(BaseNode *child);
private:
    ImportPathNode* m_importPath;
    IdentifierNode* m_importAs;
};

class ConstructorDefinitionNode : public BaseNode{
    friend class BaseNode;
    friend class ComponentDeclarationNode;

public:
    ConstructorDefinitionNode(const TSNode& node)
        : BaseNode(node, "ConstructorDefinition")
        , m_body(nullptr)
        , m_superCall(nullptr)
        , m_parameters(nullptr)
    {}

    virtual std::string toString(int indent = 0) const;
    void setSuperCall(CallExpressionNode* super) { m_superCall = super; }
    // virtual void convertToJs(const std::string& source, std::vector<ElementsInsertion*>& fragments);

private:
    JsBlockNode*        m_body;
    CallExpressionNode* m_superCall;
    ParameterListNode*  m_parameters;

};

class TypedMethodDeclarationNode : public BaseNode{
    friend class BaseNode;
public:
    TypedMethodDeclarationNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const;

    IdentifierNode* name() const{ return m_name; }
    ParameterListNode* parameters() const{ return m_parameters; }
    JsBlockNode* body() const{ return m_body; }

    void setAsync(bool async){ m_async = async; }
    void setStatic(bool value){ m_static = value; }

    bool isAsync() const{ return m_async; }
    bool isStatic() const{ return m_static; }

protected:
    TypedMethodDeclarationNode(const TSNode& node, const std::string& typeString);

private:
    IdentifierNode*    m_name;
    ParameterListNode* m_parameters;
    JsBlockNode*       m_body;
    bool               m_async;
    bool               m_static;
};

class PropertyAccessorDeclarationNode : public TypedMethodDeclarationNode{

public:
    friend class BaseNode;

    enum Access{
        Unknown = 0,
        Getter,
        Setter
    };

    class PropertyAccess{
    public:
        PropertyAccess() : getter(nullptr), setter(nullptr){}

        PropertyAccessorDeclarationNode* getter;
        PropertyAccessorDeclarationNode* setter;
    };

public:
    PropertyAccessorDeclarationNode(const TSNode& node)
        : TypedMethodDeclarationNode(node, "PropertyAccessorDeclaration")
        , m_access(PropertyAccessorDeclarationNode::Unknown)
        , m_isPropertyAttached(false)
    {}

    Access access() const{ return m_access; }
    void setIsPropertyAttached(bool attached) { m_isPropertyAttached = attached; }
    bool isPropertyAttached() const{ return m_isPropertyAttached; }
    IdentifierNode* firstParameterName() const;

private:
    Access m_access;
    bool   m_isPropertyAttached;
};

class PropertyDeclarationNode : public JsBlockNode{

    friend class BaseNode;

public:
    PropertyDeclarationNode(const TSNode& node);
    virtual ~PropertyDeclarationNode();

    virtual std::string toString(int indent = 0) const;
    TypeNode* type() const{ return m_type; }
    IdentifierNode* name() const{ return m_name; }
    BindableExpressionNode* expression() const{ return m_expression; }
    JsBlockNode* statementBlock() const {return m_statementBlock; }

    void pushToBindings(BaseNode* bn);
    std::string bindingIdentifiersToString(const std::string& source) const;
    std::string bindingIdentifiersToJs(const std::string& source) const;

    bool hasAssignment(){ return m_expression != nullptr || m_statementBlock != nullptr; }
    bool isBindingsAssignment(){ return m_isBindingAssignment; }

    void convertToJs(
        const std::string& source,
        const std::string& componentRef,
        int indt,
        ConversionContext* ctx,
        const PropertyAccessorDeclarationNode::PropertyAccess& propertyAccess,
        ElementsInsertion *compose
    );

private:
    IdentifierNode* m_name;
    TypeNode* m_type;
    bool m_isBindingAssignment;
    BindableExpressionNode* m_expression;
    JsBlockNode* m_statementBlock;
    PropertyBindingContainer* m_bindingContainer;
};


class StaticPropertyDeclarationNode : public BaseNode{

    friend class BaseNode;

public:
    StaticPropertyDeclarationNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const;
    TypeNode* type() const{ return m_type; }
    IdentifierNode* name() const{ return m_name; }
    BaseNode* expression() const{ return m_expression; }

    bool hasAssignment(){ return m_expression != nullptr; }

private:
    IdentifierNode* m_name;
    TypeNode*       m_type;
    BaseNode*       m_expression;
};

class ArgumentsNode : public BaseNode{
    friend class BaseNode;
public:
    ArgumentsNode(const TSNode& node) : BaseNode(node, "Arguments"){}
    virtual std::string toString(int indent = 0) const override;
};

class ClassDeclarationNode : public BaseNode{
public:
    ClassDeclarationNode(const TSNode& node) : BaseNode(node, "ClassDeclaration"){}
};

class PropertyAssignmentNode : public JsBlockNode{

    friend class BaseNode;
    friend class ComponentDeclarationNode;
    friend class NewComponentExpressionNode;

public:
    PropertyAssignmentNode(const TSNode& node);
    virtual ~PropertyAssignmentNode();
    virtual std::string toString(int indent = 0) const;

    void pushToBindings(BaseNode* bn);
    std::string bindingIdentifiersToString(const std::string& source) const;
    std::string bindingIdentifiersToJs(const std::string& source) const;
    bool isBindingAssignment() const{ return m_isBindingAssignment; }

private:
    std::vector<IdentifierNode*> m_property;
    bool m_isBindingAssignment;
    BindableExpressionNode*   m_expression;
    JsBlockNode*              m_statementBlock;
    PropertyBindingContainer* m_bindingContainer;
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
    virtual void convertToJs(const std::string &source, std::vector<ElementsInsertion*> &fragments, int indent = 0, ConversionContext* ctx = nullptr) override;

    void pushToProperties(PropertyDeclarationNode* prop){ m_properties.push_back(prop); }
    void pushToStaticProperties(StaticPropertyDeclarationNode* prop){ m_staticProperties.push_back(prop); }
    void pushToIdComponents(NewComponentExpressionNode* nce){ m_idComponents.push_back(nce); }
    void pushToDefault(BaseNode* nce){ m_nestedComponents.push_back(nce); }
    void pushToPropertyAccessors(PropertyAccessorDeclarationNode* pa){ m_propertyAccesors.push_back(pa); }

    void pushToAssignments(PropertyAssignmentNode* pan) { m_assignments.push_back(pan); }
    std::vector<PropertyAssignmentNode*>& assignments() { return m_assignments; }

    PropertyAccessorDeclarationNode::PropertyAccess propertyAccessors(const std::string& source, const std::string& propertyName);

    std::string name(const std::string &source) const;
    const std::vector<IdentifierNode*> heritage() const{ return m_heritage; }

private:
    IdentifierNode* m_name;
    IdentifierNode* m_id;
    std::vector<IdentifierNode*> m_heritage;
    ComponentBodyNode* m_body;

    std::vector<PropertyDeclarationNode*> m_properties;
    std::vector<PropertyAccessorDeclarationNode*> m_propertyAccesors;
    std::vector<StaticPropertyDeclarationNode*> m_staticProperties;
    std::vector<EventDeclarationNode*> m_events;
    std::vector<ListenerDeclarationNode*> m_listeners;
    std::vector<TypedMethodDeclarationNode*> m_methods;
    std::vector<BaseNode*> m_nestedComponents;
    std::vector<PropertyAssignmentNode*> m_assignments;
    std::vector<NewComponentExpressionNode*> m_idComponents;
};

class NewComponentExpressionNode : public JsBlockNode{

    friend class ComponentBodyNode;
    friend class BaseNode;

public:
    NewComponentExpressionNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const override;
    virtual void convertToJs(const std::string &source, std::vector<ElementsInsertion*> &fragments, int indent = 0, ConversionContext* ctx = nullptr) override;

    IdentifierNode* id() { return m_id; }
    std::vector<IdentifierNode*> name() { return m_name; }
    ArgumentsNode* arguments() { return m_arguments; }
    void pushToProperties(PropertyDeclarationNode* prop){ m_properties.push_back(prop); }
    std::vector<PropertyDeclarationNode*>& properties() { return m_properties; }
    void pushToAssignments(PropertyAssignmentNode* ass) { m_assignments.push_back(ass); }
    std::vector<PropertyAssignmentNode*>& assignments() { return m_assignments; }

    std::vector<EventDeclarationNode*> events(){ return m_events; }
    std::vector<ListenerDeclarationNode*> listeners(){ return m_listeners; }
    std::vector<TypedMethodDeclarationNode*> methods(){ return m_methods; }

    void pushToDefault(BaseNode* nce){ m_nestedComponents.push_back(nce); }

    std::string initializerName(const std::string& source);

protected:
    NewComponentExpressionNode(const TSNode& node, const std::string& typeString);

private:
    std::vector<IdentifierNode*> m_name;
    IdentifierNode* m_id;
    ComponentBodyNode* m_body;
    ArgumentsNode* m_arguments;

    std::vector<BaseNode*> m_nestedComponents;
    std::vector<PropertyDeclarationNode*> m_properties;
    std::vector<PropertyAssignmentNode*> m_assignments;
    std::vector<EventDeclarationNode*> m_events;
    std::vector<ListenerDeclarationNode*> m_listeners;
    std::vector<TypedMethodDeclarationNode*> m_methods;
    std::vector<NewComponentExpressionNode*> m_idComponents;
};

class RootNewComponentExpressionNode: public NewComponentExpressionNode {
    friend class BaseNode;
public:
    RootNewComponentExpressionNode(const TSNode& node) : NewComponentExpressionNode(node, "RootNewComponentExpression"){}
};

class ComponentInstanceStatementNode: public BaseNode{
    friend class BaseNode;
public:
    ComponentInstanceStatementNode(const TSNode& node) : BaseNode(node, "ComponentInstanceStatement"), m_name(nullptr){}

    std::string name(const std::string &source) const;

private:
    IdentifierNode* m_name;
};

class ExpressionStatementNode : public BaseNode{
public:
    ExpressionStatementNode(const TSNode& node) : BaseNode(node, "ExpressionStatement"){}
    virtual std::string toString(int indent = 0) const;
};

class AssignmentExpressionNode : public BaseNode{
public:
    AssignmentExpressionNode(const TSNode& node) : BaseNode(node, "AssignmentExpression"), m_left(nullptr), m_right(nullptr){}
    virtual std::string toString(int indent = 0) const;

    BaseNode* left() const{ return m_left; }
    BaseNode* right() const{ return m_right; }

protected:
    virtual void addChild(BaseNode *child);

private:
    BaseNode* m_left;
    BaseNode* m_right;
};

class BindableExpressionNode : public BaseNode{
public:
    BindableExpressionNode(const TSNode& node) : BaseNode(node, "BindableExpression"){}
};

class MemberExpressionNode : public BaseNode{
public:
    MemberExpressionNode(const TSNode& node) : BaseNode(node, "MemberExpression"){}

    std::vector<std::string> identifierChain(const std::string& source) const;
};

class SubscriptExpressionNode : public BaseNode{
public:
    SubscriptExpressionNode(const TSNode& node) : BaseNode(node, "SubscriptExpression"){}
};

class ParameterNode : public BaseNode{
    friend class BaseNode;
public:
    ParameterNode(const TSNode& node, IdentifierNode* identifier, TypeNode* typen = nullptr, bool isOptional = false)
        : BaseNode(node), m_identifier(identifier), m_type(typen), m_isOptional(isOptional)
    {
        if ( m_identifier )
            addChild(m_identifier);
        if ( m_type )
            addChild(m_type);
    }
    IdentifierNode* identifier(){ return m_identifier; }
    TypeNode* type(){ return m_type; }
    bool isOptional(){ return m_isOptional; }

private:
    IdentifierNode* m_identifier;
    TypeNode*       m_type;
    bool            m_isOptional;
};

class ParameterListNode : public BaseNode{
    friend class BaseNode;
public:
    ParameterListNode(const TSNode& node) : BaseNode(node, "ParameterList"){}
    virtual std::string toString(int indent = 0) const;
    const std::vector<ParameterNode*>& parameters() const{ return m_parameters; }

    void addParameter(ParameterNode* node){ m_parameters.push_back(node); addChild(node); }
private:
    std::vector<ParameterNode*> m_parameters;
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
    ExpressionNode* bodyExpression() const{ return m_bodyExpression; }
private:
    IdentifierNode*    m_name;
    ParameterListNode* m_parameters;
    JsBlockNode*       m_body;
    ExpressionNode*    m_bodyExpression;
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


class ArrowFunctionNode: public JsBlockNode{
    friend class BaseNode;
public:
    ArrowFunctionNode(const TSNode& node);

    virtual std::string toString(int indent = 0) const;

    IdentifierNode* name() const{ return m_name; }
    std::vector<IdentifierNode*> parameters() const{ return m_parameters; }
    JsBlockNode* body() const{ return m_body; }
private:
    IdentifierNode*    m_name;
    std::vector<IdentifierNode*> m_parameters;
    JsBlockNode*       m_body;
};

class FunctionNode: public BaseNode {
    friend class BaseNode;
public:
    FunctionNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const;

    ParameterListNode* parameters() const{ return m_parameters; }
    JsBlockNode* body() const{ return m_body; }

protected:
    FunctionNode(const TSNode& node, const std::string& nodeType);

private:
    ParameterListNode* m_parameters;
    JsBlockNode*       m_body;
};

class FunctionDeclarationNode: public FunctionNode{
    friend class BaseNode;
public:
    FunctionDeclarationNode(const TSNode& node);
    virtual std::string toString(int indent = 0) const;

    IdentifierNode* name() const{ return m_name; }

private:
    IdentifierNode* m_name;
};


}} // namespace lv, el


#endif // ELEMENTSSTATEMENTS_H
