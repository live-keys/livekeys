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

#ifndef LVELEMENTSSECTIONS_H
#define LVELEMENTSSECTIONS_H

#include <vector>
#include "languageparser.h"
#include "live/utf8.h"

namespace lv{ namespace el{

class ElementsInsertion;
class JSSection;

class InsertionSection{
public:
    enum Type{
        Insertion = 0,
        Elements,
        Js
    };
    InsertionSection(): type(Insertion){}

    virtual ~InsertionSection(){}
    virtual std::string toString() const{return content + "\n";}
    virtual void flatten(const std::string&, std::vector<std::string>& parts){
        if ( content.size() > 0 )
            parts.push_back(content);
    }

protected:
    InsertionSection(Type t) : type(t){}

public:
    Type        type;
    std::string content;
};

class JSSection : public InsertionSection{
public:
    int from;
    int to;
    std::vector<ElementsInsertion*> m_children;

    JSSection(int pFrom = 0, int pTo = 0): InsertionSection(Js), from(pFrom), to(pTo){}

    virtual std::string toString() const;

    virtual void flatten(const std::string& source, std::vector<std::string>& parts);
};

class ElementsInsertion : public InsertionSection{
public:
    int from;
    int to;
    std::vector<InsertionSection*> m_children;

    ElementsInsertion(): InsertionSection(Elements){}

    ElementsInsertion& operator<<(const std::string& content){
        if ( m_children.empty() ){
            InsertionSection* is = new InsertionSection;
            is->content = content;
            m_children.push_back(is);
        } else if ( m_children[m_children.size() - 1]->type == Insertion ){
            m_children[m_children.size() - 1]->content += content;
        } else {
            InsertionSection* is = new InsertionSection;
            is->content = content;
            m_children.push_back(is);
        }
        return *this;
    }

    ElementsInsertion& operator<<(JSSection* section){
        m_children.push_back(section);
        return *this;
    }

    virtual void flatten(const std::string& source, std::vector<std::string>& parts){
        for ( auto it = m_children.begin(); it != m_children.end(); ++it ){
            InsertionSection* ei = *it;
            ei->flatten(source, parts);
        }
    }

    std::string toString() const{
        std::string base = "[ELEMENTSSCOPE]\n";
        for ( InsertionSection const* child : m_children ){
            base += child->toString();
        }
        base += "[/ELEMENTSSCOPE]\n";
        return base;
    }

};

inline std::string JSSection::toString() const{
    std::string base = "[JSSCOPE " + std::to_string(from) + " -> " + std::to_string(to) + "]\n";
    for ( ElementsInsertion* child : m_children ){
        base += child->toString();
    }
    base += "[/JSSCOPE]\n";
    return base;
}

inline void JSSection::flatten(const std::string &source, std::vector<std::string> &parts){
    int lastSegmentStart = from;
    for ( auto it = m_children.begin(); it != m_children.end(); ++it ){
        ElementsInsertion* ei = *it;
        if ( ei->from > lastSegmentStart ){
            std::string midJs = source.substr(lastSegmentStart, ei->from - lastSegmentStart);;
            Utf8::trim(midJs);
            if ( midJs.size() > 0 )
                parts.push_back(midJs);
        }
        ei->flatten(source, parts);
        lastSegmentStart = ei->to;
    }

    if ( lastSegmentStart < to ){
        parts.push_back(source.substr(lastSegmentStart, to - lastSegmentStart));
    }
}

}}// namespace

#endif // LVELEMENTSSECTIONS_H
