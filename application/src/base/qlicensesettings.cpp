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

#include "qlicensesettings.h"
//#include "qlicensecontainer.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace lcv{

QLicenseSettings::QLicenseSettings(const QString &licenseFile)
//    : m_container(new QLicenseContainer)
    : m_container(0)
    , m_licenseFile(licenseFile)
    , m_parseError(false)
{
    reparse();
}

QLicenseSettings::~QLicenseSettings(){
    save();
//    delete m_container;
}

void QLicenseSettings::reparse(){
//    QFile file(m_licenseFile);
//    if ( file.open(QIODevice::ReadOnly) ){
//        QJsonParseError error;
//        QJsonDocument jsondoc = QJsonDocument::fromJson(file.readAll(), &error);
//        if ( error.error != QJsonParseError::NoError ){
//            m_parseError = true;
//            m_errorText  = error.errorString();
//            qCritical(
//                "Failed to parse stored licenses file(config/licenses.json): %s. Licenses will not be saved.",
//                qPrintable(m_errorText)
//            );
//        }
//        m_container->fromJson(jsondoc.array());
//        file.close();
//    }
}

void QLicenseSettings::save(){
//    if ( m_parseError || !m_container->isDirty() )
//        return;

//    QFile file(m_licenseFile);
//    if ( file.open(QIODevice::WriteOnly) ){
//        QJsonArray root = m_container->toJson();
//        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
//        file.close();
//    } else {
//        qCritical("Failed to save license settings to to file: \'%s\'. Make sure the "
//                  "path exists and you have set the correct access rights.", qPrintable(m_licenseFile));
//    }
}


}// namespace
