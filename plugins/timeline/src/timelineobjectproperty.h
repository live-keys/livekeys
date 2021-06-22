#ifndef LVTIMELINEOBJECTPROPERTY_H
#define LVTIMELINEOBJECTPROPERTY_H

#include "live/viewengine.h"
#include "live/mlnode.h"
#include "live/lvtimelineglobal.h"

namespace lv{

class LV_TIMELINE_EXPORT TimelineObjectProperty : public QObject{

    Q_OBJECT

public:
    TimelineObjectProperty(QObject* parent = nullptr);
    virtual ~TimelineObjectProperty();

    virtual void serialize(ViewEngine* engine, MLNode& node) = 0;
    virtual void deserialize(ViewEngine* engine, const MLNode& node) = 0;
};

}// namespace

#endif // LVTIMELINEOBJECTPROPERTY_H
