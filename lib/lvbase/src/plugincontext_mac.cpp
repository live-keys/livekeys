#include "plugincontext.h"
#include <QString>

#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include "mach-o/dyld.h"

namespace lv{

QString PluginContext::applicationFilePathImpl(){
    uint32_t pathNameSize = 0;
    _NSGetExecutablePath(NULL, &pathNameSize);
    char relativePath[pathNameSize];
    if ( _NSGetExecutablePath(relativePath, &pathNameSize) == 0 ){
        char fullPath[PATH_MAX];
        if ( realpath(relativePath, fullPath) != NULL )
            return QString(fullPath);
        else
            return QString(relativePath);
    } else
        return QString();
}

}// namespace

