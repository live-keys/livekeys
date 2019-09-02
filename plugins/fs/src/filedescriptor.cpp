#include "filedescriptor.h"
#include "qfile.h"
#include <QTextStream>

namespace lv {

FileDescriptor::FileDescriptor(QFile* f, QObject *parent) : QObject(parent), file(f)
{

}

FileDescriptor::FileDescriptor(const FileDescriptor &other)
{
    this->file = other.file;
    this->setParent(other.parent());
}

void FileDescriptor::operator=(const FileDescriptor &other)
{
    this->file = other.file;
    this->setParent(other.parent());
}

FileDescriptor::~FileDescriptor()
{
    if (!file) return;

    if (file->isOpen()) file->close();

    delete file;
    file = nullptr;
}

bool FileDescriptor::isValid()
{
    return file && file->isOpen();
}

void FileDescriptor::close()
{
    if (!file) return;

    file->close();
    delete file;
    file = nullptr;
}

bool FileDescriptor::seek(int pos)
{
    if (!file) return false;

    return file->seek(pos);
}

qint64 FileDescriptor::write(QByteArray content)
{
    if (!file) return 0;
    return file->write(content);
}

qint64 FileDescriptor::writeString(QString text)
{
    if (!file || !file->isTextModeEnabled()) return 0;

    int pos = file->pos();
    QTextStream stream(file);
    stream.seek(pos);
    stream << text;

    return text.length();
}

QByteArray FileDescriptor::read(qint64 numOfBytes)
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
