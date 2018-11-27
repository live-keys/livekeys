#ifndef LVVERSION_H
#define LVVERSION_H

#include "lvbaseglobal.h"
#include <string>

namespace lv{

class LV_BASE_EXPORT Version{

public:
    static const long long PATCH_MULTIPLIER;
    static const long long MINOR_MULTIPLIER;
    static const long long MAJOR_MULTIPLIER;

public:
    Version(int major = 0, int minor = 0, int patch = 0, int revision = 0);
    Version(const std::string& versionStr);
    ~Version();

    int major() const;
    int minor() const;
    int patch() const;
    int revision() const;

    bool operator == (const Version& version) const;
    bool operator != (const Version& version) const;
    bool operator <  (const Version& version) const;
    bool operator <= (const Version& version) const;
    bool operator >  (const Version& version) const;
    bool operator >= (const Version& version) const;

    std::string toString() const;

private:
    long long m_version;
};

inline int Version::major() const{
    return static_cast<int>(m_version / MAJOR_MULTIPLIER);
}

inline int Version::minor() const{
    long long cutRight = m_version / MINOR_MULTIPLIER;
    return static_cast<int>(cutRight - ((cutRight / 10000) * 10000));
}

inline int Version::patch() const{
    long long cutRight = m_version / PATCH_MULTIPLIER;
    return static_cast<int>(cutRight - ((cutRight / 10000) * 10000));
}

inline int Version::revision() const{
    return static_cast<int>(m_version - ((m_version / 10000) * 10000));
}

inline bool Version::operator == (const Version& other) const{
    return m_version == other.m_version;
}

inline bool Version::operator != (const Version& other) const{
    return m_version != other.m_version;
}

inline bool Version::operator <  (const Version& other) const{
    return m_version < other.m_version;
}

inline bool Version::operator <= (const Version& other) const{
    return m_version <= other.m_version;
}

inline bool Version::operator >  (const Version& other) const{
    return m_version > other.m_version;
}

inline bool Version::operator >= (const Version& other) const	{
    return m_version >= other.m_version;
}

}// namespace

#endif // LVVERSION_H
