/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVMLNODETOJSON_H
#define LVMLNODETOJSON_H

#include "live/mlnode.h"

class QJsonValue;

namespace lv{

namespace ml{

void LV_BASE_EXPORT toJson(const MLNode& n, std::string& result);
void LV_BASE_EXPORT fromJson(const std::string& data, MLNode& n);
void LV_BASE_EXPORT fromJson(const char* data, MLNode& n);

//void LV_BASE_EXPORT toJson(const MLNode& n, QJsonValue& result);
//void LV_BASE_EXPORT toJson(const MLNode& n, QByteArray& result);

//void LV_BASE_EXPORT fromJson(const QJsonValue& value, MLNode& n);
//void LV_BASE_EXPORT fromJson(const QByteArray& data,  MLNode& n);

}// namespace ml

}// namespace

#endif // LVMLNODETOJSON_H
