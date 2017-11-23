#include "environment.h"
#include <QVariant>

namespace lv{

Environment::Environment(QObject *parent)
    : QObject(parent)
{

    QString platform = "unknown";
#if defined(Q_OS_DARWIN)
    platform = "darwin";
#elif defined(Q_OS_SYMBIAN)
    platform = "symbian";
#elif defined(Q_OS_MSDOS)
    platform = "win32";
#elif defined(Q_OS_WIN32)
    platform = "win32";
#elif defined(Q_OS_WINCE)
    platform = "win32";
#elif defined(Q_OS_CYGWIN)
    platform = "win32";
#elif defined(Q_OS_SOLARIS)
    platform = "solaris";
#elif defined(Q_OS_LINUX)
    platform = "linux";
#elif defined(Q_OS_FREEBSD)
    platform = "freebsd";
#elif defined(Q_OS_NETBSD)
    platform = "netbsd";
#elif defined(Q_OS_OPENBSD)
    platform = "openbsd";
#elif defined(Q_OS_AIX)
    platform = "aix";
#elif defined(Q_OS_BSD4)
    platform = "bsd4";
#elif defined(Q_OS_UNIX)
    platform = "unix";
#endif

    m_os["platform"] = platform;
}

Environment::~Environment(){
}

}// namespace
