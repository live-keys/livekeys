/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef LVMLNODETOJS_H
#define LVMLNODETOJS_H

#include "live/mlnode.h"

class QJSValue;
class QJSEngine;

namespace lv{
namespace ml{

void LV_BASE_EXPORT toJs(const MLNode& n, QJSValue& result, QJSEngine* engine);
void LV_BASE_EXPORT fromJs(const QJSValue& value, MLNode& n);

}// namespace ml

}// namepsace

class MLNodeToJs
{
public:
    MLNodeToJs();
};

#endif // LVMLNODETOJS_H
