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

#include "live/settings.h"
#include <QVariant>
#include <QDir>
#include <QQmlEngine>

namespace lv{

Settings::Settings(const QString &path, QObject *parent)
    : QObject(parent)
    , m_path(path)
    , m_configFiles(new QMap<QString, QObject*>)
{
}

Settings::~Settings(){
    for ( auto it = m_configFiles->begin(); it != m_configFiles->end(); ++it ){
        delete it.value();
    }
    delete m_configFiles;
}

Settings *Settings::create(const QString &path, QObject *parent){
    if ( !QDir(path).exists() ){
        if ( !QDir().mkdir(path) ){
            qWarning("Failed to create configuration directory \'config\'\n");
        }
    }
    return new Settings(path, parent);
}

QObject *Settings::file(const QString &key){
    auto foundit = m_configFiles->find(key);
    if ( foundit == m_configFiles->end() )
        return 0;
    qmlEngine(this)->setObjectOwnership(*foundit, QQmlEngine::CppOwnership);
    return *foundit;
}

void Settings::addConfigFile(const QString &key, QObject *object){
    m_configFiles->insert(key, object);
}

}// namespace
