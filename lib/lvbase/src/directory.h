#ifndef LVDIRECTORY_H
#define LVDIRECTORY_H

#include "live/lvbaseglobal.h"
#include "live/visuallog.h"

namespace lv{

class LV_BASE_EXPORT Directory{

public:
    class IteratorPrivate;
    class LV_BASE_EXPORT Iterator{
    public:
        Iterator(const std::string& path);
        Iterator(const Iterator& other);
        Iterator& operator = (const Iterator& other);
        ~Iterator();

        void next();
        bool isEnd() const;
        bool hasEntry() const;

        std::string path();

    private:
        IteratorPrivate* m_d;
    };

public:
    static Iterator iterate(const std::string& path);

private:
    Directory();
};

}// namespace

#endif // LVDIRECTORY_H
