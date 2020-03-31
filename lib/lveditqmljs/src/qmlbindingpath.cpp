#include "qmlbindingpath.h"
#include "live/visuallogqt.h"

namespace lv{

QmlBindingPath::Node::~Node(){
    if ( parent )
        parent->child = nullptr;
    delete child;
}

QString QmlBindingPath::IndexNode::toString() const{
    return "Index(" + QString::number(index) + ")" + (child ? ( + " -> " + child->toString()) : "");
}

QString QmlBindingPath::PropertyNode::toString() const{
    QString object = objectName.join(".");
    return "Property(" + propertyName + ")" + "[" + object + "]" + (child ? ( + " -> " + child->toString()) : "");
}

QString QmlBindingPath::FileNode::toString() const{
    return "File(" + filePath.mid(filePath.indexOf("/") + 1) + ")" + (child ? ( + " -> " + child->toString()) : "");
}

QString QmlBindingPath::ComponentNode::toString() const{
    return "Component(" + name + ")" + "[" + uri + "]" + (child ? ( + " -> " + child->toString()) : "");
}

QmlBindingPath::QmlBindingPath()
    : m_root(nullptr)
{
}

QmlBindingPath::~QmlBindingPath(){
    delete m_root;
}

QmlBindingPath::Ptr QmlBindingPath::create(){
    return QmlBindingPath::Ptr(new QmlBindingPath);
}

void QmlBindingPath::updatePath(QmlBindingPath::Node *n){
    if ( m_root )
        delete m_root;
    m_root = n;
}

QmlBindingPath::Node *QmlBindingPath::lastNode(){
    if ( !m_root )
        return nullptr;

    QmlBindingPath::Node* n = m_root;
    while ( n->child ){
        n = n->child;
    }
    return n;
}

void QmlBindingPath::appendFile(const QString &filePath){
    QmlBindingPath::FileNode* fnode = new QmlBindingPath::FileNode;
    fnode->filePath = filePath;

    QmlBindingPath::Node* lnode = lastNode();
    if ( lnode ){
        lnode->child = fnode;
        fnode->parent = lnode;
    } else {
        m_root = fnode;
    }
}

void QmlBindingPath::appendComponent(const QString &name, const QString &uri){
    QmlBindingPath::ComponentNode* cnode = new QmlBindingPath::ComponentNode;
    cnode->name = name;
    cnode->uri  = uri;

    QmlBindingPath::Node* lnode = lastNode();
    if ( lnode ){
        lnode->child = cnode;
        cnode->parent = lnode;
    } else {
        m_root = cnode;
    }
}

void QmlBindingPath::appendProperty(const QString &name, const QStringList &objectName){
    QmlBindingPath::PropertyNode* pnode = new QmlBindingPath::PropertyNode;
    pnode->propertyName = name;
    pnode->objectName   = objectName;

    QmlBindingPath::Node* lnode = lastNode();
    if ( lnode ){
        lnode->child = pnode;
        pnode->parent = lnode;
    } else {
        m_root = pnode;
    }
}

void QmlBindingPath::appendIndex(int index){
    QmlBindingPath::IndexNode* inode = new QmlBindingPath::IndexNode;
    inode->index = index;

    QmlBindingPath::Node* lnode = lastNode();
    if ( lnode ){
        lnode->child = inode;
        inode->parent = lnode;
    } else {
        m_root = inode;
    }
}

QmlBindingPath::Ptr QmlBindingPath::join(QmlBindingPath::ConstPtr src1, QmlBindingPath::ConstPtr src2, bool firstIndex){
    // firstIndex: avoid first index, since that represents the root of the file
    QmlBindingPath::Ptr result = src1->clone();
    QmlBindingPath::Node* n = src2->root();
    while ( n ){
        if ( n->type() == QmlBindingPath::Node::File ){
            QmlBindingPath::FileNode* fnode = static_cast<QmlBindingPath::FileNode*>(n);
            result->appendFile(fnode->filePath);
        } else if ( n->type() == QmlBindingPath::Node::Property ){
            QmlBindingPath::PropertyNode* pnode = static_cast<QmlBindingPath::PropertyNode*>(n);
            result->appendProperty(pnode->propertyName, pnode->objectName);
        } else if ( n->type() == QmlBindingPath::Node::Index ){
            if ( firstIndex ){
                firstIndex = false;
            } else {
                QmlBindingPath::IndexNode* inode = static_cast<QmlBindingPath::IndexNode*>(n);
                result->appendIndex(inode->index);
            }
        } else if ( n->type() == QmlBindingPath::Node::Component ){
            QmlBindingPath::ComponentNode* cnode = static_cast<QmlBindingPath::ComponentNode*>(n);
            result->appendComponent(cnode->name, cnode->uri);
        }
        n = n->child;
    }
    return result;
}

QmlBindingPath::Ptr QmlBindingPath::clone() const{
    QmlBindingPath::Node* cpy     = nullptr;
    QmlBindingPath::Node* cpyroot = nullptr;

    QmlBindingPath::Node* cur = m_root;
    while ( cur ){
        if ( cur->type() == QmlBindingPath::Node::Index ){
            QmlBindingPath::IndexNode* icpy = new QmlBindingPath::IndexNode;
            // set child, parent, index
            if ( cpy ){
                icpy->parent = cpy;
                cpy->child = icpy;
            } else {
                cpyroot = icpy;
            }
            icpy->index = static_cast<QmlBindingPath::IndexNode*>(cur)->index;
            cpy = icpy;
        } else if ( cur->type() == QmlBindingPath::Node::Property ){
            // set child, parent, objectName, propertyName
            QmlBindingPath::PropertyNode* icpy = new QmlBindingPath::PropertyNode;
            if ( cpy ){
                icpy->parent = cpy;
                cpy->child = icpy;
            } else {
                cpyroot = icpy;
            }
            icpy->objectName  = static_cast<QmlBindingPath::PropertyNode*>(cur)->objectName;
            icpy->propertyName = static_cast<QmlBindingPath::PropertyNode*>(cur)->propertyName;
            cpy = icpy;
        } else if ( cur->type() == QmlBindingPath::Node::File ){
            // set child, parent, filePath
            QmlBindingPath::FileNode* icpy = new QmlBindingPath::FileNode;
            if ( cpy ){
                icpy->parent = cpy;
                cpy->child = icpy;
            } else {
                cpyroot = icpy;
            }
            icpy->filePath = static_cast<QmlBindingPath::FileNode*>(cur)->filePath;
            cpy = icpy;
        } else if ( cur->type() == QmlBindingPath::Node::Component ){
            // set child, parent, filePath
            QmlBindingPath::ComponentNode* icpy = new QmlBindingPath::ComponentNode;
            if ( cpy ){
                icpy->parent = cpy;
                cpy->child = icpy;
            } else {
                cpyroot = icpy;
            }
            icpy->name = static_cast<QmlBindingPath::ComponentNode*>(cur)->name;
            icpy->uri  = static_cast<QmlBindingPath::ComponentNode*>(cur)->uri;
            cpy = icpy;
        }

        cur = cur->child;
    }

    QmlBindingPath::Ptr bp = QmlBindingPath::create();
    bp->m_root = cpyroot;
    bp->m_documentPath = m_documentPath;
    return bp;
}

QmlBindingPath::Ptr QmlBindingPath::parentObjectPath() const{

    QmlBindingPath::Ptr result = clone();
    QmlBindingPath::Node* n = result->lastNode();
    while ( n && n->type() != QmlBindingPath::Node::Index ){
        n = n->parent;
    }

    if (!n || !n->parent)
        return nullptr;

    n = n->parent;
    delete n->child;
    n->child = nullptr;

    return result;
}

QString QmlBindingPath::rootFile() const{
    return (m_root && m_root->type() == QmlBindingPath::Node::File)
        ? static_cast<QmlBindingPath::FileNode*>(m_root)->filePath
        : "";
}

bool QmlBindingPath::operator ==(const QmlBindingPath &other) const{
    QmlBindingPath::Node* n = m_root;
    QmlBindingPath::Node* o = other.m_root;
    while (true){
        if ( n == nullptr && o == nullptr )
            return true;
        if ( n == nullptr || o == nullptr )
            return false;

        if ( n->type() == Node::Index && o->type() == n->type() ){
            QmlBindingPath::IndexNode* in = static_cast<QmlBindingPath::IndexNode*>(n);
            QmlBindingPath::IndexNode* on = static_cast<QmlBindingPath::IndexNode*>(o);
            if ( in->index != on->index )
                return false;
        } else if ( n->type() == Node::Property && o->type() == n->type() ){
            QmlBindingPath::PropertyNode* pn = static_cast<QmlBindingPath::PropertyNode*>(n);
            QmlBindingPath::PropertyNode* po = static_cast<QmlBindingPath::PropertyNode*>(o);
            if ( pn->objectName != po->objectName || pn->propertyName != po->propertyName )
                return false;
        } else if ( n->type() == Node::File && o->type() == n->type() ){
            QmlBindingPath::FileNode* pn = static_cast<QmlBindingPath::FileNode*>(n);
            QmlBindingPath::FileNode* po = static_cast<QmlBindingPath::FileNode*>(o);
            if ( pn->filePath != po->filePath )
                return false;
        } else if ( n->type() == Node::Component && o->type() == n->type() ){
            QmlBindingPath::ComponentNode* pn = static_cast<QmlBindingPath::ComponentNode*>(n);
            QmlBindingPath::ComponentNode* po = static_cast<QmlBindingPath::ComponentNode*>(o);
            if ( pn->name != po->name || pn->uri != po->uri )
                return false;
        } else {
            return false;
        }

        n = n->child;
        o = o->child;
    }
}

bool QmlBindingPath::operator !=(const QmlBindingPath &other) const{
    return !(*this == other);
}

QString QmlBindingPath::toString() const{
    return "BindingPath(" + m_root->toString() + ")";
}

}// namespace
