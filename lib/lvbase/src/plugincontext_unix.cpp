#include "plugincontext.h"
#include <QString>

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <utime.h>
#include <cstring>
#include <linux/limits.h>

namespace lv{

QString PluginContext::applicationFilePathImpl(){
    char link[PATH_MAX];
    ssize_t linkSize;
    if ( ( linkSize = readlink("/proc/self/exe", link, sizeof(link) - 1) ) != -1 ){
        link[linkSize] = '\0';
        return QString(link);
    } else {
        return QString();
    }
}

} // namespace
