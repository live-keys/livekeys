#ifndef LVPLUGIN_H
#define LVPLUGIN_H

#include "live/lvbaseglobal.h"
#include <string>

namespace lv{

class LV_BASE_EXPORT Plugin{

public:
    static const std::string fileName;

public:
    Plugin();
    ~Plugin();
};

} // namespace

#endif // PLUGIN_H
