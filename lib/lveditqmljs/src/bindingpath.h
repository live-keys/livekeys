#ifndef BINDINGPATH_H
#define BINDINGPATH_H

#include <QObject>
#include <QQmlProperty>

namespace lv{

/// \private
class BindingPath{

public:
    friend class BindingChannel;

public:
    /// \private
    class Node{
    public:
        enum Type{ Index, Property };

    public:
        Node* child;
        Node* parent;

        Node() : child(nullptr), parent(nullptr){}

        virtual Node::Type type() const = 0;
        virtual QString toString() const = 0;
        virtual ~Node();
    };

    /// \private
    class IndexNode : public Node{
    public:
        int index;

        virtual Node::Type type() const Q_DECL_OVERRIDE{ return Node::Index; }
        virtual QString toString() const Q_DECL_OVERRIDE;
    };

    /// \private
    class PropertyNode : public Node{
    public:
        QStringList objectName;
        QString     propertyName;

        virtual Node::Type type() const Q_DECL_OVERRIDE{ return Node::Property; }
        virtual QString toString() const Q_DECL_OVERRIDE;
    };

    BindingPath();
    virtual ~BindingPath();

    bool hasConnection();
    void updateConnection(const QQmlProperty& property, int listIndex = -1);
    void clearConnection();
    void updatePath(BindingPath::Node* n);
    BindingPath::Node* root(){ return m_root; }

    int listIndex() const{ return m_listIndex; }
    QQmlProperty& property(){ return m_property; }

    bool operator == (const BindingPath& other) const;
    bool operator != (const BindingPath& other) const;

    virtual QString toString() const;

private:
    QString            m_documentPath;
    BindingPath::Node* m_root;
    QQmlProperty       m_property;
    int                m_listIndex;
};

inline BindingPath::Node::~Node(){
    if ( parent )
        parent->child = nullptr;
    delete child;
}

inline QString BindingPath::IndexNode::toString() const{
    return "Index(" + QString::number(index) + ")" + (child ? ( + " -> " + child->toString()) : "");
}

inline QString BindingPath::PropertyNode::toString() const{
    QString object = objectName.join(".");
    return "Property(" + propertyName + ")" + "[" + object + "]" + (child ? ( + " -> " + child->toString()) : "");
}

inline bool BindingPath::hasConnection(){
    return m_property.isValid();
}

inline void BindingPath::updateConnection(const QQmlProperty &property, int listIndex){
    m_property = property;
    m_listIndex = listIndex;
}

inline void BindingPath::clearConnection(){
    m_property = QQmlProperty();
    m_listIndex = -1;
}


}// namespace


#endif // BINDINGPATH_H
