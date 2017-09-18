#ifndef LVMLNODETOJSON_H
#define LVMLNODETOJSON_H

#include "live/mlnode.h"

class QJsonValue;

namespace lcv{

namespace ml{

void LVBASE_EXPORT toJson(const MLNode& n, QJsonValue& result);
void LVBASE_EXPORT toJson(const MLNode& n, QByteArray& result);

void LVBASE_EXPORT fromJson(const QJsonValue& value, MLNode& n);
void LVBASE_EXPORT fromJson(const QByteArray& data,  MLNode& n);

} // namespace ml

}// namespace

#endif // LVMLNODETOJSON_H
