#ifndef LVTUPLEPROPERTYMAP_H
#define LVTUPLEPROPERTYMAP_H

#include "tuple.h"
#include <QQmlPropertyMap>

namespace lv{

class TuplePropertyMap{

public:
    static bool reserveForRead(const QQmlPropertyMap& t, Filter::SharedDataLocker* locker, Filter* filter);
    static bool reserveForWrite(const QQmlPropertyMap& t, Filter::SharedDataLocker* locker, Filter* filter);

    static void serialize(lv::Engine* engine, const QQmlPropertyMap& t, MLNode& node);
    static void deserialize(lv::Engine* engine, const MLNode& node, QQmlPropertyMap& t);
    static void deserialize(lv::Engine* engine, const MLNode& node, QVariant& v);
};

} // namespace

#endif // LVTUPLEPROPERTYMAP_H
