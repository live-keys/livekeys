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

QString QmlBindingPath::WatcherNode::toString() const{
    return "Watcher(" + filePath.mid(filePath.indexOf("/") + 1) + ")" + "[" + objectId + "]" + (child ? ( + " -> " + child->toString()) : "");
}

QString QmlBindingPath::ContextValueNode::toString() const{
    return "ContextValue(" + name + ")";
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

void QmlBindingPath::appendWatcher(const QString &filePath, const QString &objectId){
    QmlBindingPath::WatcherNode* wnode = new QmlBindingPath::WatcherNode;
    wnode->filePath = filePath;
    wnode->objectId = objectId;

    QmlBindingPath::Node* lnode = lastNode();
    if ( lnode ){
        lnode->child = wnode;
        wnode->parent = lnode;
    } else {
        m_root = wnode;
    }
}

void QmlBindingPath::appendContextValue(const QString &name){
    QmlBindingPath::ContextValueNode* vnode = new QmlBindingPath::ContextValueNode;
    vnode->name = name;

    QmlBindingPath::Node* lnode = lastNode();
    if ( lnode ){
        lnode->child = vnode;
        vnode->parent = lnode;
    } else {
        m_root = vnode;
    }
}

QmlBindingPath::Ptr QmlBindingPath::join(QmlBindingPath::ConstPtr src1, QmlBindingPath::ConstPtr src2){
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
            QmlBindingPath::IndexNode* inode = static_cast<QmlBindingPath::IndexNode*>(n);
            result->appendIndex(inode->index);
        } else if ( n->type() == QmlBindingPath::Node::Component ){
            QmlBindingPath::ComponentNode* cnode = static_cast<QmlBindingPath::ComponentNode*>(n);
            result->appendComponent(cnode->name, cnode->uri);
        } else if ( n->type() == QmlBindingPath::Node::Watcher ){
            QmlBindingPath::WatcherNode* wnode = static_cast<QmlBindingPath::WatcherNode*>(n);
            result->appendWatcher(wnode->filePath, wnode->objectId);
        } else if ( n->type() == QmlBindingPath::Node::ContextValue ){
            QmlBindingPath::ContextValueNode* vnode = static_cast<QmlBindingPath::ContextValueNode*>(n);
            result->appendContextValue(vnode->name);
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
        } else if ( cur->type() == QmlBindingPath::Node::Watcher ){
            // set child, parent, filePath
            QmlBindingPath::WatcherNode* icpy = new QmlBindingPath::WatcherNode;
            if ( cpy ){
                icpy->parent = cpy;
                cpy->child = icpy;
            } else {
                cpyroot = icpy;
            }
            icpy->filePath = static_cast<QmlBindingPath::WatcherNode*>(cur)->filePath;
            icpy->objectId = static_cast<QmlBindingPath::WatcherNode*>(cur)->objectId;
            cpy = icpy;
        } else if ( cur->type() == QmlBindingPath::Node::ContextValue ){
            // set child, parent, filePath
            QmlBindingPath::ContextValueNode* icpy = new QmlBindingPath::ContextValueNode;
            if ( cpy ){
                icpy->parent = cpy;
                cpy->child = icpy;
            } else {
                cpyroot = icpy;
            }
            icpy->name = static_cast<QmlBindingPath::ContextValueNode*>(cur)->name;
            cpy = icpy;
        }

        cur = cur->child;
    }

    QmlBindingPath::Ptr bp = QmlBindingPath::create();
    bp->m_root = cpyroot;
    bp->m_documentPath = m_documentPath;
    return bp;
}

QmlBindingPath::Ptr QmlBindingPath::headPath() const{
    QmlBindingPath::Ptr result = clone();
    QmlBindingPath::Node* n = result->lastNode();
    if ( n == result->m_root )
        result->m_root = nullptr;
    delete n;
    return result;
}

