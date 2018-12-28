#include "version.h"
#include <sstream>
#include <vector>

namespace lv{


/**
  \class lv::Version
  \brief Semantic version container

  The version is represented by four values: major version, minor version, patch number, revision number,
  in order of importance. Version is stored as a single long long value, and the four components can then be
  extracted by simple modular arithmetic modulo 10000.
  \ingroup lvbase
 */

const long long Version::PATCH_MULTIPLIER = 10000;
const long long Version::MINOR_MULTIPLIER = Version::PATCH_MULTIPLIER * 10000;
const long long Version::MAJOR_MULTIPLIER = Version::MINOR_MULTIPLIER * 10000;

/**
 * \brief A simple version constructor containing four number parameters in order of importance.
 *
 * Any number of parameters is allowed, the rest default to 0. All numbers except the major version
 * are limited to four digits max by design.
 */
Version::Version(int major, int minor, int patch, int revision)
    : m_version(revision + patch * PATCH_MULTIPLIER + minor * MINOR_MULTIPLIER + major * MAJOR_MULTIPLIER )
{
}

/**
 * \brief A simple constructor from string in format major.minor.patch.revision
 *
 * We can include 0-4 numbers in the string representation, and they will be used in
 * order of importance i.e. if we only have three numbers, we will default the revision number to 0.
 * \sa Version(int major, int minor, int patch, int revision)
 */
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

/**
 * \brief Version destructor (blank)
 */
Version::~Version(){
}

/**
 * \brief String representation of version, in the form of major.minor.patch[.revision]
 *
 * Revision is only included if it's non-zero! Other components are extracted with their respective functions
 */
std::string Version::toString() const{
    std::stringstream ss;
    ss << major() << "." << minor() << "." << patch();
    if ( revision() != 0 )
        ss << "." << revision();

    return ss.str();
}

} // namespace
