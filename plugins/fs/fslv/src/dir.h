#ifndef LVDIR_H
#define LVDIR_H


#include "live/elements/engine.h"
#include "live/elements/element.h"

namespace lv{ namespace el{

class Dir : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(Dir)
            .base<Element>()
            .scriptMethod("mkDir", &Dir::mkDir)
            .scriptMethod("mkPath", &Dir::mkPath)
            .scriptMethod("remove", &Dir::remove)
            .scriptMethod("rename", &Dir::rename)
        META_OBJECT_CLOSE
    }

public:
    Dir(Engine* engine);
    ~Dir();

    bool mkDir(const std::string& path);
    bool mkPath(const std::string& path);
    bool remove(const std::string& path);
    bool rename(const std::string& old, const std::string& nu);
};

}} // namespace lv, el

#endif // LVDIR_H
