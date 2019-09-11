#ifndef LVVERSION_H
#define LVVERSION_H

#include "lvbaseglobal.h"
#include <string>

namespace lv{

class LV_BASE_EXPORT Version{

public:
    /// \private
    static const long long PATCH_MULTIPLIER;
    /// \private
    static const long long MINOR_MULTIPLIER;
    /// \private
    static const long long MAJOR_MULTIPLIER;

public:
    Version(int majorNumber = 0, int minorNumber = 0, int patchNumber = 0, int revisionNumber = 0);
    Version(const std::string& versionStr);
    ~Version();

    int majorNumber() const;
    int minorNumber() const;
    int patchNumber() const;
    int revisionNumber() const;

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

/**
 * \brief Returns major version
 */
inline int Version::majorNumber() const{
    return static_cast<int>(m_version / MAJOR_MULTIPLIER);
}

/**
 * \brief Returns minor version
 */
inline int Version::minorNumber() const{
    long long cutRight = m_version / MINOR_MULTIPLIER;
    return static_cast<int>(cutRight - ((cutRight / 10000) * 10000));
}

/**
 * \brief Returns patch number
 */
inline int Version::patchNumber() const{
    long long cutRight = m_version / PATCH_MULTIPLIER;
    return static_cast<int>(cutRight - ((cutRight / 10000) * 10000));
}

/**
 * \brief Returns revision number
 */
inline int Version::revisionNumber() const{
    return static_cast<int>(m_version - ((m_version / 10000) * 10000));
}

/**
 * \brief Equals relational operator between two versions
 *
 * Simple check between two long longs!
 */
inline bool Version::operator == (const Version& other) const{
    return m_version == other.m_version;
}
/**
 * \brief Not-equals relational operator between two versions
 *
 * Simple check between two long longs!
 */
inline bool Version::operator != (const Version& other) const{
    return m_version != other.m_version;
}

/**
 * \brief Less-than relational operator between two versions
 *
 * Equivalent to the check between two long longs!
 */
inline bool Version::operator <  (const Version& other) const{
    return m_version < other.m_version;
}
/**
 * \brief Less-or-equal relational operator between two versions
 *
 * Simple check between two long longs!
 */
inline bool Version::operator <= (const Version& other) const{
    return m_version <= other.m_version;
}


/**
 * \brief Greater-than relational operator between two versions
 *
 * Simple check between two long longs!
 */
inline bool Version::operator >  (const Version& other) const{
    return m_version > other.m_version;
}


/**
 * \brief Greater-or-equal relational operator between two versions
 *
 * Equivalent to the check between two long longs!
 */
inline bool Version::operator >= (const Version& other) const	{
    return m_version >= other.m_version;
}

}// namespace

#endif // LVVERSION_H
