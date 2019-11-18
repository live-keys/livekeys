#include "qmlpath.h"
#include <QFileInfo>
#include <QJSValueIterator>
#include <QDebug>
#include <QFile>

namespace lv{

QmlPath::QmlPath(QObject *parent)
    : QObject(parent)
{
}

QString QmlPath::name(const QString &path){
    return QFileInfo(path).fileName();
}

QString QmlPath::join(QJSValue part1, QJSValue part2)
{
    QString result = "";
    if (part1.isArray())
    {
        const int length = part1.property("length").toInt();
        for (int i = 0; i < length; ++i) {
            if (result != "") result += "/";
            result += removeSlashes(part1.property(i).toString());
        }
        return result;
    }

    if (part1.isString())
    {
        result += removeSlashes(part1.toString());
        if (part2.isString()) result += "/" + removeSlashes(part2.toString());
    }
    return result;
}

bool QmlPath::exists(QJSValue path)
{
    if (path.isString())
        return QFileInfo::exists(path.toString());

    return false;
}

QString QmlPath::baseName(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).baseName();

    return "";
}

QString QmlPath::suffix(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).suffix();

    return "";
}

QString QmlPath::completeSuffix(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).completeSuffix();

    return "";
}

QString QmlPath::absolutePath(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).absoluteFilePath();

    return "";
}

bool QmlPath::hasExtensions(const QString &path, QJSValue extensions){
    if ( extensions.isString() )
        return path.endsWith("." + extensions.toString());
    return false;
}

bool QmlPath::isReadable(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isReadable();

    return false;
}

bool QmlPath::isWritable(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isWritable();

    return false;
}

bool QmlPath::isExecutable(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isExecutable();

    return false;
}

bool QmlPath::isHidden(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isHidden();

    return false;
}

bool QmlPath::isNativePath(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isNativePath();

    return false;
}

bool QmlPath::isRelative(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isRelative();

    return false;
}

bool QmlPath::isAbsolute(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isAbsolute();

    return false;
}

bool QmlPath::isFile(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isFile();

    return false;
}

bool QmlPath::isDir(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isDir();

    return false;
}

bool QmlPath::isSymLink(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isSymLink();

    return false;
}

bool QmlPath::isRoot(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isRoot();

    return false;
}

bool QmlPath::isBundle(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isBundle();

    return false;
}

QDateTime QmlPath::lastModified(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).lastModified();

    return QDateTime();
}

QDateTime QmlPath::created(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).created();

    return QDateTime();
}

QString QmlPath::owner(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).owner();

    return "";
}

uint QmlPath::ownerId(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).ownerId();

    return 1;
}

QString QmlPath::group(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).group();

    return "";
}

uint QmlPath::groupId(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).groupId();

    return 1;
}

QString QmlPath::parent(QJSValue path)
{
    if (isFile(path))
    {
        QDir dir = QFileInfo(path.toString()).dir();
        dir.cdUp();
        return dir.path();
    }


    QString p = path.toString();
    if (p.length() > 0 && p[p.length()-1] == '/')
        p = p.left(p.length() - 1);
    return QFileInfo(p).dir().path();
}

QString QmlPath::followSymLink(QJSValue symLink)
{
    return QFileInfo(symLink.toString()).symLinkTarget();
}

bool QmlPath::createSymLink(QJSValue symLinkPath, QJSValue path)
{
    return QFile::link(symLinkPath.toString(), path.toString());
}

/*
    enum Permission {
        ReadOwner = 0x4000, WriteOwner = 0x2000, ExeOwner = 0x1000,
        ReadUser  = 0x0400, WriteUser  = 0x0200, ExeUser  = 0x0100,
        ReadGroup = 0x0040, WriteGroup = 0x0020, ExeGroup = 0x0010,
        ReadOther = 0x0004, WriteOther = 0x0002, ExeOther = 0x0001
    };
*/

int QmlPath::permissions(QJSValue path)
{
    QFile::Permissions perm = QFileInfo(path.toString()).permissions();

    int result = 0;
    if (perm & QFile::ReadOwner) result = result | 256;     // 100 000 000
    if (perm & QFile::WriteOwner) result = result | 128;    // 010 000 000
    if (perm & QFile::ExeOwner) result = result | 64;       // 001 000 000

    if (perm & QFile::ReadGroup) result = result | 32;      // 000 100 000
    if (perm & QFile::WriteGroup) result = result | 16;     // 000 010 000
    if (perm & QFile::ExeGroup) result = result | 8;        // 000 001 000

    if (perm & QFile::ReadUser) result = result | 4;       // 000 000 100
    if (perm & QFile::WriteUser) result = result | 2;      // 000 000 010
    if (perm & QFile::ExeUser) result = result | 1;        // 000 000 001

    return result;
}

bool QmlPath::setPermissions(QJSValue path, QJSValue val)
{
    if (!val.isNumber()) return false;
    QFile::Permissions result;

    int value = val.toInt();

    if (value & 256)    result = result | QFile::ReadOwner;
    if (value & 128)    result = result | QFile::WriteOwner;
    if (value & 64)     result = result | QFile::ExeOwner;

    if (value & 32)     result = result | QFile::ReadGroup;
    if (value & 16)     result = result | QFile::WriteGroup;
    if (value & 8)      result = result | QFile::ExeGroup;

    if (value & 4)      result = result | QFile::ReadUser;
    if (value & 2)      result = result | QFile::WriteUser;
    if (value & 1)      result = result | QFile::ExeUser;

    return QFile::setPermissions(path.toString(), result);
}

QString QmlPath::toLocalFile(const QUrl &url){
    return url.toLocalFile();
}

QUrl QmlPath::urlFromLocalFile(const QString &path){
    return QUrl::fromLocalFile(path);
}

QString QmlPath::removeSlashes(QString s)
{
    if (s.length() > 0 && s[0] == '/') s = s.right(s.length()-1);
    if (s.length() > 0 && s[s.length()-1] == '/') s = s.left(s.length()-1);
    return s;
}

}// namespace
