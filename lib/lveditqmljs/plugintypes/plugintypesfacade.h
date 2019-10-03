#ifndef QPLUGINTYPESFACADE_H
#define QPLUGINTYPESFACADE_H

#include "live/lveditqmljsglobal.h"
#include <QQmlEngine>
#include <QList>
#include "qmllibraryinfo_p.h"
#include "projectqmlscope.h"
#include "projectqmlscanner_p.h"

namespace lv{

/// \private
class PluginTypesFacade{

public:
    PluginTypesFacade();
    ~PluginTypesFacade();

    static bool pluginTypesEnabled();

    static void extractTypes(
        const QString& module,
        QQmlEngine* engine,
        QList<QQmlType> &types,
        QHash<QByteArray, QSet<QQmlType> > &qmlTypesByCppName
    );

    static bool isModule(const QString& uri);

    static void getTypeDependencies(
        const QString &module,
        const QList<QQmlType> &types,
        const QHash<QByteArray, QSet<QQmlType> >& qmlTypesByCppName,
        QSet<const QMetaObject *> &solvedTypes,
        QList<const QMetaObject *> &unknownTypes,
        QStringList& dependencies
    );

    static QmlLibraryInfo::ScanStatus loadPluginInfo(
            ProjectQmlScope::Ptr projectScope,
            const QmlDirParser& dirParser,
            const QString& path,
            ProjectQmlScanner* scanner,
            QStringList& dependencyPaths,
            QByteArray* stream);

    static QString getTypeName(const QQmlType* type);

    static void extractPluginInfo(
        const QSet<const QMetaObject *> &metaTypes,
        const QHash<QByteArray, QSet<QQmlType> > &qmlTypesByCppName,
        const QList<QString>& dependencies,
        QByteArray* stream
    );

};

}// namespace

#endif // QPLUGINTYPESFACADE_H
