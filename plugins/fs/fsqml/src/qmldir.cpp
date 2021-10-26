#include "qmldir.h"
#include <QDir>
#include <QFileInfo>
#include "qmlpath.h"

#include "live/viewengine.h"
#include "live/viewcontext.h"

#include <QQmlEngine>

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

QJSValue QmlDir::listDetail(QJSValue path, QJSValue){
    QJSValue result;

    if (!path.isString())
        return result;

    if (!QFileInfo(path.toString()).isDir())
        return result;

    QFileInfoList finfos = QDir(path.toString()).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::Name | QDir::DirsFirst);

    ViewEngine* ve = engine();
    if ( !ve )
        return result;

    result = ve->engine()->newArray(finfos.size());

    for ( int i = 0; i < finfos.size(); ++i ){
        QFileInfo& finfo = finfos[i];

        QJSValue rinfo = ve->engine()->newObject();
        rinfo.setProperty("name", finfo.fileName());
        rinfo.setProperty("isDir",  finfo.isDir());
        rinfo.setProperty("size", static_cast<uint>(finfo.size()));

        result.setProperty(i, rinfo);
    }

    return result;
}

QJSValue QmlDir::detail(QJSValue path){
    if (!path.isString()) return QJSValue();

    QFileInfo finfo(path.toString());
    if ( !finfo.exists() ){
        return QJSValue();
    }

    QJSValue result;

    ViewEngine* ve = engine();
    if ( !ve )
        return result;

    result = ve->engine()->newObject();

    result.setProperty("name", finfo.fileName());
    result.setProperty("path", finfo.filePath());
    result.setProperty("isDir",  finfo.isDir());
    result.setProperty("size", static_cast<uint>(finfo.size()));

    return result;
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

ViewEngine *QmlDir::engine(){
    lv::ViewEngine* ve = lv::ViewEngine::grabFromQmlEngine(qobject_cast<QQmlEngine*>(parent()));
    if ( !ve )
        lv::QmlError::warnNoEngineCaptured(this, "Dir");
    return ve;
}

}