QmlBindingPath::Ptr QmlBindingPath::tailPath(){
    QmlBindingPath::Node* n = lastNode();
    QmlBindingPath::Node* cpy = cloneNode(n);
    QmlBindingPath::Ptr result = QmlBindingPath::create();
    result->updatePath(cpy);
    return result;
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

QmlBindingPath::Node *QmlBindingPath::cloneNode(QmlBindingPath::Node *cur){
    if ( cur->type() == QmlBindingPath::Node::Index ){
        QmlBindingPath::IndexNode* icpy = new QmlBindingPath::IndexNode;
        icpy->index = static_cast<QmlBindingPath::IndexNode*>(cur)->index;
        return icpy;
    } else if ( cur->type() == QmlBindingPath::Node::Property ){
        // set child, parent, objectName, propertyName
        QmlBindingPath::PropertyNode* icpy = new QmlBindingPath::PropertyNode;
        icpy->objectName  = static_cast<QmlBindingPath::PropertyNode*>(cur)->objectName;
        icpy->propertyName = static_cast<QmlBindingPath::PropertyNode*>(cur)->propertyName;
        return icpy;
    } else if ( cur->type() == QmlBindingPath::Node::File ){
        // set child, parent, filePath
        QmlBindingPath::FileNode* icpy = new QmlBindingPath::FileNode;
        icpy->filePath = static_cast<QmlBindingPath::FileNode*>(cur)->filePath;
        return icpy;
    } else if ( cur->type() == QmlBindingPath::Node::Component ){
        // set child, parent, filePath
        QmlBindingPath::ComponentNode* icpy = new QmlBindingPath::ComponentNode;
        icpy->name = static_cast<QmlBindingPath::ComponentNode*>(cur)->name;
        icpy->uri  = static_cast<QmlBindingPath::ComponentNode*>(cur)->uri;
        return icpy;
    } else if ( cur->type() == QmlBindingPath::Node::Watcher ){
        // set child, parent, filePath
        QmlBindingPath::WatcherNode* icpy = new QmlBindingPath::WatcherNode;
        icpy->filePath = static_cast<QmlBindingPath::WatcherNode*>(cur)->filePath;
        icpy->objectId = static_cast<QmlBindingPath::WatcherNode*>(cur)->objectId;
        return icpy;
    } else if ( cur->type() == QmlBindingPath::Node::ContextValue ){
        // set child, parent, filePath
        QmlBindingPath::ContextValueNode* icpy = new QmlBindingPath::ContextValueNode;
        icpy->name = static_cast<QmlBindingPath::ContextValueNode*>(cur)->name;
        return icpy;
    }
    return nullptr;
}

QString QmlBindingPath::rootFile() const{
    return (m_root && m_root->type() == QmlBindingPath::Node::File)
        ? static_cast<QmlBindingPath::FileNode*>(m_root)->filePath
        : "";
}

int QmlBindingPath::length() const{
    if ( !m_root )
        return 0;

    int i = 1;

    QmlBindingPath::Node* n = m_root;
    while ( n->child ){
        n = n->child;
        ++i;
    }
    return i;
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
        } else if ( n->type() == Node::Watcher && o->type() == n->type() ){
            QmlBindingPath::WatcherNode* pn = static_cast<QmlBindingPath::WatcherNode*>(n);
            QmlBindingPath::WatcherNode* po = static_cast<QmlBindingPath::WatcherNode*>(o);
            if ( pn->filePath != po->filePath || pn->objectId != po->objectId )
                return false;
        } else if ( n->type() == Node::ContextValue && o->type() == n->type() ){
            QmlBindingPath::ContextValueNode* pn = static_cast<QmlBindingPath::ContextValueNode*>(n);
            QmlBindingPath::ContextValueNode* po = static_cast<QmlBindingPath::ContextValueNode*>(o);
            if ( pn->name != po->name )
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
    return "BindingPath(" + (m_root ? m_root->toString() : "null") + ")";
}

}// namespace
