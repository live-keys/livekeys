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

#ifndef LVDOCUMENTQMLFRAGMENT_H
#define LVDOCUMENTQMLFRAGMENT_H

#include "live/lveditorglobal.h"
#include "live/documenteditfragment.h"

#include <QQmlProperty>
#include <QQuickItem>

namespace lv{

class LV_EDITOR_EXPORT DocumentQmlFragment : public DocumentEditFragment{

public:
    DocumentQmlFragment(
        CodeDeclaration::Ptr declaration,
        CodeConverter* converter,
        const QQmlProperty& property
    );
    ~DocumentQmlFragment();

    void commit(const QVariant &value) Q_DECL_OVERRIDE;
    const QQmlProperty& property() const;

private:
    QQmlProperty m_property;
};

inline void DocumentQmlFragment::commit(const QVariant &value){
    m_property.write(value);
}

inline const QQmlProperty &DocumentQmlFragment::property() const{
    return m_property;
}

}// namespace

#endif // LVDOCUMENTQMLFRAGMENT_H
