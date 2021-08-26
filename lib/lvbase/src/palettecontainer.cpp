#include "palettecontainer.h"

namespace lv{

const Utf8 &PaletteContainer::PaletteInfo::type() const{
    return m_type;
}

const Utf8 &PaletteContainer::PaletteInfo::path() const{
    return m_path;
}

const Utf8 &PaletteContainer::PaletteInfo::name() const{
    return m_name;
}

const Utf8 &PaletteContainer::PaletteInfo::extension() const{
    return m_extension;
}

const Utf8 &PaletteContainer::PaletteInfo::plugin() const{
    return m_plugin;
}

bool PaletteContainer::PaletteInfo::isValid() const{
    return m_path.utfLength() > 0;
}

PaletteContainer::PaletteInfo::PaletteInfo(
        const Utf8 &type, const Utf8 &path, const Utf8 &name, const Utf8 &extension, const Utf8 &plugin)
    : m_type(type)
    , m_path(path)
    , m_name(name)
    , m_extension(extension)
    , m_plugin(plugin)
{
}


PaletteContainer::PaletteContainer()
    : m_palettes(new std::map<std::string, std::list<PaletteInfo> >)
{
}

PaletteContainer::~PaletteContainer(){
    delete m_palettes;
}

void PaletteContainer::addPalette(const lv::Utf8 &type, const lv::Utf8 &path, const lv::Utf8 &name, const lv::Utf8 &extension, const lv::Utf8 &plugin){
    PaletteContainer::PaletteInfo pi(type, path, name, extension, plugin);

    auto it = m_palettes->find(type.data());
    if ( it != m_palettes->end() ){
        for ( auto pit = it->second.begin(); pit != it->second.end(); ++pit ){
            if ( pit->path() == pi.path() )
                return;
        }
        it->second.push_back(pi);
    } else {
        std::list<PaletteContainer::PaletteInfo> newValue;
        newValue.push_back(pi);
        (*m_palettes)[type.data()] = newValue;
    }
}

void PaletteContainer::clear(){
    m_palettes->clear();
}

Utf8 PaletteContainer::toString() const{
    std::string result;
    for ( auto it = m_palettes->begin(); it != m_palettes->end(); ++it ){
        result += it->first + "\n";
        for ( auto pit = it->second.begin(); pit != it->second.end(); ++pit ){
            result += "  " + pit->path().data() + "[" + pit->extension().data() + "]\n";
        }
    }
    return Utf8(result);
}

PaletteContainer::PaletteInfo PaletteContainer::findPaletteByName(const Utf8 &name) const{
    for ( auto it = m_palettes->begin(); it != m_palettes->end(); ++it ){
        for ( auto pit = it->second.begin(); pit != it->second.end(); ++pit ){
            if ( pit->name() == name ){
                return *pit;
            }
        }
    }
    return PaletteContainer::PaletteInfo();
}

PaletteContainer::PaletteInfo PaletteContainer::findFirstPalette(const Utf8 &type) const{
    auto it = m_palettes->find(type.data());
    if ( it != m_palettes->end() ){
        return it->second.size() > 0 ? it->second.front() : PaletteContainer::PaletteInfo();
    }
    return PaletteContainer::PaletteInfo();
}

std::list<PaletteContainer::PaletteInfo> PaletteContainer::findPalettes(const Utf8 &type) const{
    auto it = m_palettes->find(type.data());
    if ( it != m_palettes->end() ){
        return it->second;
    }
    return std::list<PaletteContainer::PaletteInfo>();
}

}// namespace
