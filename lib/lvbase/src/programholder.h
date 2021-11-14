#ifndef LVPROGRAMHOLDER_H
#define LVPROGRAMHOLDER_H

#include "live/lvbaseglobal.h"

namespace lv{

class Program;
class LV_BASE_EXPORT ProgramHolder{

public:
    virtual Program* main() = 0;
    virtual void setMain(Program* main) = 0;
};

}// namespace

#endif // LVPROGRAMHOLDER_H
