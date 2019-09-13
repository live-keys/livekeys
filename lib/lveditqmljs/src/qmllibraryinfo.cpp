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

#include "qmllibraryinfo_p.h"

namespace lv{

QmlLibraryInfo::QmlLibraryInfo()
    : m_status(QmlLibraryInfo::NotScanned)
    , m_importVersionMajor(0)
    , m_importVersionMinor(0)
{
}

QmlLibraryInfo::QmlLibraryInfo(const QmlDirParser &parser)
    : m_status(QmlLibraryInfo::NotScanned)
    , m_data(parser)
    , m_importVersionMajor(0)
    , m_importVersionMinor(0)
{
    m_importNamespace = parser.typeNamespace();
}

QmlLibraryInfo::~QmlLibraryInfo(){
}

void QmlLibraryInfo::updateExports(){
    m_exports.clear();
    foreach( const LanguageUtils::FakeMetaObject::ConstPtr& obj, m_data.metaObjects()){
        foreach( const LanguageUtils::FakeMetaObject::Export& e, obj->exports() ){
            if ( e.version.isValid() ){
                m_exports[e.type].versions.append(ExportVersion(e.version.majorVersion(), e.version.minorVersion(), obj));
                if ( e.version.majorVersion() > m_importVersionMajor ||
                     ( e.version.majorVersion() == m_importVersionMajor && e.version.minorVersion() > m_importVersionMinor))
                {
                    m_importVersionMajor = e.version.majorVersion();
                    m_importVersionMinor = e.version.minorVersion();
                }
            }
        }
    }
}

void QmlLibraryInfo::listExports(QStringList *exports){
    for ( QMap<QString, QmlLibraryInfo::Export>::ConstIterator it = m_exports.begin(); it != m_exports.end(); ++it ){
        exports->append(it.key());
    }
}

QmlLibraryInfo::ExportVersion QmlLibraryInfo::findExport(const QString &type){
    QmlLibraryInfo::ExportVersion base;
    if ( m_exports.contains(type) ){
        QmlLibraryInfo::Export e = m_exports.value(type);
        foreach( QmlLibraryInfo::ExportVersion ev, e.versions ){
            if ( ev.versionMajor > base.versionMajor )
                base = ev;
            else if ( ev.versionMajor == base.versionMajor && ev.versionMinor > base.versionMinor )
                base = ev;
        }
    }
    return base;
}

QmlLibraryInfo::ExportVersion QmlLibraryInfo::findExportByClassName(const QString &name){
    QmlLibraryInfo::ExportVersion base;
    foreach( const LanguageUtils::FakeMetaObject::ConstPtr& obj, m_data.metaObjects()){
        if ( obj->className() == name ){
            foreach( const LanguageUtils::FakeMetaObject::Export& e, obj->exports() ){
                if ( e.version.isValid() ){
                    return findExport(e.type);
                }
            }
            return base;
        }
    }
    return base;
}

LanguageUtils::FakeMetaObject::ConstPtr QmlLibraryInfo::findObjectByClassName(const QString &name){
    foreach( const LanguageUtils::FakeMetaObject::ConstPtr& obj, m_data.metaObjects()){
        if ( obj->className() == name )
            return obj;
    }
    return LanguageUtils::FakeMetaObject::ConstPtr(0);
}

void QmlLibraryInfo::setDependencies(const QList<QString> &paths){
    m_dependencyPaths = paths;
}

void QmlLibraryInfo::setImportNamespace(const QString &importNamespace){
    m_importNamespace = importNamespace;
}

}// namespace
