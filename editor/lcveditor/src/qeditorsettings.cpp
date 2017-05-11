#include "qeditorsettings.h"
#include "qprojectdocument.h"
#include "qprojectfile.h"
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

namespace lcv{

QEditorSettings::QEditorSettings(const QString &path, QObject *parent)
    : QObject(parent)
    , m_fontSize(12)
    , m_path(path)
{
    syncWithFile();
}

QEditorSettings::~QEditorSettings(){
}

void QEditorSettings::fromJson(const QJsonObject &root){
    if ( root.contains("fontSize") ){
        m_fontSize = root["fontSize"].toInt();
        emit fontSizeChanged(m_fontSize);
    }
}

QJsonObject QEditorSettings::toJson() const{
    QJsonObject root;
    root["fontSize"] = m_fontSize;
    return root;
}

void QEditorSettings::syncWithFile(){
    QFile file(m_path);
    if ( !file.exists() ){
        m_content = QJsonDocument(toJson()).toJson(QJsonDocument::Indented);
        if ( file.open(QIODevice::WriteOnly) ){
            file.write(m_content);
            file.close();
        } else {
            qCritical("Failed to open settings file for writting: %s", qPrintable(m_path));
        }
    } else if ( file.open(QIODevice::ReadOnly) ){
        init(file.readAll());
        file.close();
    }
}

void QEditorSettings::init(const QByteArray &data){
    m_content = data;
    QJsonParseError error;
    QJsonDocument jsondoc = QJsonDocument::fromJson(data, &error);
    if ( error.error != QJsonParseError::NoError ){
        emit initError(error.errorString());
        qCritical("Failed to parse settings file: %s", qPrintable(error.errorString()));
    }
    fromJson(jsondoc.object());
}

void QEditorSettings::documentOpened(lcv::QProjectDocument *document){
    connect(document, SIGNAL(isDirtyChanged()), this, SLOT(documentIsDirtyChanged()));
}

void QEditorSettings::documentIsDirtyChanged(){
    QProjectDocument* document = qobject_cast<QProjectDocument*>(sender());
    if ( document ){
        if ( !document->isDirty() && document->file()->path() == m_path ){
            init(document->content().toUtf8());
        }
    }
}

}// namespace
