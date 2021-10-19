#include "tablecolumn.h"
#include "tablemodel.h"
#include "tableschema.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"

namespace lv{

TableColumn::TableColumn(QObject *parent)
    : Table(parent)
    , m_table(nullptr)
{
    setSchema(new TableSchema);
    schema()->insertField(0, TableField::create("Result"));
}

TableColumn::~TableColumn(){
    //TODO: Table deletion causes exception
//    if ( m_table )
//        Shared::unref(m_table);
}

void TableColumn::setTable(Table *table){
    if (m_table == table)
        return;

    if ( m_table ){
        //TODO
//        Shared::unref(m_table);
        disconnect(m_table->model(), &QAbstractItemModel::rowsInserted, this, &TableColumn::__tableRowsInserted);
        disconnect(m_table->model(), &QAbstractItemModel::rowsRemoved, this, &TableColumn::__tableRowsRemoved);
        disconnect(m_table->model(), &QAbstractItemModel::modelReset, this, &TableColumn::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::columnsInserted, this, &TableColumn::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::columnsRemoved, this, &TableColumn::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::dataChanged, this, &TableColumn::__tableDataChanged);
    }

    m_table = table;

    if ( m_table ){
        Shared::ref(m_table);
        connect(m_table->model(), &QAbstractItemModel::rowsInserted, this, &TableColumn::__tableRowsInserted);
        connect(m_table->model(), &QAbstractItemModel::rowsRemoved, this, &TableColumn::__tableRowsRemoved);
        connect(m_table->model(), &QAbstractItemModel::modelReset, this, &TableColumn::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::columnsInserted, this, &TableColumn::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::columnsRemoved, this, &TableColumn::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::dataChanged, this, &TableColumn::__tableDataChanged);
    }

    emit tableChanged();
    process();
}

void TableColumn::process(){
    if ( !m_table )
        return;
    if ( !m_column.isString() || !m_column.isNumber() )
        return;

    ViewEngine* ve = ViewEngine::grab(this);
    if ( !ve ){
       qWarning("TableColumn: Failed to grab view engine. This object will not work.");
       return;
    }

    int columnIndex = -1;

    if ( m_column.isString() ){
        int ci = m_table->schema()->fieldIndex(m_column.toString());
        if ( ci == -1 ){
            Exception e = CREATE_EXCEPTION(lv::Exception, "TableColumn: Column '" + m_column.toString().toStdString() + "' not found.", Exception::toCode("~ReadOnly"));
            ve->throwError(&e, this);
        }
    } else if ( m_column.isNumber() ){
        columnIndex = m_column.toInt();
    }

    if ( columnIndex == -1 )
        return;

    int total = m_table->totalRows();
    if ( isModelSet() )
        model()->notifyTableRefresh();

    m_data.clear();

    for ( int i = 0; i < total; ++i ){
        QJSValue row = m_table->rowAt(i);
        m_data.append(row.property(columnIndex).toString());
    }


    if ( isModelSet() )
        model()->notifyTableRefreshEnd();
}

QString TableColumn::valueAt(int row, int){
    return m_data[row];
}

int TableColumn::totalRows() const{
    return m_data.size();
}

QJSValue TableColumn::rowAt(int index){
    ViewEngine* ve = ViewEngine::grab(this);
    if (!ve)
        return QJSValue();
    if (index >= m_data.length() )
        return QJSValue();

    auto& rowData = m_data[index];
    QJSValue result = ve->engine()->newArray(rowData.length());
    result.setProperty(0, rowData);
    return result;
}

void TableColumn::insertField(int, TableField::Ptr){
    throwTableReadOnly();
}

void TableColumn::updateField(int, const QJSValue &){
    throwTableReadOnly();
}

void TableColumn::removeField(int){
    throwTableReadOnly();
}

void TableColumn::__tableRowsInserted(const QModelIndex &, int, int){
    process();
}

void TableColumn::__tableRowsRemoved(const QModelIndex &, int, int){
    process();
}

void TableColumn::__tableDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &){
    process();
}

void TableColumn::__tableReset(){
    process();
}

void TableColumn::insertRow(const QJSValue &, Table::InsertCallback){
    throwTableReadOnly();
}

void TableColumn::updateRow(int, const QJSValue &){
    throwTableReadOnly();
}

void TableColumn::removeRow(int){
    throwTableReadOnly();
}

void TableColumn::throwTableReadOnly(){
    ViewEngine* ve = ViewEngine::grab(this);
    if ( !ve ){
       qWarning("TableRowEach: Failed to grab view engine. Errors will not report.");
       return;
    }
    Exception e = CREATE_EXCEPTION(lv::Exception, "TableRowEach: Table is readonly.", Exception::toCode("~ReadOnly"));
    ve->throwError(&e, this);
}

}// namespace
