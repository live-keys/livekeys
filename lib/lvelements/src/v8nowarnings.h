#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)

#pragma warning(push)
#pragma warning(disable : 4100)

#include "v8.h"
#include "libplatform/libplatform.h"

#pragma warning(pop)

#elif defined(__clang__)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

#include "v8.h"
#include "libplatform/libplatform.h"

#pragma clang diagnostic pop

#else

#include "v8.h"
#include "libplatform/libplatform.h"

#endif
