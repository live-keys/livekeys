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

#include "qlivecvsettings.h"
#include "qeditorsettings.h"
#include "qlicensesettings.h"
#include <QDir>

namespace lcv{

QLiveCVSettings::QLiveCVSettings(const QString &path, QObject *parent)
    : QObject(parent)
    , m_path(path)
    , m_editor(0)
    , m_license(0)
{
    m_editor  = new QEditorSettings(path + "/editor.json");
    m_license = new QLicenseSettings(path + "/licenses.json");
}

QLiveCVSettings::~QLiveCVSettings(){
    delete m_editor;
    delete m_license;
}

QLiveCVSettings *QLiveCVSettings::initialize(const QString &path, QObject *parent){
    if ( !QDir(path).exists() ){
        if ( !QDir().mkdir(path) ){
            qWarning("Failed to create configuration directory \'config\'\n");
        }
    }
    return new QLiveCVSettings(path, parent);
}

QLicenseContainer* QLiveCVSettings::license(){
    return m_license->container();
}

}// namespace
