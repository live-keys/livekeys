#ifndef LVPATH_H
#define LVPATH_H

#include <QObject>
#include <QJSValue>
#include <QDateTime>
#include <QDir>

namespace lv{

/// \private
class QmlPath : public QObject{

    Q_OBJECT

public:
    explicit QmlPath(QObject *parent = nullptr);

public slots:
    QString name(const QString& path);
    QString join(QJSValue part1, QJSValue part2 = QJSValue());
    bool exists(QJSValue path);
    QString baseName(QJSValue path);
    QString suffix(QJSValue path);
    QString completeSuffix(QJSValue path);
    QString absolutePath(QJSValue path);

    bool isReadable(QJSValue path);
    bool isWritable(QJSValue path);
    bool isExecutable(QJSValue path);
    bool isHidden(QJSValue path);
    bool isNativePath(QJSValue path);
    bool isRelative(QJSValue path);
    bool isAbsolute(QJSValue path);

    bool isFile(QJSValue path);
    bool isDir(QJSValue path);
    bool isSymLink(QJSValue path);
    bool isRoot(QJSValue path);
    bool isBundle(QJSValue path);

    QDateTime lastModified(QJSValue path);
    QDateTime created(QJSValue path);

    QString owner(QJSValue path);
    uint ownerId(QJSValue path);
    QString group(QJSValue path);
    uint groupId(QJSValue path);

    QString parent(QJSValue path);
    QString followSymLink(QJSValue symLink);
    bool createSymLink(QJSValue symLinkPath, QJSValue path);

    int permissions(QJSValue path);
    bool setPermissions(QJSValue path, QJSValue val);

    static QString removeSlashes(QString s);
};

}// namespace

#endif // LVPATH_H
