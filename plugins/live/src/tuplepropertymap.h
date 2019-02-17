#ifndef LVTUPLEPROPERTYMAP_H
#define LVTUPLEPROPERTYMAP_H

#include "tuple.h"
#include <QQmlPropertyMap>

namespace lv{

/// \private
class TuplePropertyMap{

public:
    static bool reserveForRead(const QQmlPropertyMap& t, Shared::ReadScope* locker, Filter* filter);
    static bool reserveForWrite(const QQmlPropertyMap& t, Shared::ReadScope* locker, Filter* filter);

    static void serialize(lv::ViewEngine* engine, const QQmlPropertyMap& t, MLNode& node);
    static void deserialize(lv::ViewEngine* engine, const MLNode& node, QQmlPropertyMap& t);
    static void deserialize(lv::ViewEngine* engine, const MLNode& node, QVariant& v);
};

} // namespace

#endif // LVTUPLEPROPERTYMAP_H
