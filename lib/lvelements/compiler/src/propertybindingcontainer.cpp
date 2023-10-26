#include "propertybindingcontainer_p.h"
#include "languagenodes_p.h"

#include "live/visuallog.h"

namespace lv{ namespace el{

// PropertyBindingContainer::Node
// ------------------------------------------------------------------

PropertyBindingContainer::Node::~Node(){
    for ( auto it = next.begin(); it != next.end(); ++it ){
        delete *it;
    }
}

PropertyBindingContainer::Node *PropertyBindingContainer::Node::findNextIdentifier(const std::string &name) const{
    for ( auto it = next.begin(); it != next.end(); ++it ){
        if ( (*it)->name == name ){
            return *it;
        }
    }
    return nullptr;
}

// PropertyBindingContainer
// ------------------------------------------------------------------

PropertyBindingContainer::PropertyBindingContainer(){
}

PropertyBindingContainer::~PropertyBindingContainer(){
}

void PropertyBindingContainer::addBinding(BaseNode *binding){
    m_bindings.push_back(binding);
}

void PropertyBindingContainer::setDeclarationCheck(std::function<bool (const std::string&, const std::string &, BaseNode *)> fn){
    m_declarationCheck = fn;
}

std::string PropertyBindingContainer::bindingIdentifiersToString(const std::string &source) const{
    auto result = bindingIdentifiers(source);

    if ( result.empty() )
        return "";

    std::string stringResult;
    for ( auto it = result.begin(); it != result.end(); ++it ){
        if ( it != result.begin() )
            stringResult += "\n";
        stringResult += bindingIdentifierToString(*it);
        delete *it;
    }
    result.clear();

    return stringResult;
}

std::string PropertyBindingContainer::bindingIdentifiersToJs(const std::string &source) const{
    auto result = bindingIdentifiers(source);

    if ( result.empty() )
        return "";

    std::string stringResult = "[";
    for ( auto it = result.begin(); it != result.end(); ++it ){
        if ( it != result.begin() )
            stringResult += ",";
        stringResult += bindingIdentifierToJs(*it);
        delete *it;
    }
    result.clear();

    return stringResult + "]";
}

/*
      this -> x
      [this, x]

      this -> x -> y -> z
      [this, [x, [y, z] ] ]

      this -> x -> a
                -> b
           -> y -> c
      [this, [x, a, b], [y, c]]

      this -> y -> a
                -> b -> c
      [[this, [y, a, [b, c] ] ], ...]
*/
std::vector<PropertyBindingContainer::Node *> PropertyBindingContainer::bindingIdentifiers(const std::string& source) const{
    std::vector<PropertyBindingContainer::Node*> result;

    for (auto idx = m_bindings.begin(); idx != m_bindings.end(); ++idx){
        BaseNode* node = *idx;
        if (node->isNodeType<MemberExpressionNode>() ){
            MemberExpressionNode* men = node->as<MemberExpressionNode>();
            auto identifierChain = men->identifierChain(source);

            if ( identifierChain.size() > 1 ){
                std::string startPoint = identifierChain.front();

                bool isDeclaredInScope = m_declarationCheck && m_declarationCheck(source, startPoint, node);
                if ( isDeclaredInScope )
                    continue;
                bool isImport = startPoint == "import";
                if ( isImport )
                    continue;

                PropertyBindingContainer::Node* foundNode = nullptr;
                for ( auto fit = result.begin(); fit != result.end(); ++fit ){
                    if ( (*fit)->name == startPoint ){
                        foundNode = *fit;
                        break;
                    }
                }

                if ( !foundNode ){
                    PropertyBindingContainer::Node* node = new PropertyBindingContainer::Node;
                    node->name = startPoint;
                    result.push_back(node);
                    for ( size_t i = 1; i < identifierChain.size(); ++i ){
                        PropertyBindingContainer::Node* newNode = new PropertyBindingContainer::Node;
                        newNode->name = identifierChain[i];
                        node->next.push_back(newNode);
                        node = newNode;
                    }
                } else {
                    PropertyBindingContainer::Node* node = foundNode;
                    for ( size_t i = 1; i < identifierChain.size(); ++i ){
                        PropertyBindingContainer::Node* nextNode = node->findNextIdentifier(identifierChain[i]);
                        if ( !nextNode ){
                            for ( size_t j = i; j < identifierChain.size(); ++j ){
                                PropertyBindingContainer::Node* newNode = new PropertyBindingContainer::Node;
                                newNode->name = identifierChain[j];
                                node->next.push_back(newNode);
                                node = newNode;
                            }
                            break;
                        } else {
                            node = nextNode;
                        }
                    }
                }
            }
        }
    }
    return result;
}

std::string PropertyBindingContainer::bindingIdentifierToString(PropertyBindingContainer::Node *n) const{
    std::string result;
    if ( n->next.size() > 0 ){
        result = "[" + n->name;

        for ( auto it = n->next.begin(); it != n->next.end(); ++it ){
            result += "," + bindingIdentifierToString(*it);
        }

        result += "]";
    } else {
        result += n->name;
    }
    return result;
}

std::string PropertyBindingContainer::bindingIdentifierToJs(PropertyBindingContainer::Node *n) const{
    std::string result;
    if ( n->next.size() > 0 ){
        result = "[" + n->name;

        for ( auto it = n->next.begin(); it != n->next.end(); ++it ){
            result += "," + bindingIdentifierPropertyToJs(*it);
        }

        result += "]";
    } else {
        result += n->name;
    }
    return result;
}

std::string PropertyBindingContainer::bindingIdentifierPropertyToJs(PropertyBindingContainer::Node *n) const{
    std::string result;
    if ( n->next.size() > 0 ){
        result = "[";
        if ( n->event.length() > 0 ){
            result += "{n: '" + n->name + "',e: '" + n->event + "'}";
        } else {
            result += "'" + n->name + "'";
        }

        for ( auto it = n->next.begin(); it != n->next.end(); ++it ){
            result += "," + bindingIdentifierPropertyToJs(*it);
        }

        result += "]";
    } else {
        if ( n->event.length() > 0 ){
            result = "{n: '" + n->name + "',e: '" + n->event + "'}";
        } else {
            result = "'" + n->name + "'";
        }
    }
    return result;
}


}} // namespace lv, el
