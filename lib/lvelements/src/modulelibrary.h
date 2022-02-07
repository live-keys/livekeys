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

#ifndef LVMODULELIBRARY_H
#define LVMODULELIBRARY_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/metaobject.h"

#include <memory>
#include <string>

namespace lv{ namespace el{

class ModuleLibraryPrivate;
class LV_ELEMENTS_EXPORT ModuleLibrary{

    DISABLE_COPY(ModuleLibrary);

public:
    ~ModuleLibrary();

    static ModuleLibrary* create(Engine* engine, const std::string& path);
    static ModuleLibrary* load(Engine *engine, const std::string& path);

    void addInstance(const std::string& name, Element* element);
    template<typename T> void addType();

    std::map<std::string, const MetaObject*>::iterator typesBegin();
    std::map<std::string, const MetaObject*>::iterator typesEnd();

    std::map<std::string, Element*>::iterator instancesBegin();
    std::map<std::string, Element*>::iterator instancesEnd();

    Engine* engine();

    const std::string& path() const;

private:
    ModuleLibrary(Engine* engine, const std::string& path);

    ModuleLibraryPrivate* m_d;
    std::map<std::string, Element*>          m_instances;
    std::map<std::string, const MetaObject*> m_types;
};

inline std::map<std::string, const MetaObject*>::iterator ModuleLibrary::typesBegin(){
    return m_types.begin();
}

inline std::map<std::string, const MetaObject*>::iterator ModuleLibrary::typesEnd(){
    return m_types.end();
}

inline std::map<std::string, Element*>::iterator ModuleLibrary::instancesBegin(){
    return m_instances.begin();
}

inline std::map<std::string, Element*>::iterator ModuleLibrary::instancesEnd(){
    return m_instances.end();
}

template<typename T> void ModuleLibrary::addType(){
    const MetaObject& mo = T::metaObject();
    m_types[mo.name()] = &mo;
}

}} // namespace lv, el

#endif // LVMODULELIBRARY_H
