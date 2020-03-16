/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef LVDOCUMENTQMLSCOPE_H
#define LVDOCUMENTQMLSCOPE_H

#include "live/lveditqmljsglobal.h"
#include "live/projectqmlscope.h"
#include "live/documentqmlinfo.h"

#include <QString>
#include <QSharedPointer>

namespace lv{

class LV_EDITQMLJS_EXPORT DocumentQmlScope{

public:
    /// \brief SharedPointer to q DocumentQmlScope
    typedef QSharedPointer<DocumentQmlScope> Ptr;

private:
    DocumentQmlScope(ProjectQmlScope::Ptr projectScope, DocumentQmlInfo::Ptr documentInfo);

public:
    ~DocumentQmlScope();

    static DocumentQmlScope::Ptr createEmptyScope(
        ProjectQmlScope::Ptr projectScope
    );
    static DocumentQmlScope::Ptr createScope(
        const QString& fileName,
        const QString& data,
        ProjectQmlScope::Ptr projectScope
    );

    DocumentQmlInfo::ConstPtr info() const;
    DocumentQmlInfo::Ptr info();
    ProjectQmlScope::Ptr projectScope() const;

private:
    ProjectQmlScope::Ptr m_projectScope;
    DocumentQmlInfo::Ptr m_documentInfo;
};

/// \brief Returns the lv::DocumentQmlInfo associated with this object.
inline DocumentQmlInfo::ConstPtr DocumentQmlScope::info() const{
    return m_documentInfo;
}

/// \brief Returns the lv::DocumentQmlInfo associated with this object.
inline DocumentQmlInfo::Ptr DocumentQmlScope::info(){
    return m_documentInfo;
}

/// \brief Returns the project scope.
inline ProjectQmlScope::Ptr DocumentQmlScope::projectScope() const{
    return m_projectScope;
}

}// namespace

#endif // LVDOCUMENTQMLSCOPE_H
