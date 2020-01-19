#ifndef LVMODULE_H
#define LVMODULE_H

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

    void loadExports(const Object &exportsObject);

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

#endif // LVMODULE_H
