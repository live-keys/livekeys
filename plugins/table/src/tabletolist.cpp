#include "tabletolist.h"
#include "tablemodel.h"
#include "tableschema.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"
#include "live/shared.h"

namespace lv{

TableToList::TableToList(QObject *parent)
    : QObject(parent)
    , m_table(nullptr)
    , m_result(QJSValue(QJSValue::NullValue))
{
}

TableToList::~TableToList(){
    //TODO
//    if ( m_table )
    //        Shared::unref(m_table);
}

void TableToList::process(){
    if ( !m_table )
        return;

    ViewEngine* ve = ViewEngine::grab(this);
    if ( !ve ){
       qWarning("TableToList: Failed to grab view engine. This object will not work.");
       return;
    }

    int total = m_table->totalRows();
    m_result = ve->engine()->newArray(total);

    QList<QString> fields;
    for ( int i = 0; i < m_table->schema()->totalFields(); ++i ){
        TableField::Ptr tf = m_table->schema()->fieldAt(i);
        fields.append(tf->name());
    }

    for ( int i = 0; i < total; ++i ){
        QJSValue row = m_table->rowAt(i);

        QJSValue rowObject = ve->engine()->newObject();
        for ( int j = 0; j < fields.length(); ++j ){
            rowObject.setProperty(fields[j], row.property(j));
        }
        m_result.setProperty(i, rowObject);
    }

    emit resultChanged();
}

void TableToList::setTable(Table *table){
    if (m_table == table)
        return;

    if ( m_table ){
        //TODO
//        Shared::unref(m_table);
        disconnect(m_table->model(), &QAbstractItemModel::rowsInserted, this, &TableToList::__tableRowsInserted);
        disconnect(m_table->model(), &QAbstractItemModel::rowsRemoved, this, &TableToList::__tableRowsRemoved);
        disconnect(m_table->model(), &QAbstractItemModel::modelReset, this, &TableToList::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::columnsInserted, this, &TableToList::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::columnsRemoved, this, &TableToList::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::dataChanged, this, &TableToList::__tableDataChanged);
    }

    m_table = table;

    if ( m_table ){
        Shared::ref(m_table);
        connect(m_table->model(), &QAbstractItemModel::rowsInserted, this, &TableToList::__tableRowsInserted);
        connect(m_table->model(), &QAbstractItemModel::rowsRemoved, this, &TableToList::__tableRowsRemoved);
        connect(m_table->model(), &QAbstractItemModel::modelReset, this, &TableToList::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::columnsInserted, this, &TableToList::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::columnsRemoved, this, &TableToList::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::dataChanged, this, &TableToList::__tableDataChanged);
    }

    m_table = table;
    emit tableChanged();

    process();
}

void TableToList::__tableRowsInserted(const QModelIndex &, int, int){
    process();
}

void TableToList::__tableRowsRemoved(const QModelIndex &, int, int){
    process();
}

void TableToList::__tableDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &){
    process();
}

void TableToList::__tableReset(){
    process();
}

}// namespace
