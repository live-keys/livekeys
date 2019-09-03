#include "qmlfile.h"
#include "qmlfiledescriptor.h"
#include <QFile>
#include <QVariant>

namespace lv {


QmlFile::QmlFile(QObject *parent) : QObject(parent)
{

}

bool QmlFile::remove(QJSValue path)
{
    if (!path.isString()) return false;

    return QFile::remove(path.toString());
}

bool QmlFile::rename(QJSValue old, QJSValue nu)
{
    if (!old.isString() || !nu.isString()) return false;

    return QFile::rename(old.toString(), nu.toString());
}

qint64 QmlFile::size(QJSValue path)
{
    if (!path.isString()) return 0;

    return QFile(path.toString()).size();
}

lv::QmlFileDescriptor* QmlFile::open(QJSValue path, QmlFile::Flags flags)
{
    if (!path.isString()) return nullptr;
    QFile* file = new QFile(path.toString());

    bool success = file->open(static_cast<QFile::OpenMode>(flags));
    if (!success) return nullptr;

    QmlFileDescriptor *fd = new QmlFileDescriptor(file, this);
    return fd;

}


}
