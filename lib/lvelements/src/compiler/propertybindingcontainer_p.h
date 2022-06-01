#ifndef LVPROPERTYBINDINGCONTAINER_H
#define LVPROPERTYBINDINGCONTAINER_H

#include <vector>
#include "live/utf8.h"

namespace lv{ namespace el{

class BaseNode;

class PropertyBindingContainer{

public:
    class Node{
    public:
        Node() : parent(nullptr){}

        ~Node();
        Node* findNextIdentifier(const std::string& name) const;

        std::string name;
        std::string event;
        Node*       parent;
        std::vector<Node*> next;
    };

public:
    PropertyBindingContainer();
    ~PropertyBindingContainer();

    void addBinding(BaseNode* binding);
    size_t totalStoredBindings() const;

    void setDeclarationCheck(std::function<bool (const std::string &, const std::string &, BaseNode *)> fn);

    std::string bindingIdentifiersToString(const std::string& source) const;
    std::string bindingIdentifiersToJs(const std::string& source) const;

private:
    std::vector<Node*> bindingIdentifiers(const std::string& source) const;
    std::string bindingIdentifierToString(Node* n) const;
    std::string bindingIdentifierToJs(Node* n) const;
    std::string bindingIdentifierPropertyToJs(Node* n) const;

    std::function<bool(const std::string&, const std::string&, BaseNode*)> m_declarationCheck;
    std::vector<BaseNode*> m_bindings;
};

inline size_t PropertyBindingContainer::totalStoredBindings() const{
    return m_bindings.size();
}

}} // namespace lv, el

#endif // PROPERTYBINDINGCONTAINER_H
