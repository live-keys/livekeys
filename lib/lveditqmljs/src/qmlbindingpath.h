#ifndef LVQMLBINDINGPATH_H
#define LVQMLBINDINGPATH_H

#include <QObject>
#include <QQmlProperty>
#include <QSharedPointer>

#include "live/lvglobal.h"

namespace lv{

/// \private
class QmlBindingPath{

public:

    typedef QSharedPointer<QmlBindingPath> Ptr;
    typedef QSharedPointer<QmlBindingPath> ConstPtr;

public:
    /// \private
    class Node{
    public:
        enum Type{ Index, Property, File, Component, Watcher, ContextValue };

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

        virtual Node::Type type() const override{ return Node::Index; }
        virtual QString toString() const override;
    };

    /// \private
    class PropertyNode : public Node{
    public:
        QStringList objectName;
        QString     propertyName;

        virtual Node::Type type() const override{ return Node::Property; }
        virtual QString toString() const override;
    };

    /// \private
    class FileNode : public Node{
    public:
        QString  filePath;

        virtual Node::Type type() const override{ return Node::File; }
        virtual QString toString() const override;
    };

    /// \private
    class ComponentNode : public Node{
    public:
        QString  name;
        QString  uri;

        virtual Node::Type type() const override{ return Node::Component; }
        virtual QString toString() const override;
    };

    /// \private
    class WatcherNode : public Node{
    public:
        QString  filePath;
        QString  objectId;

        virtual Node::Type type() const override{ return Node::Watcher; }
        virtual QString toString() const override;
    };

    /// \private
    class ContextValueNode : public Node{
    public:
        QString name;

        virtual Node::Type type() const override{ return Node::ContextValue; }
        virtual QString toString() const override;
    };

public:
    virtual ~QmlBindingPath();

    static QmlBindingPath::Ptr create();

    void updatePath(QmlBindingPath::Node* n);
    QmlBindingPath::Node* root(){ return m_root; }
    QmlBindingPath::Node* lastNode();

    void appendFile(const QString& filePath);
    void appendComponent(const QString& name, const QString& uri);
    void appendProperty(const QString& name, const QStringList& objectName);
    void appendIndex(int index);
    void appendWatcher(const QString& filePath, const QString& objectId);
    void appendContextValue(const QString& name);

    static QmlBindingPath::Ptr join(QmlBindingPath::ConstPtr src1, QmlBindingPath::ConstPtr src2);
    QmlBindingPath::Ptr clone() const;
    QmlBindingPath::Ptr headPath() const;
    QmlBindingPath::Ptr tailPath();
    QmlBindingPath::Ptr parentObjectPath() const;

    QmlBindingPath::Node* cloneNode(QmlBindingPath::Node* node);

    QString rootFile() const;

    int length() const;

    bool operator == (const QmlBindingPath& other) const;
    bool operator != (const QmlBindingPath& other) const;

    virtual QString toString() const;

private:
    QmlBindingPath();

    DISABLE_COPY(QmlBindingPath);

    QString            m_documentPath;
    QmlBindingPath::Node* m_root;
};

}// namespace


#endif // LVQMLBINDINGPATH_H
