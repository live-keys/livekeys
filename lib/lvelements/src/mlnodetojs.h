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

#ifndef MLNODETOJS_H
#define MLNODETOJS_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/value.h"
#include "live/mlnode.h"

namespace lv{

namespace ml{

void LV_ELEMENTS_EXPORT toJs(const MLNode& n, el::ScopedValue& v, el::Engine* engine);
void LV_ELEMENTS_EXPORT fromJs(const el::ScopedValue& v, MLNode& n, el::Engine *engine);

} // namespace ml
} // namespace


#endif // MLNODETOJS_H
