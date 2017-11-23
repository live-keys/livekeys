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
    typedef QSharedPointer<DocumentQmlScope> Ptr;

    class Import{

    public:
        static const int NoVersion;

        enum Type{
            Invalid,
            Library,
            Directory,
            ImplicitDirectory,
            File,
            UnknownFile
        };

        Import(
            Type importType,
            const QString& path,
            const QString& as = "",
            int vMajor = NoVersion,
            int vMinor = NoVersion
        );


        Type importType() const;
        void setImportType(Type importType);

        const QString& path() const;
        const QString& as() const;
        int versionMajor() const;
        int versionMinor() const;
        bool isVersionValid() const;

        bool operator ==(const Import& other) const;

    private:
        Type    m_type;
        QString m_path;
        QString m_as;
        int     m_versionMajor;
        int     m_versionMinor;
    };

    typedef QPair<Import, QString> ImportEntry;
    typedef QList<ImportEntry> ImportList;

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

    static QList<Import> extractImports(DocumentQmlInfo::Ptr document);

    bool hasImport(const Import& key);
    int totalImports() const;
    const ImportList& imports() const;

    void addImport(const Import& key, const QString& path);

    DocumentQmlInfo::ConstPtr info() const;
    ProjectQmlScope::Ptr projectScope() const;

    QString path() const;
    QString componentName() const;

private:
    ImportList                   m_imports;
    ProjectQmlScope::Ptr        m_projectScope;
    DocumentQmlInfo::Ptr m_documentInfo;
};

inline int DocumentQmlScope::Import::versionMajor() const{
    return m_versionMajor;
}

inline int DocumentQmlScope::Import::versionMinor() const{
    return m_versionMinor;
}

inline bool DocumentQmlScope::Import::isVersionValid() const{
    return m_versionMajor >= 0 && m_versionMinor >= 0;
}

inline bool DocumentQmlScope::Import::operator ==(const DocumentQmlScope::Import &other) const{
    if ( path() != other.path() )
        return false;
    if ( versionMajor() != other.versionMajor() )
        return false;
    return versionMinor() == other.versionMinor();
}

inline const QString &DocumentQmlScope::Import::path() const{
    return m_path;
}

inline void DocumentQmlScope::Import::setImportType(DocumentQmlScope::Import::Type importType){
    m_type = importType;
}

inline DocumentQmlScope::Import::Type DocumentQmlScope::Import::importType() const{
    return m_type;
}

inline const QString &DocumentQmlScope::Import::as() const{
    return m_as;
}

inline int DocumentQmlScope::totalImports() const{
    return m_imports.size();
}

inline DocumentQmlInfo::ConstPtr DocumentQmlScope::info() const{
    return m_documentInfo;
}

inline const DocumentQmlScope::ImportList &DocumentQmlScope::imports() const{
    return m_imports;
}

inline ProjectQmlScope::Ptr DocumentQmlScope::projectScope() const{
    return m_projectScope;
}

inline QString DocumentQmlScope::path() const{
    return m_documentInfo->path();
}

inline QString DocumentQmlScope::componentName() const{
    return m_documentInfo->componentName();
}

}// namespace

#endif // LVDOCUMENTQMLSCOPE_H
