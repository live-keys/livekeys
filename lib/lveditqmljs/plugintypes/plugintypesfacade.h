#ifndef QPLUGINTYPESFACADE_H
#define QPLUGINTYPESFACADE_H

#include "live/lveditqmljsglobal.h"
#include <QQmlEngine>
#include <QList>

namespace lv{

class LV_EDITQMLJS_EXPORT PluginTypesFacade{

public:
    PluginTypesFacade();
    ~PluginTypesFacade();

    static bool pluginTypesEnabled();

    static void extractTypes(
        const QString& module,
        QQmlEngine* engine,
        QList<const QQmlType*>& types,
        QHash<QByteArray, QSet<const QQmlType *> >& qmlTypesByCppName
    );

    static bool isModule(const QString& uri);

    static void getTypeDependencies(
        const QString &module,
        const QList<const QQmlType*>& types,
        const QHash<QByteArray, QSet<const QQmlType *> >& qmlTypesByCppName,
        QSet<const QMetaObject *> &solvedTypes,
        QList<const QMetaObject *> &unknownTypes,
        QStringList& dependencies
    );

    static QString getTypeName(const QQmlType* type);

    static void extractPluginInfo(
        const QSet<const QMetaObject *> &metaTypes,
        const QHash<QByteArray, QSet<const QQmlType *> >& qmlTypesByCppName,
        const QList<QString>& dependencies,
        QByteArray* stream
    );

};

}// namespace

#endif // QPLUGINTYPESFACADE_H
