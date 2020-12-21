#include "qmlfiledescriptor.h"
#include "qfile.h"
#include <QTextStream>
#include <QDebug>
#include "live/exception.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"

namespace lv {

QmlFileDescriptor::QmlFileDescriptor(QFile* f, QObject *parent)
    : QObject(parent), file(f)
{
}

QmlFileDescriptor::QmlFileDescriptor(const QmlFileDescriptor &other){
    this->file = other.file;
    this->setParent(other.parent());
}

void QmlFileDescriptor::operator=(const QmlFileDescriptor &other)
{
    this->file = other.file;
    this->setParent(other.parent());
}

QmlFileDescriptor::~QmlFileDescriptor()
{
    if (!file)
        return;

    if (file->isOpen())
        file->close();

    delete file;
    file = nullptr;
}

bool QmlFileDescriptor::isValid()
{
    return file && file->isOpen();
}

void QmlFileDescriptor::close()
{
    if (!file) return;

    file->close();
    delete file;
    file = nullptr;
}

bool QmlFileDescriptor::seek(int pos)
{
    if ( !file ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "No file opened.", Exception::toCode("~File"));
        lv::ViewContext::instance().engine()->throwError(&e);
        return false;
    }
    return file->seek(pos);
}

bool QmlFileDescriptor::isEof() const{
    return file->atEnd();
}

qint64 QmlFileDescriptor::write(QByteArray content)
{
    if ( !file ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "No file opened.", Exception::toCode("~File"));
        lv::ViewContext::instance().engine()->throwError(&e);
        return 0;
    }
    return file->write(content);
}

qint64 QmlFileDescriptor::writeString(QString text){
    if ( !file ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "No text file opened.", Exception::toCode("~File"));
        lv::ViewContext::instance().engine()->throwError(&e);
        return 0;
    }

    return file->write(text.toUtf8());
}

QByteArray QmlFileDescriptor::read(qint64 numOfBytes){
    if ( !file ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "No file opened.", Exception::toCode("~File"));
        lv::ViewContext::instance().engine()->throwError(&e);
        return QByteArray();
    }

    return file->read(numOfBytes);
}

QByteArray QmlFileDescriptor::readAll() const{
    if ( !file ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "No file opened.", Exception::toCode("~File"));
        lv::ViewContext::instance().engine()->throwError(&e);
        return QByteArray();
    }

    return file->readAll();
}

QByteArray QmlFileDescriptor::readLine(){
    if ( !file ){
        lv::Exception e = CREATE_EXCEPTION(lv::Exception, "No file opened.", Exception::toCode("~File"));
        lv::ViewContext::instance().engine()->throwError(&e);
        return QByteArray();
    }

    return file->readLine();
}

}
