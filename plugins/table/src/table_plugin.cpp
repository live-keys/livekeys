#include "table_plugin.h"
#include "table.h"

#include <qqml.h>
#include <QQmlEngine>
#include <QDebug>

void TablePlugin::registerTypes(const char *uri)
{
    qmlRegisterType<lv::Table>(uri, 1, 0, "Table");
}
