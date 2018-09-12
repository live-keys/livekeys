#include "module.h"

namespace lv{ namespace el{

Module::Ptr Module::create(const std::string &path, int versionMajor, int versionMinor){
    return std::shared_ptr<Module>(new Module(path, versionMajor, versionMinor));
}

Module::Module(const std::string &path, int versionMajor, int versionMinor)
    : m_path(path)
    , m_versionMajor(versionMajor)
    , m_versionMinor(versionMinor)
{

}

}} // namespace lv, script
