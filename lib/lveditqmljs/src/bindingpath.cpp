#include "bindingpath.h"

namespace lv{

BindingPath::BindingPath()
    : m_root(nullptr)
    , m_listIndex(-1)
{
}

BindingPath::~BindingPath(){
    delete m_root;
}

void BindingPath::updatePath(BindingPath::Node *n){
    if ( m_root )
        delete m_root;
    m_root = n;
}

bool BindingPath::operator ==(const BindingPath &other) const{
    BindingPath::Node* n = m_root;
    BindingPath::Node* o = other.m_root;
    while (true){
        if ( n == nullptr && o == nullptr )
            return true;
        if ( n == nullptr || o == nullptr )
            return false;

        if ( n->type() == Node::Index && o->type() == n->type() ){
            BindingPath::IndexNode* in = static_cast<BindingPath::IndexNode*>(n);
            BindingPath::IndexNode* on = static_cast<BindingPath::IndexNode*>(o);
            if ( in->index != on->index )
                return false;
        } else if ( n->type() == Node::Property && o->type() == n->type() ){
            BindingPath::PropertyNode* pn = static_cast<BindingPath::PropertyNode*>(n);
            BindingPath::PropertyNode* po = static_cast<BindingPath::PropertyNode*>(o);
            if ( pn->objectName != po->objectName || pn->propertyName != po->propertyName )
                return false;
        } else {
            return false;
        }

        n = n->child;
        o = o->child;
    }
}

bool BindingPath::operator !=(const BindingPath &other) const{
    return !(*this == other);
}

QString BindingPath::toString() const{
    return "BindingPath(" + m_root->toString() + ")";
}

}// namespace
