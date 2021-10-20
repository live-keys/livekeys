#include "tablegroup.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"
#include "tablemodel.h"

namespace lv{

TableGroup::TableGroup(QObject *parent)
    : QObject(parent)
    , m_table(nullptr)
    , m_isComponentComplete(false)
{
}

TableGroup::~TableGroup()
{
    //TODO: Table deletion causes exception
//    if ( m_table )
//        Shared::unref(m_table);
}

void TableGroup::setTable(Table *table){
    if (m_table == table)
        return;

    if ( m_table ){
        //TODO
//        Shared::unref(m_table);
        disconnect(m_table->model(), &QAbstractItemModel::rowsInserted, this, &TableGroup::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::rowsRemoved, this, &TableGroup::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::modelReset, this, &TableGroup::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::columnsInserted, this, &TableGroup::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::columnsRemoved, this, &TableGroup::__tableReset);
        disconnect(m_table->model(), &QAbstractItemModel::dataChanged, this, &TableGroup::__tableReset);
    }

    m_table = table;

    if ( m_table ){
        Shared::ref(m_table);
        connect(m_table->model(), &QAbstractItemModel::rowsInserted, this, &TableGroup::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::rowsRemoved, this, &TableGroup::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::modelReset, this, &TableGroup::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::columnsInserted, this, &TableGroup::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::columnsRemoved, this, &TableGroup::__tableReset);
        connect(m_table->model(), &QAbstractItemModel::dataChanged, this, &TableGroup::__tableReset);
    }

    emit tableChanged();
    process();

}

void TableGroup::process(){
    if ( !m_table || !m_isComponentComplete )
        return;

    ViewEngine* ve = ViewEngine::grab(this);
    if ( !ve ){
        QmlError::warnNoEngineCaptured(this, "TableGroup");
        return;
    }

    QJSValue reduceResult;
    if ( m_reduce.isCallable() ){

        if ( m_rowExtras.isUndefined() ){
            m_rowExtras = ve->engine()->evaluate("(function(index){ return parseFloat(this[index]); })");
        }

        int total = m_table->totalRows();
        if ( total == 0 )
            return;

        QJSValue accumulator = m_reduceInit.isUndefined() ? getRow(0) : m_reduceInit;
        int start = accumulator.isUndefined() ? 1 : 0;

        for ( int i = start; i < total; ++i ){
            QJSValue row = getRow(i);
            accumulator = m_reduce.call(QJSValueList() << accumulator << row);
            if ( accumulator.isError() ){
                ve->throwError(accumulator, this);
                return;
            }
        }

        reduceResult = accumulator;
    }

    if ( m_all.isCallable() ){

        QJSValue result = m_all.call(QJSValueList() << ve->engine()->newQObject(m_table->model()) << reduceResult);
        if ( result.isError() ){
            ve->throwError(result, this);
            return;
        }

        m_result = result;
        emit resultChanged();
    } else {
        m_result = reduceResult;
        emit resultChanged();
    }
}

void TableGroup::componentComplete(){
    m_isComponentComplete = true;
    process();
}

void TableGroup::__tableReset(){
    process();
}

QJSValue TableGroup::getRow(int index) const{
    QJSValue row = m_table->rowAt(index);
    row.setProperty("nrAt", m_rowExtras);
    return row;
}

}// namespace
