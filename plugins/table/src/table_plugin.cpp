#include "table_plugin.h"
#include "qtable.h"

#include <qqml.h>
#include <QQmlEngine>
#include <QDebug>

void TablePlugin::registerTypes(const char *uri)
{
    qmlRegisterType<QTable>(uri, 1, 0, "Table");
}
