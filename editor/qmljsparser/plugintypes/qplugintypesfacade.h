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

    static QList<const QQmlType*> extractTypes(const QString& module, QQmlEngine* engine);
    static QString getTypeName(const QQmlType* type);
    static void extractPluginInfo(QByteArray* stream);

};

}// namespace

#endif // QPLUGINTYPESFACADE_HPP
