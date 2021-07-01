#ifndef LVQMLSTREAMPROVIDER_H
#define LVQMLSTREAMPROVIDER_H

#include "live/lvviewglobal.h"

namespace lv{

class LV_VIEW_EXPORT QmlStreamProvider{

public:
    QmlStreamProvider();
    virtual ~QmlStreamProvider();

    virtual void wait() = 0;
    virtual void resume() = 0;
};


}// namespace


#endif // LVQMLSTREAMPROVIDER_H
