#include "file.h"
#include "filedescriptor.h"
#include <QFile>
#include <QVariant>

namespace lv {


File::File(QObject *parent) : QObject(parent)
{

}

bool File::remove(QJSValue path)
{
    if (!path.isString()) return false;

    return QFile::remove(path.toString());
}

bool File::rename(QJSValue old, QJSValue nu)
{
    if (!old.isString() || !nu.isString()) return false;

    return QFile::rename(old.toString(), nu.toString());
}

qint64 File::size(QJSValue path)
{
    if (!path.isString()) return 0;

    return QFile(path.toString()).size();
}

lv::FileDescriptor File::open(QJSValue path, File::Flags flags)
{
    FileDescriptor result(nullptr, this);
    if (!path.isString()) return result;
    QFile* file = new QFile(path.toString());

    bool success = file->open(static_cast<QFile::OpenMode>(flags));
    if (!success) return result;

    result.setFile(file);

    return result;

}


}
