#ifndef LVMODULE_H
#define LVMODULE_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/metaobject.h"
#include <memory>
#include <string>

namespace lv{ namespace el{

class LV_ELEMENTS_EXPORT Module{

public:
    typedef std::shared_ptr<Module>       Ptr;
    typedef std::shared_ptr<const Module> ConstPtr;

public:
    static Ptr create(const std::string& path, int versionMajor, int versionMinor);

    template<typename T> void addType();

    std::map<std::string, const MetaObject*>::iterator begin();
    std::map<std::string, const MetaObject*>::iterator end();

private:
    Module(const std::string& path, int versionMajor, int versinoMinor);

    std::string m_path;
    int         m_versionMajor;
    int         m_versionMinor;
    std::map<std::string, const MetaObject*> m_types;
};

inline std::map<std::string, const MetaObject*>::iterator Module::begin(){
    return m_types.begin();
}

inline std::map<std::string, const MetaObject*>::iterator Module::end(){
    return m_types.end();
}

template<typename T> void Module::addType(){
    const MetaObject& mo = T::metaObject();
    m_types[mo.name()] = &mo;
}

}} // namespace lv, namespace script

#endif // LVMODULE_H
