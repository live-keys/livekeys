#include "path.h"
#include <QFileInfo>
#include <QJSValueIterator>
#include <QDebug>
#include <QFile>

namespace lv{

Path::Path(QObject *parent)
    : QObject(parent)
{
}

QString Path::name(const QString &path){
    return QFileInfo(path).fileName();
}

QString Path::join(QJSValue part1, QJSValue part2)
{
    QString result = "";
    if (part1.isArray())
    {
        const int length = part1.property("length").toInt();
        for (int i = 0; i < length; ++i) {
            if (result != "") result += "/";
            result += part1.property(i).toString();
        }
        return result;
    }

    if (part1.isString())
    {
        result += part1.toString();
        if (part2.isString()) result += "/" + part2.toString();
    }
    return result;
}

bool Path::exists(QJSValue path)
{
    if (path.isString())
        return QFileInfo::exists(path.toString());

    return false;
}

QString Path::baseName(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).baseName();

    return "";
}

QString Path::suffix(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).suffix();

    return "";
}

QString Path::completeSuffix(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).completeSuffix();

    return "";
}

QString Path::absolutePath(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).absoluteFilePath();

    return "";
}

bool Path::isReadable(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isReadable();

    return false;
}

bool Path::isWritable(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isWritable();

    return false;
}

bool Path::isExecutable(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isExecutable();

    return false;
}

bool Path::isHidden(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isHidden();

    return false;
}

bool Path::isNativePath(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isNativePath();

    return false;
}

bool Path::isRelative(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isRelative();

    return false;
}

bool Path::isAbsolute(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isAbsolute();

    return false;
}

bool Path::isFile(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isFile();

    return false;
}

bool Path::isDir(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isDir();

    return false;
}

bool Path::isSymLink(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isSymLink();

    return false;
}

bool Path::isRoot(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isRoot();

    return false;
}

bool Path::isBundle(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).isBundle();

    return false;
}

QDateTime Path::lastModified(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).lastModified();

    return QDateTime();
}

QDateTime Path::created(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).created();

    return QDateTime();
}

QString Path::owner(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).owner();

    return "";
}

uint Path::ownerId(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).ownerId();

    return 1;
}

QString Path::group(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).group();

    return "";
}

uint Path::groupId(QJSValue path)
{
    if (path.isString())
        return QFileInfo(path.toString()).groupId();

    return 1;
}

QString Path::parent(QJSValue path)
{
    if (isFile(path))
    {
        QDir dir = QFileInfo(path.toString()).dir();
        dir.cdUp();
        return dir.path();
    }


    QString p = path.toString();
    if (p.back() == '/') p = p.left(p.length() - 1);
    return QFileInfo(p).dir().path();
}

QString Path::followSymLink(QJSValue symLink)
{
    return QFileInfo(symLink.toString()).symLinkTarget();
}

bool Path::createSymLink(QJSValue symLinkPath, QJSValue path)
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

int Path::permissions(QJSValue path)
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

bool Path::setPermissions(QJSValue path, QJSValue val)
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

}// namespace
