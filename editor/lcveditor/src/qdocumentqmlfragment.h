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

#ifndef QDOCUMENTQMLFRAGMENT_H
#define QDOCUMENTQMLFRAGMENT_H

#include "qlcveditorglobal.h"
#include "qdocumenteditfragment.h"

#include <QQmlProperty>
#include <QQuickItem>

namespace lcv{

class Q_LCVEDITOR_EXPORT QDocumentQmlFragment : public QDocumentEditFragment{

public:
    QDocumentQmlFragment(
        QCodeDeclaration::Ptr declaration,
        QCodeConverter* converter,
        const QQmlProperty& property
    );
    ~QDocumentQmlFragment();

    void commit(const QVariant &value) Q_DECL_OVERRIDE;
    const QQmlProperty& property() const;

private:
    QQmlProperty m_property;
};

inline void QDocumentQmlFragment::commit(const QVariant &value){
    m_property.write(value);
}

inline const QQmlProperty &QDocumentQmlFragment::property() const{
    return m_property;
}

}// namespace

#endif // QDOCUMENTQMLFRAGMENT_H
