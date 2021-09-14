#include "tablegroup.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/visuallogqt.h"
#include "tablemodel.h"

namespace lv{

TableGroup::TableGroup(QObject *parent)
    : QObject(parent)
    , m_table(nullptr)
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
    if ( m_fn.isCallable() && m_table ){
        ViewEngine* ve = ViewEngine::grab(this);
        if ( !ve ){
            qWarning("TableGroup: Failed to grab view engine. This object will not work.");
            return;
        }

        QJSValue result = m_fn.call(QJSValueList() << ve->engine()->newQObject(m_table->model()));
        if ( result.isError() ){
            ve->throwError(result, this);
            return;
        }

        m_result = result;
        emit resultChanged();
    }
}

void TableGroup::__tableReset(){
    process();
}

}// namespace
