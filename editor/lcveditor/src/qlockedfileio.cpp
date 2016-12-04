#include "qlockedfileio.h"
#include <QFile>
#include <QTextStream>

namespace lcv{

QLockedFileIO::QLockedFileIO(){

}

QLockedFileIO::~QLockedFileIO(){

}

QLockedFileIO::MutablePtr QLockedFileIO::createInstance(){
    return QLockedFileIO::MutablePtr(new QLockedFileIO);
}

QString QLockedFileIO::readFromFile(const QString &path){
    //TODO: Make thread safe
    QFile fileInput(path);
    if ( !fileInput.open(QIODevice::ReadOnly ) ){
        qCritical("Cannot open file: %s", qPrintable(path));
        return "";
    }

    QTextStream in(&fileInput);
    QString content = in.readAll();
    fileInput.close();
    return content;
}

bool QLockedFileIO::writeToFile(const QString &path, const QString &data){
    //TODO: Make thread safe
    QFile fileInput(path);
    if ( !fileInput.open(QIODevice::WriteOnly ) ){
        qCritical("Can't open file for writing: %s", qPrintable(path));
        return false;
    }

    QTextStream stream(&fileInput);
    stream << data;
    stream.flush();
    fileInput.close();
    return true;
}

}// namespace
