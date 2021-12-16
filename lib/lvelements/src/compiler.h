#ifndef LVCOMPILER
#define LVCOMPILER

#include "live/elements/lvelementsglobal.h"
#include "live/utf8.h"

namespace lv{ namespace el{

class Compiler{

public:
    class Options{
    public:
        bool hasCustomElement();

    public:
        std::string elementPath;
    };

public:
    Compiler();
};

}}// namespace lv,  el

#endif // LVCOMPILER
