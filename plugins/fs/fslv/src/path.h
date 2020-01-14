#ifndef LVPATH_H
#define LVPATH_H

#include "live/elements/engine.h"
#include "live/elements/element.h"

namespace lv{ namespace el{

class Path : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(Path)
            .base<Element>()
            .scriptMethod("name", &Path::name)
            .scriptMethod("exists", &Path::name)
        META_OBJECT_CLOSE
    }

public:
    Path(Engine* engine);
    ~Path() override;

    std::string name(const std::string& path) const;
    bool exists(const std::string& path) const;
};

}} // namespace lv, el

#endif // LVPATH_H
