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

#ifndef LVCOMPLETIONCONTEXT_H
#define LVCOMPLETIONCONTEXT_H

#include "live/elements/compiler/lvelcompilerglobal.h"
#include "live/elements/compiler/sourcerange.h"
#include "live/utf8.h"
#include <set>
#include <vector>

namespace lv{ namespace el{

class LV_ELEMENTS_COMPILER_EXPORT CursorContext{

public:
    enum Context{
        InImport = 1,
        InElements = 2,
        InLeftOfDeclaration = 4,
        InRightOfDeclaration = 8,
        InStringLiteral = 16,
        InRelativeImport = 32
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

    static std::set<std::string> keywords;
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
