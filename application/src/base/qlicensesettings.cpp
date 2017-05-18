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
    delete m_container;
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
