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
            const QString &,
            QQmlEngine *,
            QList<const QQmlType *> &,
            QHash<QByteArray, QSet<const QQmlType *> > &);

    static bool isModule(const QString& uri);

    static void getTypeDependencies(
            const QString &,
            const QList<const QQmlType *> &,
            const QHash<QByteArray, QSet<const QQmlType *> > &,
            QSet<const QMetaObject *> &,
            QList<const QMetaObject *> &,
            QStringList &);

    static QmlLibraryInfo::ScanStatus loadPluginInfo(
            ProjectQmlScope::Ptr projectScope,
            const QmlDirParser& dirParser,
            const QString& path,
            ProjectQmlScanner* scanner,
            QStringList& dependencyPaths,
            QByteArray* stream) { return QmlLibraryInfo::ScanStatus::NotScanned; }

    static QString getTypeName(const QQmlType* type);

    static void extractPluginInfo(
            const QSet<const QMetaObject *> &,
            const QHash<QByteArray, QSet<const QQmlType *> > &,
            const QList<QString> &,
            QByteArray *);

};

}// namespace

#endif // QPLUGINTYPESFACADE_H
