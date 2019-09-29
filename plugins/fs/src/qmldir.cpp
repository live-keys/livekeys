#include "qmldir.h"
#include <QDir>
#include <QFileInfo>
#include "qmlpath.h"
#include <QDebug>

namespace lv {


QmlDir::QmlDir(QObject *parent)
    : QObject(parent)
{
}

QStringList QmlDir::list(QJSValue path){
    QStringList result;
    if (!path.isString())
        return result;

    if (!QFileInfo(path.toString()).isDir())
        return result;

    return QDir(path.toString()).entryList(QDir::NoFilter, QDir::Name | QDir::DirsFirst);
}

bool QmlDir::mkDir(QJSValue path)
{
    if (!path.isString()) return false;
    return QDir(".").mkdir(path.toString());
}

bool QmlDir::mkPath(QJSValue path)
{
    if (!path.isString()) return false;
    return QDir(".").mkpath(path.toString());
}

bool QmlDir::remove(QJSValue path)
{
    if (!path.isString()) return false;
    return QDir(path.toString()).removeRecursively();
}

bool QmlDir::rename(QJSValue old, QJSValue nu)
{
    if (!old.isString() || !nu.isString()) return false;
    return QDir(".").rename(old.toString(), nu.toString());
}

}
