#include <tree_sitter/parser.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define TSLANGUAGE_EXPORT __declspec(dllexport)
#else
#define TSLANGUAGE_EXPORT
#endif

TSLANGUAGE_EXPORT const TSLanguage *tree_sitter_elements(void);

#ifdef __cplusplus
}
#endif
