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

    void initializeExports();

    template<typename T> void addType();

    std::map<std::string, const MetaObject*>::iterator begin();
    std::map<std::string, const MetaObject*>::iterator end();

private:
    ModuleLibrary(Engine* engine, const std::string& path);

    ModuleLibraryPrivate* m_d;
    std::map<std::string, const MetaObject*> m_types;
};

inline std::map<std::string, const MetaObject*>::iterator ModuleLibrary::begin(){
    return m_types.begin();
}

inline std::map<std::string, const MetaObject*>::iterator ModuleLibrary::end(){
    return m_types.end();
}

template<typename T> void ModuleLibrary::addType(){
    const MetaObject& mo = T::metaObject();
    m_types[mo.name()] = &mo;
}

}} // namespace lv, el

#endif // LVMODULE_H
