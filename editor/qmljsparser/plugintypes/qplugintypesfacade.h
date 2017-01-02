#ifndef QPLUGINTYPESFACADE_HPP
#define QPLUGINTYPESFACADE_HPP

#include "qqmljsparserglobal.h"
#include <QQmlEngine>
#include <QList>

namespace lcv{

class Q_QMLJSPARSER_EXPORT QPluginTypesFacade{

public:
    QPluginTypesFacade();
    ~QPluginTypesFacade();

    static void extractTypes(
        const QString& module,
        QQmlEngine* engine,
        QList<const QQmlType*>& types,
        QHash<QByteArray, QSet<const QQmlType *> >& qmlTypesByCppName
    );
    static void getTypeDependencies(
        const QString &module,
        const QList<const QQmlType*>& types,
        const QHash<QByteArray, QSet<const QQmlType *> >& qmlTypesByCppName,
        QList<const QMetaObject *> &unknownTypes,
        QStringList& dependencies
    );
    static QString getTypeName(const QQmlType* type);
    static void extractPluginInfo(
        const QSet<const QMetaObject*> metaTypes,
        const QHash<QByteArray, QSet<const QQmlType *> >& qmlTypesByCppName,
        QByteArray* stream
    );

};

}// namespace

#endif // QPLUGINTYPESFACADE_HPP
