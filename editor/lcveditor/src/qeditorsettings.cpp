#include "qeditorsettings.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

namespace lcv{

QEditorSettings::QEditorSettings(const QString &path, QObject *parent)
    : QObject(parent)
    , m_path(path)
    , m_fontSize(13)
{
    reparse();
}

QEditorSettings::~QEditorSettings(){
    save();
}

void QEditorSettings::reparse(){
    QFile file(m_path);
    if ( !file.exists() ){
        QJsonObject root;
        root["fontSize"] = m_fontSize;
        m_content = QJsonDocument(root).toJson(QJsonDocument::Indented);
    } else if ( file.open(QIODevice::ReadOnly) ){
        init(file.readAll());
        file.close();
    }
}

void QEditorSettings::save(){
    QFile file(m_path);
    if ( file.open(QIODevice::WriteOnly) ){
        file.write(m_content);
        file.close();
    } else {
        qCritical("Failed to save settings to file");
    }
}

void QEditorSettings::init(const QByteArray &data){
    m_content = data;
    QJsonDocument jsondoc = QJsonDocument::fromJson(data);
    QJsonObject root = jsondoc.object();
    if ( root.contains("fontSize") ){
        m_fontSize = root["fontSize"].toInt();
        emit fontSizeChanged(m_fontSize);
    }
}

}// namespace
