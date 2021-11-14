#ifndef LVPROGRAM_H
#define LVPROGRAM_H

#include "live/lvbaseglobal.h"

namespace lv{

class Utf8;
class LV_BASE_EXPORT Program{

public:
    virtual const Utf8& rootPath() const = 0;

    Program();
    virtual ~Program();
};

}// namespace

#endif // PROGRAM_H
