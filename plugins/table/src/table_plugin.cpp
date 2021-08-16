#include "table_plugin.h"
#include "table.h"
#include "tableheader.h"
#include "tablerowsinfo.h"
#include "tabledatasource.h"
#include "localdatasource.h"

#include <qqml.h>
#include <QQmlEngine>
#include <QDebug>

void TablePlugin::registerTypes(const char *uri)
{
    qmlRegisterType<lv::Table>(uri, 1, 0, "Table");
    qmlRegisterUncreatableType<lv::TableHeader>(uri, 1, 0, "TableHeader", "Cannot create TableHeader instance.");
    qmlRegisterUncreatableType<lv::TableRowsInfo>(uri, 1, 0, "TableRows", "Cannot create TableRows instance.");

    qmlRegisterUncreatableType<lv::TableDataSource>(uri, 1, 0, "TableDataSource", "TableDataSource is of abstract type.");
    qmlRegisterType<lv::LocalDataSource>(uri, 1, 0, "LocalDataSource");
}
