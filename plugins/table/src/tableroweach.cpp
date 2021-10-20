#include "tableroweach.h"
#include "tablemodel.h"
#include "tableschema.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"

namespace lv{

TableRowEach::TableRowEach(QObject *parent)
    : Table(parent)
    , m_table(nullptr)
{
    setSchema(new TableSchema);
    schema()->insertField(0, TableField::create("Result"));
}

TableRowEach::~TableRowEach(){
    //TODO: Table deletion causes exception
//    if ( m_table )
//        Shared::unref(m_table);
}

void TableRowEach::setTable(Table *table){
    if (m_table == table)
        return;

    if ( m_table ){
        //TODO
//        Shared::unref(m_table);
        disconnect(m_table->model(), &QAbstractItemModel::rowsInserted, this, &TableRowEach::__tableRowsInserted);
        disconnect(m_table->model(), &QAbstractItemModel::rowsRemoved, this, &TableRowEach::__tableRowsRemoved);
        disconnect(m_table->model(), &QAbstractItemModel::modelReset, this, &TableRowEach::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::columnsInserted, this, &TableRowEach::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::columnsRemoved, this, &TableRowEach::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::dataChanged, this, &TableRowEach::__tableDataChanged);
    }

    m_table = table;

    if ( m_table ){
        Shared::ref(m_table);
        connect(m_table->model(), &QAbstractItemModel::rowsInserted, this, &TableRowEach::__tableRowsInserted);
        connect(m_table->model(), &QAbstractItemModel::rowsRemoved, this, &TableRowEach::__tableRowsRemoved);
        connect(m_table->model(), &QAbstractItemModel::modelReset, this, &TableRowEach::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::columnsInserted, this, &TableRowEach::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::columnsRemoved, this, &TableRowEach::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::dataChanged, this, &TableRowEach::__tableDataChanged);
    }

    emit tableChanged();
    process();
}

void TableRowEach::process(){
    if ( m_fn.isCallable() && m_table ){
        ViewEngine* ve = ViewEngine::grab(this);
        if ( !ve ){
           qWarning("TableRowEach: Failed to grab view engine. This object will not work.");
           return;
        }

        if ( m_rowExtras.isUndefined() ){
            m_rowExtras = ve->engine()->evaluate("(function(index){ return parseFloat(this[index]); })");
        }

        int total = m_table->totalRows();

        if ( isModelSet() )
            model()->notifyTableRefresh();

        m_data.clear();

        for ( int i = 0; i < total; ++i ){
            QJSValue row = m_table->rowAt(i);

            row.setProperty("nrAt", m_rowExtras);

            QJSValue result = m_fn.call(QJSValueList() << row);
            if ( result.isError() ){
                ve->throwError(result, this);
                m_data.clear();
                return;
            }
            m_data.append(QList<QString>() << result.toString());
        }


        if ( isModelSet() )
            model()->notifyTableRefreshEnd();
    }
}

QString TableRowEach::valueAt(int row, int column){
    auto& rowData = m_data[row];
    return rowData[column];
}

int TableRowEach::totalRows() const{
    return m_data.size();
}

QJSValue TableRowEach::rowAt(int index){
    ViewEngine* ve = ViewEngine::grab(this);
    if (!ve)
        return QJSValue();
    if (index >= m_data.length() )
        return QJSValue();

    auto& rowData = m_data[index];
    QJSValue result = ve->engine()->newArray(rowData.length());
    for ( int i = 0; i < rowData.length(); ++i ){
        result.setProperty(i, rowData[i]);
    }
    return result;
}

void TableRowEach::insertField(int, TableField::Ptr){
    throwTableReadOnly();
}

void TableRowEach::updateField(int, const QJSValue &){
    throwTableReadOnly();
}

void TableRowEach::removeField(int){
    throwTableReadOnly();
}

void TableRowEach::__tableRowsInserted(const QModelIndex &, int, int){
    process();
}

void TableRowEach::__tableRowsRemoved(const QModelIndex &, int, int){
    process();
}

void TableRowEach::__tableDataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &){
    process();
}

void TableRowEach::__tableReset(){
    process();
}

void TableRowEach::insertRow(const QJSValue &, Table::InsertCallback){
    throwTableReadOnly();
}

void TableRowEach::updateRow(int, const QJSValue &){
    throwTableReadOnly();
}

void TableRowEach::removeRow(int){
    throwTableReadOnly();
}

void TableRowEach::throwTableReadOnly(){
    ViewEngine* ve = ViewEngine::grab(this);
    if ( !ve ){
       qWarning("TableRowEach: Failed to grab view engine. Errors will not report.");
       return;
    }
    Exception e = CREATE_EXCEPTION(lv::Exception, "TableRowEach: Table is readonly.", Exception::toCode("~ReadOnly"));
    ve->throwError(&e, this);
}

} // namespace
