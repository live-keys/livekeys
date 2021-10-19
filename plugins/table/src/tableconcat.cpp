#include "tableconcat.h"
#include "tablemodel.h"
#include "tableschema.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"

#include <QJSValueIterator>

namespace lv{

TableConcat::TableConcat(QObject *parent)
    : Table(parent)
    , m_table1(nullptr)
    , m_table2(nullptr)
{
    setSchema(new TableSchema);
}

TableConcat::~TableConcat(){
    //TODO: Table deletion causes exception
//    if ( m_table1 )
//        Shared::unref(m_table1);
//    if ( m_table2 )
//        Shared::unref(m_table1);
}

void TableConcat::setTable1(Table *table){
    if (m_table1 == table)
        return;

    if ( m_table1 ){
        //TODO
//        Shared::unref(m_table1);
        disconnect(m_table1->model(), &QAbstractItemModel::rowsInserted, this, &TableConcat::__table1RowsInserted);
        disconnect(m_table1->model(), &QAbstractItemModel::rowsRemoved, this, &TableConcat::__table1RowsRemoved);
        disconnect(m_table1->model(), &QAbstractItemModel::modelReset, this, &TableConcat::__table1Reset);
        disconnect(m_table1->model(), &QAbstractItemModel::columnsInserted, this, &TableConcat::__table1Reset);
        disconnect(m_table1->model(), &QAbstractItemModel::columnsRemoved, this, &TableConcat::__table1Reset);
        disconnect(m_table1->model(), &QAbstractItemModel::dataChanged, this, &TableConcat::__table1DataChanged);
    }

    m_table1 = table;

    if ( m_table1 ){
        Shared::ref(m_table1);
        connect(m_table1->model(), &QAbstractItemModel::rowsInserted, this, &TableConcat::__table1RowsInserted);
        connect(m_table1->model(), &QAbstractItemModel::rowsRemoved, this, &TableConcat::__table1RowsRemoved);
        connect(m_table1->model(), &QAbstractItemModel::modelReset, this, &TableConcat::__table1Reset);
        connect(m_table1->model(), &QAbstractItemModel::columnsInserted, this, &TableConcat::__table1Reset);
        connect(m_table1->model(), &QAbstractItemModel::columnsRemoved, this, &TableConcat::__table1Reset);
        connect(m_table1->model(), &QAbstractItemModel::dataChanged, this, &TableConcat::__table1DataChanged);
    }

    emit table1Changed();
    process();
}

void TableConcat::setTable2(Table *table){
    if (m_table2 == table)
        return;

    if ( m_table2 ){
        //TODO
    //        Shared::unref(m_table2);
        disconnect(m_table2->model(), &QAbstractItemModel::rowsInserted, this, &TableConcat::__table2RowsInserted);
        disconnect(m_table2->model(), &QAbstractItemModel::rowsRemoved, this, &TableConcat::__table2RowsRemoved);
        disconnect(m_table2->model(), &QAbstractItemModel::modelReset, this, &TableConcat::__table2Reset);
        disconnect(m_table2->model(), &QAbstractItemModel::columnsInserted, this, &TableConcat::__table2Reset);
        disconnect(m_table2->model(), &QAbstractItemModel::columnsRemoved, this, &TableConcat::__table2Reset);
        disconnect(m_table2->model(), &QAbstractItemModel::dataChanged, this, &TableConcat::__table2DataChanged);
    }

    m_table2 = table;

    if ( m_table2 ){
        Shared::ref(m_table2);
        connect(m_table2->model(), &QAbstractItemModel::rowsInserted, this, &TableConcat::__table2RowsInserted);
        connect(m_table2->model(), &QAbstractItemModel::rowsRemoved, this, &TableConcat::__table2RowsRemoved);
        connect(m_table2->model(), &QAbstractItemModel::modelReset, this, &TableConcat::__table2Reset);
        connect(m_table2->model(), &QAbstractItemModel::columnsInserted, this, &TableConcat::__table2Reset);
        connect(m_table2->model(), &QAbstractItemModel::columnsRemoved, this, &TableConcat::__table2Reset);
        connect(m_table2->model(), &QAbstractItemModel::dataChanged, this, &TableConcat::__table2DataChanged);
    }

    emit table2Changed();
    process();
}

void TableConcat::process(){
    if ( !m_table1 || !m_table2 )
        return;

    int total1 = m_table1->totalRows();
    int total2 = m_table2->totalRows();

    if ( isModelSet() )
        model()->notifyTableRefresh();

    // Update schema

    schema()->clearFields();

    for ( int i = 0; i < m_table1->schema()->totalFields(); ++i ){
        TableField::Ptr tf = m_table1->schema()->fieldAt(i);

        TableField::Ptr newTf = TableField::create(tf->name());
        schema()->insertField(schema()->totalFields(), newTf);
    }
    for ( int i = 0; i < m_table2->schema()->totalFields(); ++i ){
        TableField::Ptr tf = m_table2->schema()->fieldAt(i);

        TableField::Ptr newTf = TableField::create(tf->name());
        schema()->insertField(schema()->totalFields(), newTf);
    }

    // Populate fields

    int total = total1 < total2 ? total1 : total2;

    for ( int i = 0; i < total; ++i ){
        QList<QString> fields;

        QJSValue row1 = m_table1->rowAt(i);
        QJSValue row2 = m_table2->rowAt(i);

        QJSValueIterator it1(row1);
        while ( it1.hasNext() ){
            it1.next();
            if ( it1.name() != "length" ){
                fields.append(it1.value().toString());
            }
        }

        QJSValueIterator it2(row1);
        while ( it2.hasNext() ){
            it2.next();
            if ( it2.name() != "length" ){
                fields.append(it2.value().toString());
            }
        }

        m_data.append(fields);
    }


    if ( isModelSet() )
        model()->notifyTableRefreshEnd();
}

QString TableConcat::valueAt(int row, int column){
    auto& rowData = m_data[row];
    return rowData[column];
}

int TableConcat::totalRows() const{
    return m_data.size();
}

QJSValue TableConcat::rowAt(int index){
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

void TableConcat::insertField(int, TableField::Ptr){
    throwTableReadOnly();
}

void TableConcat::updateField(int, const QJSValue &){
    throwTableReadOnly();
}

void TableConcat::removeField(int){
    throwTableReadOnly();
}

void TableConcat::__table1RowsInserted(const QModelIndex &, int, int){
    process();
}

void TableConcat::__table1RowsRemoved(const QModelIndex &, int, int){
    process();
}

void TableConcat::__table1DataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &){
    process();
}

void TableConcat::__table1Reset(){
    process();
}

void TableConcat::__table2RowsInserted(const QModelIndex &, int, int){
    process();
}

void TableConcat::__table2RowsRemoved(const QModelIndex &, int, int){
    process();
}

void TableConcat::__table2DataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &){
    process();
}

void TableConcat::__table2Reset(){
    process();
}

void TableConcat::insertRow(const QJSValue &, Table::InsertCallback){
    throwTableReadOnly();
}

void TableConcat::updateRow(int, const QJSValue &){
    throwTableReadOnly();
}

void TableConcat::removeRow(int){
    throwTableReadOnly();
}

void TableConcat::throwTableReadOnly(){
    ViewEngine* ve = ViewEngine::grab(this);
    if ( !ve ){
       qWarning("TableConcat: Failed to grab view engine. Errors will not report.");
       return;
    }
    Exception e = CREATE_EXCEPTION(lv::Exception, "TableConcat: Table is readonly.", Exception::toCode("~ReadOnly"));
    ve->throwError(&e, this);
}

} // namespace
