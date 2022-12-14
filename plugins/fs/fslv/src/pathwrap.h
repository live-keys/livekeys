#ifndef LVPATH_H
#define LVPATH_H

#include "live/elements/engine.h"
#include "live/elements/element.h"

namespace lv{ namespace el{

class PathWrap : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(PathWrap)
            .base<Element>()
            .scriptMethod("name", &PathWrap::name)
            .scriptMethod("exists", &PathWrap::name)
        META_OBJECT_CLOSE
    }

public:
    PathWrap(Engine* engine);
    ~PathWrap() override;

    std::string name(const std::string& path) const;
    bool exists(const std::string& path) const;
};

}} // namespace lv, el

#endif // LVPATH_H
