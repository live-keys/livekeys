#include "qmlfiledescriptor.h"
#include "qfile.h"
#include <QTextStream>
#include <QDebug>

namespace lv {

QmlFileDescriptor::QmlFileDescriptor(QFile* f, QObject *parent) : QObject(parent), file(f)
{

}

QmlFileDescriptor::QmlFileDescriptor(const QmlFileDescriptor &other)
{
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
    if (!file) return;

    if (file->isOpen()) file->close();

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
    if (!file) return false;

    return file->seek(pos);
}

qint64 QmlFileDescriptor::write(QByteArray content)
{
    if (!file) return 0;
    return file->write(content);
}

qint64 QmlFileDescriptor::writeString(QString text)
{
    if (!file || !file->isTextModeEnabled()) return 0;

    int pos = file->pos();
    QTextStream stream(file);
    stream.seek(pos);
    stream << text;

    return text.length();
}

QByteArray QmlFileDescriptor::read(qint64 numOfBytes)
{
    if (!file) return QByteArray();

    if (numOfBytes == -1) return file->readAll();

    char* data = new char[numOfBytes];

    qint64 num = file->read(data, numOfBytes);
    if (num == -1) return QByteArray();

    QByteArray result(data, num);
    return result;
}

}
