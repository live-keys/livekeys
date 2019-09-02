#include "dir.h"
#include <QDir>
#include <QFileInfo>
#include "path.h"
#include <QDebug>

namespace lv {


Dir::Dir(QObject *parent) : QObject(parent)
{

}

QStringList Dir::list(QJSValue path)
{
    QStringList result;
    if (!path.isString()) return result;

    if (!QFileInfo(path.toString()).isDir())
        return result;

    return QDir(path.toString()).entryList(QDir::NoFilter, QDir::Name | QDir::DirsFirst);
}

bool Dir::mkDir(QJSValue path)
{
    if (!path.isString()) return false;
    return QDir(".").mkdir(path.toString());
}

bool Dir::mkPath(QJSValue path)
{
    if (!path.isString()) return false;
    return QDir(".").mkpath(path.toString());
}

bool Dir::remove(QJSValue path)
{
    if (!path.isString()) return false;
    return QDir(path.toString()).removeRecursively();
}

bool Dir::rename(QJSValue old, QJSValue nu)
{
    if (!old.isString() || !nu.isString()) return false;
    return QDir(".").rename(old.toString(), nu.toString());
}

}
