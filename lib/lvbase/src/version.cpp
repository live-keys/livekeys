#include "version.h"
#include <sstream>
#include <vector>

namespace lv{

const long long Version::PATCH_MULTIPLIER = 10000;
const long long Version::MINOR_MULTIPLIER = Version::PATCH_MULTIPLIER * 10000;
const long long Version::MAJOR_MULTIPLIER = Version::MINOR_MULTIPLIER * 10000;

Version::Version(int major, int minor, int patch, int revision)
    : m_version(revision + patch * PATCH_MULTIPLIER + minor * MINOR_MULTIPLIER + major * MAJOR_MULTIPLIER )
{
}

Version::Version(const std::string &versionStr){
    int major = 0;
    int minor = 0;
    int patch = 0;
    int revision = 0;

    std::vector<std::string> parts;
    std::istringstream is(versionStr);
    std::string part;

    while (std::getline(is, part, '.'))
        parts.push_back(part);

    if ( parts.size() > 0 )
        major = std::stoi(parts[0]);
    if ( parts.size() > 1 )
        minor = std::stoi(parts[1]);
    if ( parts.size() > 2 )
        patch = std::stoi(parts[2]);
    if ( parts.size() > 3 )
        revision = std::stoi(parts[3]);

    m_version = revision + patch * PATCH_MULTIPLIER + minor * MINOR_MULTIPLIER + major * MAJOR_MULTIPLIER;
}

Version::~Version(){
}

std::string Version::toString() const{
    std::stringstream ss;
    ss << major() << "." << minor() << "." << patch();
    if ( revision() != 0 )
        ss << "." << revision();

    return ss.str();
}

} // namespace
