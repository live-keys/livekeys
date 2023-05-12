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

#ifndef BINDINGPATH_H
#define BINDINGPATH_H

#include <memory>
#include <string>
#include <vector>
#include "live/elements/lvelengineglobal.h"

namespace lv { namespace el {

class LV_ELEMENTS_ENGINE_EXPORT BindingPath{

public:
    friend class BindingChannel;

    typedef std::shared_ptr<BindingPath> Ptr;
    typedef std::shared_ptr<BindingPath> ConstPtr;

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
        virtual std::string toString() const = 0;
        virtual ~Node();
    };

    /// \private
    class IndexNode : public Node{
    public:
        int index;

        virtual Node::Type type() const override{ return Node::Index; }
        virtual std::string toString() const override;
    };

    /// \private
    class PropertyNode : public Node{
    public:
        std::vector<std::string>    objectName;
        std::string                 propertyName;

        virtual Node::Type type() const override{ return Node::Property; }
        virtual std::string toString() const override;
    };

    /// \private
    class FileNode : public Node{
    public:
        std::string  filePath;

        virtual Node::Type type() const override{ return Node::File; }
        virtual std::string toString() const override;
    };

    /// \private
    class ComponentNode : public Node{
    public:
        std::string  name;
        std::string  uri;

        virtual Node::Type type() const override{ return Node::Component; }
        virtual std::string toString() const override;
    };

public:
    virtual ~BindingPath();

    static BindingPath::Ptr create();

    void updatePath(BindingPath::Node* n);
    BindingPath::Node* root(){ return m_root; }
    BindingPath::Node* lastNode();

    void appendFile(const std::string& filePath);
    void appendComponent(const std::string& name, const std::string& uri);
    void appendProperty(const std::string& name, const std::vector<std::string>& objectName);
    void appendIndex(int index);

    static BindingPath::Ptr join(BindingPath::ConstPtr src1, BindingPath::ConstPtr src2, bool firstIndex = true);
    BindingPath::Ptr clone();

    std::string rootFile() const;

    bool operator == (const BindingPath& other) const;
    bool operator != (const BindingPath& other) const;

    virtual std::string toString() const;

private:
    BindingPath();

    std::string             m_documentPath;
    BindingPath::Node*      m_root;
};


}
}

#endif // BINDINGPATH_H
