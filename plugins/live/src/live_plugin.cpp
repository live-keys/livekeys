#include "live_plugin.h"
#include "qlivecvarguments.h"
#include "qlivecvmain.h"
#include "qstaticloader.h"
#include <qqml.h>

void LivePlugin::registerTypes(const char *uri){
    // @uri modules.live
    qmlRegisterUncreatableType<lcv::QLiveCVArguments>(
        uri, 1, 0, "LiveArguments", "LiveArguments is available through the arguments property."
    );
    qmlRegisterType<lcv::QLiveCVMain>(  uri, 1, 0, "Main");
    qmlRegisterType<lcv::QStaticLoader>(uri, 1, 0, "StaticLoader");
}
