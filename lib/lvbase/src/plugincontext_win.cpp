#include "plugincontext.h"
#include <QString>

#include <windows.h>
#include <tchar.h>

namespace lv{

QString PluginContext::applicationFilePathImpl(){
    char* buffer    = 0;
    char* newBuffer = 0;
    int   size      = 0;
    DWORD n         = 0;
    do{
        size += 256; // grow until fits
        newBuffer = (char*)realloc(buffer, size * sizeof(char));
        if ( newBuffer != NULL ){
            buffer = newBuffer;
        } else {
            free(buffer);
            return QString();
        }
        n = GetModuleFileNameA(NULL, buffer, size);
    } while ((int)n >= size);
    if ( buffer != 0 ){
        QString base(buffer);
        free(buffer);
        return base;
    }
    return QString();
}

} // namespace
