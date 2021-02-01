#include "fileio.h"

#include <QQmlEngine>
#include <QFile>

FileIO::FileIO(QObject *parent)
    : QObject(parent)
    , m_engine(nullptr)
{
    m_engine = qobject_cast<QQmlEngine*>(parent);
}

void FileIO::setEngine(QQmlEngine *engine){
    m_engine = engine;
}

QObject* FileIO::provider(QQmlEngine *engine, QJSEngine *){
    QObject* ob = new FileIO(engine);
    QQmlEngine::setObjectOwnership(ob, QQmlEngine::CppOwnership);
    return ob;
}

QByteArray FileIO::readUrl(const QUrl &url){
    return read(url.toLocalFile());
}

QByteArray FileIO::read(const QString &path){
    if ( !m_engine ){
        qWarning("Engine not set in FileIO. This object is disabled.");
        return "";
    }

    QFile file(path);
    if ( !file.exists() ){
        m_engine->throwError("File does not exist: " + path);
        return "";
    }
    if ( !file.open(QIODevice::ReadOnly) ){
        m_engine->throwError("Failed to open file for reading: " + path);
        return "";
    }

    return file.readAll();

}

void FileIO::write(const QString &path, const QString &data){
    if ( !m_engine ){
        qWarning("Engine not set in FileIO. This object is disabled.");
        return;
    }

    QFile file(path);
    if ( !file.open(QIODevice::WriteOnly) ){
        m_engine->throwError("Failed to open file for writing: " + path);
        return;
    }

    file.write(data.toUtf8());
}

void FileIO::writeUrl(const QUrl &path, const QString &data){
    write(path.toLocalFile(), data);
}


