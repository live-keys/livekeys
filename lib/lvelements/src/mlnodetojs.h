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
