#ifndef LVCOMPLETIONCONTEXT_H
#define LVCOMPLETIONCONTEXT_H

#include "live/elements/lvelementsglobal.h"
#include "live/elements/sourcerange.h"
#include "live/utf8.h"

#include <vector>

namespace lv{ namespace el{

class LV_ELEMENTS_EXPORT CursorContext{

public:
    enum Context{
        InImport = 1,
        InElements = 2,
        InLeftOfDeclaration = 4,
        InRightOfDeclaration = 8,
        InStringLiteral = 16
    };

public:
    CursorContext(
        int context,
        const std::vector<SourceRange>& expressionPath,
        const std::vector<SourceRange> &propertyPath = std::vector<SourceRange>(),
        const SourceRange& propertyDeclaredType = SourceRange(),
        const SourceRange& objectType = SourceRange(),
        const SourceRange& objectImportNamespace = SourceRange()
    );
    ~CursorContext();

    int context() const;
    Utf8 contextString() const;

    const std::vector<SourceRange>& expressionPath() const;
    const std::vector<SourceRange>& propertyPath() const;
    SourceRange propertyName() const;
    const SourceRange& propertyRange() const;
    const SourceRange& propertyDeclaredType() const;
    const SourceRange& objectType() const;
    const SourceRange& objectImportNamespace() const;

public:
    int                      m_context;
    std::vector<SourceRange> m_expressionPath;
    std::vector<SourceRange> m_propertyPath;
    SourceRange              m_propertyRange;
    SourceRange              m_propertyDeclaredType;
    SourceRange              m_objectType;
    SourceRange              m_objectImportNamespace;
};

inline const std::vector<SourceRange> &CursorContext::expressionPath() const{
    return m_expressionPath;
}

inline const std::vector<SourceRange> &CursorContext::propertyPath() const{
    return m_propertyPath;
}

inline SourceRange CursorContext::propertyName() const{
    if ( m_propertyPath.size() > 0 )
        return m_propertyPath.back();
    return SourceRange();
}

inline const SourceRange &CursorContext::propertyRange() const{
    return m_propertyRange;
}

inline const SourceRange &CursorContext::propertyDeclaredType() const{
    return m_propertyDeclaredType;
}

inline const SourceRange &CursorContext::objectType() const{
    return m_objectType;
}

inline const SourceRange &CursorContext::objectImportNamespace() const{
    return m_objectImportNamespace;
}

}} // namespace lv, el

#endif // LVCOMPLETIONCONTEXT_H
