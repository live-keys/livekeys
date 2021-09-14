#include "table_plugin.h"
#include "tablemodel.h"
#include "tablemodelheader.h"
#include "tablemodelrowsinfo.h"
#include "table.h"
#include "localtable.h"
#include "tableroweach.h"
#include "tablegroup.h"

#include <qqml.h>
#include <QQmlEngine>
#include <QDebug>

void TablePlugin::registerTypes(const char *uri)
{
    qmlRegisterType<lv::LocalTable>(  uri, 1, 0, "LocalTable");
    qmlRegisterType<lv::TableRowEach>(uri, 1, 0, "TableRowEach");
    qmlRegisterType<lv::TableGroup>(  uri, 1, 0, "TableGroup");

    qmlRegisterUncreatableType<lv::TableModelHeader>(
        uri, 1, 0, "TableModelHeader", "Cannot create TableModelHeader instance.");
    qmlRegisterUncreatableType<lv::TableModelRowsInfo>(
        uri, 1, 0, "TableModelRowsInfo", "Cannot create TableModelRowsInfo instance.");
    qmlRegisterUncreatableType<lv::TableModel>(
        uri, 1, 0, "TableModel", "Cannot create TableModel instance. Use 'table.model'.");
    qmlRegisterUncreatableType<lv::Table>(
        uri, 1, 0, "Table", "Table is of abstract type.");
}
