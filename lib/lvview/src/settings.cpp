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

#include "live/settings.h"
#include <QVariant>
#include <QDir>
#include <QQmlEngine>

#include <QtDebug>
#include "live/viewcontext.h"

/**
 * \class lv::Settings
 * \brief General settings (on top of Editor settings) that uses a settings folder
 * \ingroup lvview
 */
namespace lv{

Settings::Settings(const QString &path, QObject *parent)
    : QObject(parent)
    , m_path(path)
    , m_configFiles(new QMap<QString, QObject*>)
{
}

/** Default destructor */
Settings::~Settings(){
    for ( auto it = m_configFiles->begin(); it != m_configFiles->end(); ++it ){
        delete it.value();
    }
    delete m_configFiles;
}

/** Creator function that creates the settings singleton given the path */
Settings *Settings::create(const QString &path, QObject *parent){
    if ( !QDir(path).exists() ){
        if ( !QDir().mkdir(path) ){
            qWarning("Failed to create configuration directory \'config\'\n");
        }
    }
    return new Settings(path, parent);
}

/** Returns the file under the given key */
QObject *Settings::file(const QString &key){
    auto foundit = m_configFiles->find(key);
    if ( foundit == m_configFiles->end() )
        return nullptr;
    qmlEngine(this)->setObjectOwnership(*foundit, QQmlEngine::CppOwnership);
    return *foundit;
}

/** Adds a config file with a given key */
void Settings::addConfigFile(const QString &key, QObject *object){
    m_configFiles->insert(key, object);
}

}// namespace
