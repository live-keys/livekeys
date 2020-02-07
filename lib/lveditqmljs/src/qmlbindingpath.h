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
    friend class BindingChannel;

    typedef QSharedPointer<QmlBindingPath> Ptr;
    typedef QSharedPointer<QmlBindingPath> ConstPtr;

public:
    /// \private
    class Node{
    public:
        enum Type{ Index, Property, File, Component };

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

    static QmlBindingPath::Ptr join(QmlBindingPath::ConstPtr src1, QmlBindingPath::ConstPtr src2, bool firstIndex = true);
    QmlBindingPath::Ptr clone();

    QString rootFile() const;

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
