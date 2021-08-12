#include "table_plugin.h"
#include "table.h"
#include "tableheader.h"
#include "tablerowsinfo.h"

#include <qqml.h>
#include <QQmlEngine>
#include <QDebug>

void TablePlugin::registerTypes(const char *uri)
{
    qmlRegisterType<lv::Table>(uri, 1, 0, "Table");
    qmlRegisterUncreatableType<lv::TableHeader>(uri, 1, 0, "TableHeader", "Cannot create TableHeader instance.");
    qmlRegisterUncreatableType<lv::TableRowsInfo>(uri, 1, 0, "TableRows", "Cannot create TableRows instance.");
}
