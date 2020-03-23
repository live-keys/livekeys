#ifndef QPLUGINTYPESFACADE_H
#define QPLUGINTYPESFACADE_H

#include "live/lveditqmljsglobal.h"
#include <QQmlEngine>
#include <QList>
#include "projectqmlscope.h"

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
            QList<QQmlType> &,
            QHash<QByteArray, QSet<QQmlType> > &);

    static bool isModule(const QString& uri);

    static void getTypeDependencies(
            const QString &,
            const QList<QQmlType> &,
            const QHash<QByteArray, QSet<QQmlType> >& ,
            QSet<const QMetaObject *> &,
            QList<const QMetaObject *> &,
            QStringList &);

    static QmlLibraryInfo::ScanStatus loadPluginInfo(
            QmlLanguageScanner *scanner,
            QmlLibraryInfo::Ptr lib,
            QQmlEngine *engine,
            QByteArray *stream);

    static QString getTypeName(const QQmlType* type);

    static void extractPluginInfo(
            const QSet<const QMetaObject *> &,
            const QHash<QByteArray, QSet<QQmlType> > &,
            const QList<QString> &,
            QByteArray *);

};

}// namespace

#endif // QPLUGINTYPESFACADE_H
