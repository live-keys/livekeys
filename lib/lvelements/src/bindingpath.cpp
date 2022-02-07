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

#include "bindingpath.h"

namespace lv { namespace el {

BindingPath::Node::~Node(){
    if ( parent )
        parent->child = nullptr;
    delete child;
}

std::string BindingPath::IndexNode::toString() const{
    return "Index(" + std::to_string(index) + ")" + (child ? ( + " -> " + child->toString()) : "");
}

std::string BindingPath::PropertyNode::toString() const{
    std::string object;
    for (auto name: objectName)
    {
        if (!object.empty()) object += ".";
        object += name;
    }
    return "Property(" + propertyName + ")" + "[" + object + "]" + (child ? ( + " -> " + child->toString()) : "");
}

std::string BindingPath::FileNode::toString() const{


    return "File(" + filePath.substr(filePath.find("/") + 1) + ")" + (child ? ( + " -> " + child->toString()) : "");
}

std::string BindingPath::ComponentNode::toString() const{
    return "Component(" + name + ")" + "[" + uri + "]" + (child ? ( + " -> " + child->toString()) : "");
}

BindingPath::BindingPath()
    : m_root(nullptr)
{
}

BindingPath::~BindingPath(){
    delete m_root;
}

BindingPath::Ptr BindingPath::create(){
    return BindingPath::Ptr(new BindingPath);
}

void BindingPath::updatePath(BindingPath::Node *n){
    if ( m_root )
        delete m_root;
    m_root = n;
}

BindingPath::Node *BindingPath::lastNode(){
    if ( !m_root )
        return nullptr;

    BindingPath::Node* n = m_root;
    while ( n->child ){
        n = n->child;
    }
    return n;
}

void BindingPath::appendFile(const std::string &filePath){
    BindingPath::FileNode* fnode = new BindingPath::FileNode;
    fnode->filePath = filePath;

    BindingPath::Node* lnode = lastNode();
    if ( lnode ){
        lnode->child = fnode;
        fnode->parent = lnode;
    } else {
        m_root = fnode;
    }
}

void BindingPath::appendComponent(const std::string &name, const std::string &uri){
    BindingPath::ComponentNode* cnode = new BindingPath::ComponentNode;
    cnode->name = name;
    cnode->uri  = uri;

    BindingPath::Node* lnode = lastNode();
    if ( lnode ){
        lnode->child = cnode;
        cnode->parent = lnode;
    } else {
        m_root = cnode;
    }
}

void BindingPath::appendProperty(const std::string &name, const std::vector<std::string> &objectName){
    BindingPath::PropertyNode* pnode = new BindingPath::PropertyNode;
    pnode->propertyName = name;
    pnode->objectName   = objectName;

    BindingPath::Node* lnode = lastNode();
    if ( lnode ){
        lnode->child = pnode;
        pnode->parent = lnode;
    } else {
        m_root = pnode;
    }
}

void BindingPath::appendIndex(int index){
    BindingPath::IndexNode* inode = new BindingPath::IndexNode;
    inode->index = index;

    BindingPath::Node* lnode = lastNode();
    if ( lnode ){
        lnode->child = inode;
        inode->parent = lnode;
    } else {
        m_root = inode;
    }
}

BindingPath::Ptr BindingPath::join(BindingPath::ConstPtr src1, BindingPath::ConstPtr src2, bool firstIndex){
    // firstIndex: avoid first index, since that represents the root of the file
    BindingPath::Ptr result = src1->clone();
    BindingPath::Node* n = src2->root();
    while ( n ){
        if ( n->type() == BindingPath::Node::File ){
            BindingPath::FileNode* fnode = static_cast<BindingPath::FileNode*>(n);
            result->appendFile(fnode->filePath);
        } else if ( n->type() == BindingPath::Node::Property ){
            BindingPath::PropertyNode* pnode = static_cast<BindingPath::PropertyNode*>(n);
            result->appendProperty(pnode->propertyName, pnode->objectName);
        } else if ( n->type() == BindingPath::Node::Index ){
            if ( firstIndex ){
                firstIndex = false;
            } else {
                BindingPath::IndexNode* inode = static_cast<BindingPath::IndexNode*>(n);
                result->appendIndex(inode->index);
            }
        } else if ( n->type() == BindingPath::Node::Component ){
            BindingPath::ComponentNode* cnode = static_cast<BindingPath::ComponentNode*>(n);
            result->appendComponent(cnode->name, cnode->uri);
        }
        n = n->child;
    }
    return result;
}

BindingPath::Ptr BindingPath::clone(){
    BindingPath::Node* cpy     = nullptr;
    BindingPath::Node* cpyroot = nullptr;

    BindingPath::Node* cur = m_root;
    while ( cur ){
        if ( cur->type() == BindingPath::Node::Index ){
            BindingPath::IndexNode* icpy = new BindingPath::IndexNode;
            // set child, parent, index
            if ( cpy ){
                icpy->parent = cpy;
                cpy->child = icpy;
            } else {
                cpyroot = icpy;
            }
            icpy->index = static_cast<BindingPath::IndexNode*>(cur)->index;
            cpy = icpy;
        } else if ( cur->type() == BindingPath::Node::Property ){
            // set child, parent, objectName, propertyName
            BindingPath::PropertyNode* icpy = new BindingPath::PropertyNode;
            if ( cpy ){
                icpy->parent = cpy;
                cpy->child = icpy;
            } else {
                cpyroot = icpy;
            }
            icpy->objectName  = static_cast<BindingPath::PropertyNode*>(cur)->objectName;
            icpy->propertyName = static_cast<BindingPath::PropertyNode*>(cur)->propertyName;
            cpy = icpy;
        } else if ( cur->type() == BindingPath::Node::File ){
            // set child, parent, filePath
            BindingPath::FileNode* icpy = new BindingPath::FileNode;
            if ( cpy ){
                icpy->parent = cpy;
                cpy->child = icpy;
            } else {
                cpyroot = icpy;
            }
            icpy->filePath = static_cast<BindingPath::FileNode*>(cur)->filePath;
            cpy = icpy;
        } else if ( cur->type() == BindingPath::Node::Component ){
            // set child, parent, filePath
            BindingPath::ComponentNode* icpy = new BindingPath::ComponentNode;
            if ( cpy ){
                icpy->parent = cpy;
                cpy->child = icpy;
            } else {
                cpyroot = icpy;
            }
            icpy->name = static_cast<BindingPath::ComponentNode*>(cur)->name;
            icpy->uri  = static_cast<BindingPath::ComponentNode*>(cur)->uri;
            cpy = icpy;
        }

        cur = cur->child;
    }

    BindingPath::Ptr bp = BindingPath::create();
    bp->m_root = cpyroot;
    bp->m_documentPath = m_documentPath;
    return bp;
}

std::string BindingPath::rootFile() const{
    return (m_root && m_root->type() == BindingPath::Node::File)
        ? static_cast<BindingPath::FileNode*>(m_root)->filePath
        : "";
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
        } else if ( n->type() == Node::File && o->type() == n->type() ){
            BindingPath::FileNode* pn = static_cast<BindingPath::FileNode*>(n);
            BindingPath::FileNode* po = static_cast<BindingPath::FileNode*>(o);
            if ( pn->filePath != po->filePath )
                return false;
        } else if ( n->type() == Node::Component && o->type() == n->type() ){
            BindingPath::ComponentNode* pn = static_cast<BindingPath::ComponentNode*>(n);
            BindingPath::ComponentNode* po = static_cast<BindingPath::ComponentNode*>(o);
            if ( pn->name != po->name || pn->uri != po->uri )
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

std::string BindingPath::toString() const{
    return "BindingPath(" + m_root->toString() + ")";
}

}

}

