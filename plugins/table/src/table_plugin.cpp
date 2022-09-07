#include "table_plugin.h"
#include "tablemodel.h"
#include "tablemodelheader.h"
#include "tablemodelrowsinfo.h"
#include "table.h"
#include "localtable.h"
#include "tableroweach.h"
#include "tablegroup.h"
#include "tableconcat.h"
#include "tablecolumn.h"
#include "tabletolist.h"

#include <qqml.h>
#include <QQmlEngine>
#include <QDebug>

void TablePlugin::registerTypes(const char *uri)
{
    lv::QmlMetaExtension::add<lv::LocalTable, lv::MetaSerializableI>(lv::LocalTable::serialize, lv::LocalTable::deserialize);

    qmlRegisterType<lv::LocalTable>(  uri, 1, 0, "LocalTable");
    qmlRegisterType<lv::TableRowEach>(uri, 1, 0, "TableRowEach");
    qmlRegisterType<lv::TableGroup>(  uri, 1, 0, "TableGroup");
    qmlRegisterType<lv::TableConcat>( uri, 1, 0, "TableConcat");
    qmlRegisterType<lv::TableColumn>( uri, 1, 0, "TableColumn");
    qmlRegisterType<lv::TableToList>( uri, 1, 0, "TableToList");

    qmlRegisterUncreatableType<lv::TableModelHeader>(
        uri, 1, 0, "TableModelHeader", "Cannot create TableModelHeader instance.");
    qmlRegisterUncreatableType<lv::TableModelRowsInfo>(
        uri, 1, 0, "TableModelRowsInfo", "Cannot create TableModelRowsInfo instance.");
    qmlRegisterUncreatableType<lv::TableModel>(
        uri, 1, 0, "TableModel", "Cannot create TableModel instance. Use 'table.model'.");
    qmlRegisterUncreatableType<lv::Table>(
        uri, 1, 0, "Table", "Table is of abstract type.");
}
