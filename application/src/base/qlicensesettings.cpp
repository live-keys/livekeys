#include "qlicensesettings.h"
#include "qlicensecontainer.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace lcv{

QLicenseSettings::QLicenseSettings(const QString &licenseFile)
    : m_licenseFile(licenseFile)
{
    reparse();
}

QLicenseSettings::~QLicenseSettings(){
    save();
    delete m_container;
}

void QLicenseSettings::reparse(){
    QFile file(m_licenseFile);
    if ( file.open(QIODevice::ReadOnly) ){
        QJsonDocument jsondoc = QJsonDocument::fromJson(file.readAll());
        QJsonArray root = jsondoc.array();
        //TODO: Iterate array
        file.close();
    }
}

void QLicenseSettings::save(){
    QFile file(m_licenseFile);
    if ( file.open(QIODevice::WriteOnly) ){
        QJsonArray root;
        //TODO
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
        file.close();
    } else {
        qCritical("Failed to save license settings to to file: \'%s\'. Make sure the "
                  "path exists and you have set the correct access rights.", qPrintable(m_licenseFile));
    }
}


}// namespace
